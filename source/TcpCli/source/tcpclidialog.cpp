#include "tcpclidialog.h"
#include "ui_tcpclidialog.h"
#include <QMessageBox>
#include "widgetutil.h"
#include <QSettings>
#include <QRegExpValidator>
#include <QRegExp>
#include <QDateTime>
#include <QUrl>
#include <QDesktopServices>
#include <QHostAddress>


TcpCliDialog::TcpCliDialog(QDialog *parent) :
	QDialog(parent),
	ui(new Ui::TcpCliDialog)
{
    ui->setupUi(this);
	
	connect(ui->btnOpen, SIGNAL(clicked()), this, SLOT(onClkBtnOpen()));
	connect(ui->btnClear, SIGNAL(clicked()), this, SLOT(onClkBtnClear()));
	connect(ui->btnAutoSend, SIGNAL(clicked()), this, SLOT(onClkBtnAutoSend()));
	connect(ui->btnAbout, SIGNAL(clicked()), this, SLOT(onClkBtnAbout()));
	connect(ui->btnHome, SIGNAL(clicked()), this, SLOT(onClkBtnHome()));
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	Init();
	onDisConnected();
}

TcpCliDialog::~TcpCliDialog()
{
    delete ui;
}

void TcpCliDialog::Init()
{
	m_bConnected = false;
	m_bAutoSend = false;
	
	m_sg[0].leSend = ui->leSend0;
	m_sg[0].btnSend = ui->btnSend0;
	
	m_sg[1].leSend = ui->leSend1;
	m_sg[1].btnSend = ui->btnSend1;
	
	m_sg[2].leSend = ui->leSend2;
	m_sg[2].btnSend = ui->btnSend2;
	
	tcpSocket = new QTcpSocket(this);
	connect(tcpSocket, SIGNAL(connected()), this, SLOT(onConnected()));
	connect(tcpSocket, SIGNAL(disconnected()), this, SLOT(onDisConnected()));
	connect(tcpSocket, SIGNAL(readyRead()), this, SLOT(onRead()));
	connect(tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
		this, SLOT(onError(QAbstractSocket::SocketError)));
		
	QSettings setting;
	m_latestIpList = setting.value("TcpCliTest/LatestIP", "").toStringList();
	int nCount = m_latestIpList.count();
	for (int i = nCount-1; i >= 0; i--)
	{
		if (!m_latestIpList[i].isEmpty())
		{
			ui->cbIpAddr->addItem(m_latestIpList[i]);
		}
	}
	ui->spinPort->setValue(setting.value("TcpCliTest/LatestPort", 2404).toInt());
	
    QRegExp regExp("[A-Fa-f0-9 ]{1,}");
	QValidator *validator = new QRegExpValidator(regExp, this);
	for(int i=0; i<MAX_SENDGROUP; i++)
	{
		connect(m_sg[i].btnSend, SIGNAL(clicked()), this, SLOT(onClkBtnSend()));
		m_sg[i].leSend->setValidator(validator);
		m_sg[i].leSend->setText(setting.value(QString("TcpCliTest/SendData%1").arg(i), "").toString());
	}
}

void TcpCliDialog::onClkBtnOpen()
{
	if(m_bConnected)
	{
		tcpSocket->disconnectFromHost();
	}
	else
	{
		tcpSocket->connectToHost(ui->cbIpAddr->currentText(), ui->spinPort->value());
		m_latestIpList.removeOne(ui->cbIpAddr->currentText());
		m_latestIpList << ui->cbIpAddr->currentText();
		if (m_latestIpList.count() > 5)
		{
			m_latestIpList.removeFirst();
		}
	
		m_nSendBytes = 0;
		m_nRecvBytes = 0;
	}
}

void TcpCliDialog::onClkBtnSend()
{
	QLineEdit* leSend = NULL;
	for(int i=0; i<MAX_SENDGROUP; i++)
	{
		if(m_sg[i].btnSend == (QPushButton*)sender())
		{
			leSend = m_sg[i].leSend;
			break;
		}		
	}
	SendData(leSend);
}

void TcpCliDialog::SendData(QLineEdit* leSend)
{
	if(!leSend)	return;
	
	QString strText = leSend->text();
	QString strData;
	int nLen = strText.count();
	int m_nDataCount = 0;
	for(int i=0; i<nLen; i++)
	{
		const QChar data = strText.at(i);
		if(m_nDataCount == 2)
		{
			strData += " ";
			m_nDataCount = 0;
		}
		if(data == QChar(' '))
		{
			continue;
		}
		else
		{
			strData += data;
			m_nDataCount++;
		}
	}
	if(m_nDataCount == 1)
	{
		strData += '0';
	}
	leSend->setText(strData);
	
	nLen = (strData.count()+1)/3;	
	quint8* buffer = new quint8[nLen];
	Wu_HexString2Bin(buffer, nLen, strData.toLocal8Bit().data(), strData.count(), " ");
	ui->teMain->append(tr("TX(%1): %2").arg(QTime::currentTime().toString("HH:mm:ss")).arg(strData));

	tcpSocket->write((const char*)buffer, nLen);
	delete[] buffer;

	m_nSendBytes += nLen;

	ShowHead();

	ui->leStatus->setText(tr("TX"));	
}

