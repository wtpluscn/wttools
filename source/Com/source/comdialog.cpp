#include "ComDialog.h"
#include "ui_ComDialog.h"
#include "QMessageBox.h"
#include "WidgetUtil.h"
#include <QSettings>
#include <QRegExpValidator>
#include <QRegExp>
#include <QDateTime>
#include <QUrl>
#include <QDesktopServices>

ComDialog::ComDialog(QDialog *parent) :
	QDialog(parent),
	ui(new Ui::ComDialog)
{
    ui->setupUi(this);
	
	connect(ui->btnOpen, SIGNAL(clicked()), this, SLOT(onClkBtnOpen()));
	connect(ui->btnClear, SIGNAL(clicked()), this, SLOT(onClkBtnClear()));
	connect(ui->btnAutoSend, SIGNAL(clicked()), this, SLOT(onClkBtnAutoSend()));
	connect(ui->btnAbout, SIGNAL(clicked()), this, SLOT(onClkBtnAbout()));
	connect(ui->btnHome, SIGNAL(clicked()), this, SLOT(onClkBtnHome()));
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(onTimer()));
	Init();
	onClose();
}

ComDialog::~ComDialog()
{
    delete ui;
}

void ComDialog::Init()
{
	m_bAutoSend = false;
	
	m_sg[0].leSend = ui->leSend0;
	m_sg[0].btnSend = ui->btnSend0;
	
	m_sg[1].leSend = ui->leSend1;
	m_sg[1].btnSend = ui->btnSend1;
	
	m_sg[2].leSend = ui->leSend2;
	m_sg[2].btnSend = ui->btnSend2;
	
	WU_InitCbCom(ui->cbCom);
	WU_InitCbBaudRate(ui->cbBaudRate);
	WU_InitCbDataBits(ui->cbDataBits);
	WU_InitCbCrcBits(ui->cbCrcBits);
	WU_InitCbStopBits(ui->cbStopBits);
	WU_InitCbFlowCtrl(ui->cbFlowCtrl);
	serialPort = new QextSerialPort();

	QSettings setting;
	QString strCom = setting.value("COMTest/LatestCOM", "").toString();
	int nPos = ui->cbCom->findText(strCom);
	if (nPos >= 0)
	{
		ui->cbCom->setCurrentIndex(nPos);
	}

	int nBaudRate = setting.value("COMTest/LatestBaudRate", "").toInt();
	nPos = ui->cbBaudRate->findData(nBaudRate);
	if (nPos >= 0)
	{
		ui->cbBaudRate->setCurrentIndex(nPos);
	}

	int nDataBits = setting.value("COMTest/LatestDataBits", "").toInt();
	nPos = ui->cbDataBits->findData(nDataBits);
	if (nPos >= 0)
	{
		ui->cbDataBits->setCurrentIndex(nPos);
	}

	int nStopBits = setting.value("COMTest/LatestStopBits", "").toInt();
	nPos = ui->cbStopBits->findData(nStopBits);
	if (nPos >= 0)
	{
		ui->cbStopBits->setCurrentIndex(nPos);
	}

	int nCrcBits = setting.value("COMTest/LatestCrcBits", "").toInt();
	nPos = ui->cbCrcBits->findData(nCrcBits);
	if (nPos >= 0)
	{
		ui->cbCrcBits->setCurrentIndex(nPos);
	}

	int nFlowCtrl = setting.value("COMTest/LatestFlowCtrl", "").toInt();
	nPos = ui->cbFlowCtrl->findData(nFlowCtrl);
	if (nPos >= 0)
	{
		ui->cbFlowCtrl->setCurrentIndex(nPos);
	}	
	
    QRegExp regExp("[A-Fa-f0-9 ]{1,}");
	QValidator *validator = new QRegExpValidator(regExp, this);
	for(int i=0; i<MAX_SENDGROUP; i++)
	{
		connect(m_sg[i].btnSend, SIGNAL(clicked()), this, SLOT(onClkBtnSend()));
		m_sg[i].leSend->setValidator(validator);
		m_sg[i].leSend->setText(setting.value(QString("COMTest/SendData%1").arg(i), "").toString());
	}
}

void ComDialog::onClkBtnOpen()
{
	if(serialPort->isOpen())
	{
		onClose();
	}
	else
	{
		onOpen();
	}
	bool bOpen = serialPort->isOpen();
	ui->gbCom->setEnabled(!bOpen);
	ui->gbSend->setEnabled(bOpen);
	ui->btnAutoSend->setEnabled(bOpen);
	ui->btnOpen->setText(bOpen?tr("Close"):tr("Open"));
	m_bAutoSend = false;
	ui->btnAutoSend->setText(tr("Auto Send"));
}

