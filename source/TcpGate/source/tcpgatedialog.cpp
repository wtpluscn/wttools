#include "tcpgatedialog.h"
#include "ui_tcpgatedialog.h"
#include <QMessageBox>
#include "widgetutil.h"
#include <QSettings>
#include <QRegExpValidator>
#include <QRegExp>
#include <QDateTime>
#include <QUrl>
#include <QDesktopServices>
#include <QHostAddress>


TcpGateDialog::TcpGateDialog(QDialog *parent) :
	QDialog(parent),
	ui(new Ui::TcpGateDialog)
{
    ui->setupUi(this);
	
	connect(ui->btnClear, SIGNAL(clicked()), this, SLOT(onClkBtnClear()));
	connect(ui->btnAbout, SIGNAL(clicked()), this, SLOT(onClkBtnAbout()));
	connect(ui->btnHome, SIGNAL(clicked()), this, SLOT(onClkBtnHome()));
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	InitListen();
	InitConnect();
}

TcpGateDialog::~TcpGateDialog()
{
    delete ui;
}

void TcpGateDialog::InitListen()
{
	m_ndListen.group = ui->gbListen;
	m_ndListen.cbIp = ui->cbLIP;
	m_ndListen.spinPort = ui->spinLPort;
	m_ndListen.lnSend = ui->lnLSendBytes;
	m_ndListen.lnRecv = ui->lnLRecvBytes;
	m_ndListen.leStatus = ui->leLStatus;
	m_ndListen.btnStart = ui->btnListen;	
	connect(m_ndListen.btnStart, SIGNAL(clicked()), this, SLOT(onClkBtnListen()));
		
	m_ndListen.bConnected = false;
	m_ndListen.bStarted = false;
	m_ndListen.nSendBytes = 0;
	m_ndListen.nRecvBytes = 0;	
	QSettings setting;
	m_ndListen.latestIpList = setting.value("TcpGateTest/Listen/LatestIP", "").toStringList();
	int nCount = m_ndListen.latestIpList.count();
	for (int i = nCount-1; i >= 0; i--)
	{
		if (!m_ndListen.latestIpList[i].isEmpty())
		{
			m_ndListen.cbIp->addItem(m_ndListen.latestIpList[i]);
		}
	}
	
	m_ndListen.tcpSocket = NULL;	
	m_ndListen.tcpServer = new QTcpServer(this);
	connect(m_ndListen.tcpServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));	
	m_ndListen.spinPort->setValue(setting.value("TcpGateTest/Listen/LatestPort", 5100).toInt());	
}

void TcpGateDialog::InitConnect()
{
	m_ndConnect.group = ui->gbConnect;
	m_ndConnect.cbIp = ui->cbCIP;
	m_ndConnect.spinPort = ui->spinCPort;
	m_ndConnect.lnSend = ui->lnCSendBytes;
	m_ndConnect.lnRecv = ui->lnCRecvBytes;
	m_ndConnect.leStatus = ui->leCStatus;
	m_ndConnect.btnStart = ui->btnConnect;	
	connect(m_ndConnect.btnStart, SIGNAL(clicked()), this, SLOT(onClkBtnConnect()));
		
	m_ndConnect.bConnected = false;
	m_ndConnect.bStarted = false;
	m_ndConnect.nSendBytes = 0;
	m_ndConnect.nRecvBytes = 0;	
	QSettings setting;
	m_ndConnect.latestIpList = setting.value("TcpGateTest/Connect/LatestIP", "").toStringList();
	int nCount = m_ndConnect.latestIpList.count();
	for (int i = nCount-1; i >= 0; i--)
	{
		if (!m_ndConnect.latestIpList[i].isEmpty())
		{
			m_ndConnect.cbIp->addItem(m_ndConnect.latestIpList[i]);
		}
	}
	
	m_ndConnect.tcpServer = NULL;	
	m_ndConnect.tcpSocket = new QTcpSocket(this);
	m_ndConnect.spinPort->setValue(setting.value("TcpGateTest/Connect/LatestPort", 5100).toInt());	
	connect(m_ndConnect.tcpSocket, SIGNAL(connected()), this, SLOT(onConnected()));
	connect(m_ndConnect.tcpSocket, SIGNAL(disconnected()), this, SLOT(onDisConnected()));
	connect(m_ndConnect.tcpSocket, SIGNAL(readyRead()), this, SLOT(onRead()));
	connect(m_ndConnect.tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
		this, SLOT(onNdcError(QAbstractSocket::SocketError)));
}