void TcpCliDialog::onClkBtnAutoSend()
{
	m_bAutoSend = !m_bAutoSend;
	ui->btnAutoSend->setText(m_bAutoSend?tr("Stop Send"):tr("Auto Send"));
		
	if(m_bAutoSend)
	{
		ui->gbSend->setEnabled(false);
	}
	else
	{
		ui->gbSend->setEnabled(true);		
	}
	m_nAutoSendCount = 0;
	m_nAutoSendPos = 0;
}

void TcpCliDialog::onClkBtnClear()
{
	ui->teMain->clear();
}
 

void TcpCliDialog::keyPressEvent(QKeyEvent *event)
{
   switch(event->key())
   {
     case Qt::Key_Escape:
       this->close();
       break;

    default:
       QDialog::keyPressEvent(event);
   }
}

void TcpCliDialog::closeEvent(QCloseEvent *event)
{
	if(QMessageBox::warning(NULL, tr("Quit"), tr("Sure to quit?"), QMessageBox::Ok|QMessageBox::Cancel) != QMessageBox::Ok)
	{
		event->ignore();
		return;
	}
	
	SaveSetting();
	event->accept();
}

void TcpCliDialog::onTimer()
{
	if (!m_bConnected)	return;
	
	if(m_bAutoSend)
	{
		AutoSend();
	}
}

void TcpCliDialog::AutoSend()
{
	if(m_nAutoSendCount%ui->spinInterval->value() == 0)
	{
		for(int i=0; i<MAX_SENDGROUP; i++)
		{
			int nPos = (i+m_nAutoSendPos)%MAX_SENDGROUP;
			if(m_sg[nPos].leSend->text().isEmpty())
			{
				continue;
			}
			SendData(m_sg[nPos].leSend);
			m_nAutoSendPos = nPos+1;
			break;
		}
	}
	m_nAutoSendCount++;
}

void TcpCliDialog::ShowHead()
{
	ui->lnSendBytes->display(m_nSendBytes);
	ui->lnRecvBytes->display(m_nRecvBytes);
}

void TcpCliDialog::ShowInfo()
{
	ui->teMain->append(tr("TX %1 bytes, RX %2 bytes")
		.arg(m_nSendBytes).arg(m_nRecvBytes));
}

void TcpCliDialog::onClkBtnAbout()
{
	QString strInfo = QString("Wt TCP Client %1").arg(1.1);
	strInfo += "\n";
	strInfo += tr("Homepage: %2").arg("WtPlus.cn");
	QMessageBox::about(NULL, tr("About"), strInfo);
}

void TcpCliDialog::onClkBtnHome()
{
	const QUrl url("http://www.wtplus.cn/wttcpcli");
    QDesktopServices::openUrl(url);
}

void TcpCliDialog::SaveSetting()
{
	QSettings setting;
	setting.setValue(QString("TcpCliTest/LatestIP"), m_latestIpList);
	setting.setValue(QString("TcpCliTest/LatestPort"), ui->spinPort->value());
	for(int i=0; i<MAX_SENDGROUP; i++)
	{
		setting.setValue(QString("TcpCliTest/SendData%1").arg(i), m_sg[i].leSend->text());
	}
}


void TcpCliDialog::onConnected()
{
	ui->teMain->append(tr("connected to %1:%2 succeed").arg(tcpSocket->peerAddress().toString()).arg(tcpSocket->peerPort()));
	
	m_bConnected = true;
	m_bAutoSend = false;
	ui->gbTcp->setEnabled(false);
	ui->gbSend->setEnabled(true);
	ui->btnAutoSend->setEnabled(true);
	ui->btnOpen->setText(tr("Close"));
	ui->btnAutoSend->setText(tr("Auto Send"));
	
	if (!m_timer.isActive())
	{
		m_timer.start(100);
	}
}

void TcpCliDialog::onDisConnected()
{
	m_bConnected = false;
	m_bAutoSend = false;
	ui->gbTcp->setEnabled(true);
	ui->gbSend->setEnabled(false);
	ui->btnAutoSend->setEnabled(false);
	ui->btnOpen->setText(tr("Open"));
	ui->btnAutoSend->setText(tr("Auto Send"));
	
	m_timer.stop();
}

void TcpCliDialog::onRead()
{
	if (tcpSocket->bytesAvailable() < 0)
		return;
	
	QByteArray  arData = tcpSocket->readAll();
	int nLen = arData.size();
	char* buffer = new char[nLen * 3 + 1];
	Wu_Bin2HexString(buffer, nLen * 3 + 1, (quint8*)arData.data(), nLen, ' ');
	ui->teMain->append(tr("RX(%1): %2").arg(QTime::currentTime().toString("HH:mm:ss")).arg(buffer));
	delete[] buffer;
	m_nRecvBytes += nLen;
	ui->leStatus->setText(tr("RX"));	
}

void TcpCliDialog::onError(QAbstractSocket::SocketError socketError)
{
	switch (socketError) {
	case QAbstractSocket::RemoteHostClosedError:
		break;
	case QAbstractSocket::HostNotFoundError:
		ui->teMain->append(tr("The host was not found. Please check the "
			"host name and port settings."));
		break;
	case QAbstractSocket::ConnectionRefusedError:
		ui->teMain->append(tr("The connection was refused by the peer. "
			"Check that the host name and port "
			"settings are correct."));
		break;
	default:
		ui->teMain->append(tr("The following error occurred: %1.")
			.arg(tcpSocket->errorString()));
	}
	onDisConnected();

	ui->leStatus->setText(tr("Error..."));	
}