void ComDialog::onOpen()
{
	QString strCom = ui->cbCom->currentText();
	if (strCom.isEmpty())
	{
		ui->teMain->append(tr("COM not exist"));
		return;
	}
    serialPort->flush();
	serialPort->setPortName(strCom);
	if (!serialPort->open(QIODevice::ReadWrite))
	{
		ui->teMain->append(tr("Open %1 failed").arg(strCom));
		return;
	}
	serialPort->setBaudRate((BaudRateType)WU_CbCurrentData(ui->cbBaudRate).toInt());
	DataBitsType dataBits = DATA_8;
	if (WU_CbCurrentData(ui->cbDataBits) == 7)
	{
		dataBits = DATA_7;
	}
	else if (WU_CbCurrentData(ui->cbDataBits) == 6)
	{
		dataBits = DATA_6;
	}
	serialPort->setDataBits(dataBits);

	StopBitsType stopBits = STOP_1;
	if (WU_CbCurrentData(ui->cbStopBits) == 2)
	{
		stopBits = STOP_2;
	}
	else if(WU_CbCurrentData(ui->cbStopBits) == 3)
	{
		stopBits = STOP_1_5;
	}
	serialPort->setStopBits(stopBits);

	ParityType parity = PAR_NONE;
	if (WU_CbCurrentData(ui->cbCrcBits) == 1)
	{
		parity = PAR_ODD;
	}
	else if (WU_CbCurrentData(ui->cbCrcBits) == 2)
	{
		parity = PAR_EVEN;
	}
	serialPort->setParity(parity);

	FlowType flow = FLOW_OFF;
	if (WU_CbCurrentData(ui->cbFlowCtrl) == 1)
	{
		flow = FLOW_HARDWARE;
	}
	serialPort->setFlowControl(flow);
	ui->leStatus->setText(tr("Com opened"));
    serialPort->setTimeout(10);
	
	m_nSendBytes = 0;
	m_nRecvBytes = 0;

	if (!m_timer.isActive())
	{
		m_timer.start(100);
	}
}

void ComDialog::onClose()
{
	m_timer.stop();
	if(serialPort->isOpen())
	{
		serialPort->close();
	}
}

void ComDialog::onClkBtnSend()
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

void ComDialog::SendData(QLineEdit* leSend)
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
	delete[] buffer;

	serialPort->write((const char*)buffer, nLen);

	m_nSendBytes += nLen;

	ShowHead();

	ui->leStatus->setText(tr("TX"));	
}

void ComDialog::onClkBtnAutoSend()
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

void ComDialog::onClkBtnClear()
{
	ui->teMain->clear();
}
 

void ComDialog::keyPressEvent(QKeyEvent *event)
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

void ComDialog::closeEvent(QCloseEvent *event)
{
	if(QMessageBox::warning(NULL, tr("Quit"), tr("Sure to quit?"), QMessageBox::Ok|QMessageBox::Cancel) != QMessageBox::Ok)
	{
		event->ignore();
		return;
	}
	
	SaveSetting();
	event->accept();
}

void ComDialog::onTimer()
{
	if (!serialPort->isOpen())	return;
	
	if(m_bAutoSend)
	{
		AutoSend();
	}
	
	if (serialPort->bytesAvailable()<=0)	return;

	QByteArray  arData = serialPort->readAll();
	int nLen = arData.size();
	char* buffer = new char[nLen * 3 + 1];
	Wu_Bin2HexString(buffer, nLen * 3 + 1, (quint8*)arData.data(), nLen, ' ');
	ui->teMain->append(tr("RX(%1): %2").arg(QTime::currentTime().toString("HH:mm:ss")).arg(buffer));
	delete[] buffer;
	m_nRecvBytes += nLen;

	ShowHead();
	ui->leStatus->setText(tr("RX"));
}

void ComDialog::AutoSend()
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

void ComDialog::ShowHead()
{
	ui->lnSendBytes->display(m_nSendBytes);
	ui->lnRecvBytes->display(m_nRecvBytes);
}

void ComDialog::ShowInfo()
{
	ui->teMain->append(tr("TX %1 bytes, RX %2 bytes")
		.arg(m_nSendBytes).arg(m_nRecvBytes));
}

void ComDialog::onClkBtnAbout()
{
	QString strInfo = QString("WtCom %1").arg(1.1);
	strInfo += "\n";
	strInfo += tr("Homepage: %2").arg("WtPlus.cn");
	QMessageBox::about(NULL, tr("About"), strInfo);
}

void ComDialog::onClkBtnHome()
{
	const QUrl url("http://www.wtplus.cn/wtcom");
    QDesktopServices::openUrl(url);
}

void ComDialog::SaveSetting()
{
	QSettings setting;
	setting.setValue(QString("COMTest/LatestCOM"), ui->cbCom->currentText());
	setting.setValue(QString("COMTest/LatestBaudRate"), WU_CbCurrentData(ui->cbBaudRate));
	setting.setValue(QString("COMTest/LatestDataBits"), WU_CbCurrentData(ui->cbDataBits));
	setting.setValue(QString("COMTest/LatestStopBits"), WU_CbCurrentData(ui->cbStopBits));
	setting.setValue(QString("COMTest/LatestCrcBits"), WU_CbCurrentData(ui->cbCrcBits));
	setting.setValue(QString("COMTest/LatestFlowCtrl"), WU_CbCurrentData(ui->cbFlowCtrl));
	for(int i=0; i<MAX_SENDGROUP; i++)
	{
		setting.setValue(QString("COMTest/SendData%1").arg(i), m_sg[i].leSend->text());
	}
}