void TcpGateDialog::onClkBtnListen()
{
	if(m_ndListen.bStarted)
	{
		m_ndListen.btnStart->setText(tr("Listen"));
		if(m_ndListen.tcpSocket)
		{
			delete m_ndListen.tcpSocket;
			m_ndListen.tcpSocket = NULL;
		}
		m_ndListen.tcpServer->close();
		m_ndListen.bStarted = false;
		m_ndListen.bConnected = false;
		m_ndListen.spinPort->setEnabled(true);
		m_ndListen.cbIp->setEnabled(true);
		Log2Main(tr("stop listen"));
	}
	else
	{
		bool nRet;
		if(m_ndListen.cbIp->currentText().isEmpty())
		{
			nRet = m_ndListen.tcpServer->listen(QHostAddress::Any, m_ndListen.spinPort->value());
		}
		else
		{
			nRet = m_ndListen.tcpServer->listen(QHostAddress(m_ndListen.cbIp->currentText()), m_ndListen.spinPort->value());
		}
		
		if(nRet)
		{
			m_ndListen.btnStart->setText(tr("Stop Listen"));
			m_ndListen.bStarted = true;
			m_ndListen.bConnected = false;
			m_ndListen.spinPort->setEnabled(false);
			m_ndListen.cbIp->setEnabled(false);
			Log2Main(tr("listen at %1:%2 succeed").arg(m_ndListen.tcpServer->serverAddress().toString()).arg(m_ndListen.tcpServer->serverPort()));
		}
		else
		{
			Log2Main(tr("listen at %1:%2 failed").arg(m_ndListen.tcpServer->serverAddress().toString()).arg(m_ndListen.tcpServer->serverPort()));
		}		
				
		m_ndListen.latestIpList.removeOne(m_ndListen.cbIp->currentText());
		m_ndListen.latestIpList << m_ndListen.cbIp->currentText();
		if (m_ndListen.latestIpList.count() > 5)
		{
			m_ndListen.latestIpList.removeFirst();
		}
		
		m_ndListen.nSendBytes = 0;
		m_ndListen.nRecvBytes = 0;
	}
}

void TcpGateDialog::onClkBtnConnect()
{
	if(m_ndConnect.bConnected)
	{
		m_ndConnect.btnStart->setText(tr("Connect"));
		Log2Main(tr("break connect"));
		m_ndConnect.tcpSocket->disconnectFromHost();
		m_ndConnect.bStarted = false;
		m_ndConnect.bConnected = false;
		m_ndConnect.spinPort->setEnabled(true);
		m_ndConnect.cbIp->setEnabled(true);
	}
	else
	{
		if(m_ndConnect.cbIp->currentText().isEmpty()) return;
		
		m_ndConnect.tcpSocket->connectToHost(m_ndConnect.cbIp->currentText(), m_ndConnect.spinPort->value());
		m_ndConnect.latestIpList.removeOne(m_ndConnect.cbIp->currentText());
		m_ndConnect.latestIpList << m_ndConnect.cbIp->currentText();
		if (m_ndConnect.latestIpList.count() > 5)
		{
			m_ndConnect.latestIpList.removeFirst();
		}
	
		m_ndConnect.nSendBytes = 0;
		m_ndConnect.nRecvBytes = 0;
	}
}


void TcpGateDialog::Log2Main(QString strLog)
{
	ui->teMain->append(strLog);
}

void TcpGateDialog::onClkBtnClear()
{
	ui->teMain->clear();
}
 
void TcpGateDialog::keyPressEvent(QKeyEvent *event)
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

void TcpGateDialog::closeEvent(QCloseEvent *event)
{
	if(QMessageBox::warning(NULL, tr("Quit"), tr("Sure to quit?"), QMessageBox::Ok|QMessageBox::Cancel) != QMessageBox::Ok)
	{
		event->ignore();
		return;
	}
	
	SaveSetting();
	event->accept();
}

void TcpGateDialog::onTimer()
{
}

void TcpGateDialog::onClkBtnAbout()
{
	QString strInfo = QString("Wt TCP Gate %1").arg(1.1);
	strInfo += "\n";
	strInfo += tr("Homepage: %2").arg("WtPlus.cn");
	QMessageBox::about(NULL, tr("About"), strInfo);
}

void TcpGateDialog::onClkBtnHome()
{
	const QUrl url("http://www.wtplus.cn/wttcpgate");
    QDesktopServices::openUrl(url);
}

void TcpGateDialog::SaveSetting()
{
	QSettings setting;
	setting.setValue(QString("TcpGateTest/Listen/LatestIP"), 	m_ndListen.latestIpList);
	setting.setValue(QString("TcpGateTest/Listen/LatestPort"), 	m_ndListen.spinPort->value());
	setting.setValue(QString("TcpGateTest/Connect/LatestIP"), 	m_ndConnect.latestIpList);
	setting.setValue(QString("TcpGateTest/Connect/LatestPort"), m_ndConnect.spinPort->value());
}

NetData_t* TcpGateDialog::GetNetData(QTcpSocket* tcpSocket)
{
	if(tcpSocket == m_ndConnect.tcpSocket)
	{
		return &m_ndConnect;
	}
	else
	{
		return &m_ndListen;
	}
}

void TcpGateDialog::onNewConnection()
{
	if(m_ndListen.tcpSocket)
	{
		m_ndListen.tcpSocket->close();
		delete m_ndListen.tcpSocket;
	}
	
	m_ndListen.tcpSocket = m_ndListen.tcpServer->nextPendingConnection();
	connect(m_ndListen.tcpSocket, SIGNAL(disconnected()), this, SLOT(onDisConnected()));
	connect(m_ndListen.tcpSocket, SIGNAL(readyRead()), this, SLOT(onRead()));
	connect(m_ndListen.tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)),
		this, SLOT(onError(QAbstractSocket::SocketError)));
	
	Log2Main(tr("receive new connect%1:%2").arg(m_ndListen.tcpSocket->peerAddress().toString()).arg(m_ndListen.tcpSocket->peerPort()));
	
	m_ndListen.bConnected = true;
}


void TcpGateDialog::disconnected(QTcpSocket* tcpSocket)
{
	NetData_t* ndData = GetNetData(tcpSocket);
	ndData->bConnected = false;
	if(ndData == &m_ndConnect)
	{
		m_ndConnect.spinPort->setEnabled(true);
		m_ndConnect.cbIp->setEnabled(true);
		m_ndConnect.btnStart->setText(tr("Connect"));
	}
	else
	{
		m_ndListen.tcpSocket->deleteLater();
		m_ndListen.tcpSocket = NULL;
	}
}

void TcpGateDialog::onDisConnected()
{
	QTcpSocket* tcpSocket = (QTcpSocket*)sender();
	if (!tcpSocket)	return;
	disconnected(tcpSocket);
}

void TcpGateDialog::onRead()
{
	QTcpSocket* tcpSocket = (QTcpSocket*)sender();
	if (!tcpSocket || tcpSocket->bytesAvailable() < 0)
		return;
	
	NetData_t* ndRx;
	NetData_t* ndTx;
	QString strSrc;
	if(tcpSocket == m_ndConnect.tcpSocket)
	{
		ndRx = &m_ndConnect;
		ndTx = &m_ndListen;
		strSrc = tr("from connect:");
	}
	else
	{
		ndRx = &m_ndListen;
		ndTx = &m_ndConnect;
		strSrc = tr("from listen:");
	}
	
	QByteArray  arData = tcpSocket->readAll();
	int nLen = arData.size();
	char* buffer = new char[nLen * 3 + 1];
	Wu_Bin2HexString(buffer, nLen * 3 + 1, (quint8*)arData.data(), nLen, ' ');
	Log2Main(tr("%1(%2): %3").arg(strSrc).arg(QTime::currentTime().toString("HH:mm:ss")).arg(buffer));
	delete[] buffer;
	ndRx->nRecvBytes += nLen;	
	ndRx->lnRecv->display(ndTx->nRecvBytes);
	ndRx->leStatus->setText(tr("RX"));		
	
	if(ndTx->tcpSocket->isWritable())
	{
		ndTx->tcpSocket->write(arData);
		ndTx->nSendBytes += nLen;	
		ndTx->lnSend->display(ndTx->nSendBytes);
		ndTx->leStatus->setText(tr("TX"));	
	}
}

void TcpGateDialog::onError(QAbstractSocket::SocketError socketError)
{
	QTcpSocket* tcpSocket = (QTcpSocket*)sender();
	NetData_t* ndData = GetNetData(tcpSocket);
	
	switch (socketError) {
	case QAbstractSocket::RemoteHostClosedError:
		Log2Main(tr("remote host error occurred: %1.")
			.arg(tcpSocket->errorString()));
		break;
	case QAbstractSocket::HostNotFoundError:
		Log2Main(tr("The host was not found. Please check the "
			"host name and port settings."));
		break;
	case QAbstractSocket::ConnectionRefusedError:
		Log2Main(tr("The connection was refused by the peer. "
			"Check that the host name and port "
			"settings are correct."));
		break;
	default:
		Log2Main(tr("The following error occurred: %1.")
			.arg(tcpSocket->errorString()));
	}
	disconnected(tcpSocket);
	ndData->leStatus->setText(tr("Error..."));
}


void TcpGateDialog::onConnected()
{
	QTcpSocket* tcpSocket = (QTcpSocket*)sender();
	
	Log2Main(tr("connected to %1:%2 succeed").arg(tcpSocket->peerAddress().toString()).arg(tcpSocket->peerPort()));
	
	m_ndConnect.btnStart->setText(tr("Break"));
	m_ndConnect.bConnected = true;
	m_ndConnect.spinPort->setEnabled(false);
	m_ndConnect.cbIp->setEnabled(false);	
}