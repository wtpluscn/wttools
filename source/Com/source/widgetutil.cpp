#include "widgetutil.h"
#include "qextserialport.h"

void WU_InitCbCom(QComboBox* cbCom)
{
	cbCom->clear();
	QStringList list;
#ifdef Q_OS_WIN
	list << "Com1" << "Com2" << "Com3" << "Com4" << "Com5" << "Com6" << "Com7" << "Com8";
#else
    list<<"ttyUSB0"<<"ttyUSB1"<<"ttyUSB2"<<"ttyUSB3"<<"ttyUSB4"<<"ttyUSB5"
          <<"ttyS0"<<"ttyS1"<<"ttyS2"<<"ttyS3"<<"ttyS4"<<"ttyS5"<<"ttyS6"
         <<"ttyS7"<<"ttyS8"<<"ttyS9";
#endif
	cbCom->addItems(list);
}

void WU_InitCbBaudRate(QComboBox* cbBaudRate)
{
	cbBaudRate->clear();
	cbBaudRate->addItem("300", BAUD300);
	cbBaudRate->addItem("600", BAUD600);
	cbBaudRate->addItem("1200", BAUD1200);
	cbBaudRate->addItem("2400", BAUD2400);
	cbBaudRate->addItem("4800", BAUD4800);
	cbBaudRate->addItem("9600", BAUD9600);
	cbBaudRate->addItem("19200", BAUD19200);
	cbBaudRate->addItem("38400", BAUD38400);
	cbBaudRate->addItem("57600", BAUD57600);
	cbBaudRate->addItem("115200", BAUD115200);
}

void WU_InitCbDataBits(QComboBox* cbDataBits)
{
	cbDataBits->clear();
	cbDataBits->addItem("8", 8);
	cbDataBits->addItem("7", 7);
	cbDataBits->addItem("6", 6);
}

void WU_InitCbCrcBits(QComboBox* cbCrcBits)
{
	cbCrcBits->clear();
	cbCrcBits->addItem(QObject::tr("None"), 0);
	cbCrcBits->addItem(QObject::tr("Odd"), 1);
	cbCrcBits->addItem(QObject::tr("Even"), 2);
}

void WU_InitCbStopBits(QComboBox* cbStopBits)
{
	cbStopBits->clear();
	cbStopBits->addItem("1", 1);
	cbStopBits->addItem("2", 2);
#ifdef Q_OS_WIN
	cbStopBits->addItem("1.5", 3);
#endif
}

void WU_InitCbFlowCtrl(QComboBox* cbFlowCtrl)
{
	cbFlowCtrl->clear();
	cbFlowCtrl->addItem(QObject::tr("None"), 0);
	cbFlowCtrl->addItem(QObject::tr("Hardware"), 1);
}

QVariant WU_CbCurrentData(QComboBox* cb)
{
	return cb->itemData(cb->currentIndex());
}

#define HARFSTR2HEX(x)	(x < 10 ? (x+'0') : (x-0x0A+'A'))
qint32	Wu_Bin2HexString(char* o_pString, qint32 i_nMaxLen, const quint8* i_ptrBin, int i_nBinLen, quint8 nSparator)
{
	if (nSparator == 0)
	{
		if (i_nMaxLen < i_nBinLen * 2 + 1)	return -1;
	}
	else
	{
		if (i_nMaxLen < i_nBinLen * 3 + 1)	return -1;
	}
	int nCount = 0;
	quint8 nData;
	for (int i = 0; i < i_nBinLen; i++)
	{
		nData = (i_ptrBin[i] >> 4);
		o_pString[nCount++] = HARFSTR2HEX(nData);
		nData = (i_ptrBin[i] & 0x0f);
		o_pString[nCount++] = HARFSTR2HEX(nData);
		if (nSparator != 0 && i != i_nBinLen-1)
		{
			o_pString[nCount++] = nSparator;
		}
	}
	o_pString[nCount] = '\0';
	return nCount;
}

qint32 Wu_HexString2Bin(quint8* o_ptrBin, qint32 i_nMaxLen, const char* i_pString, qint32 i_nStringLen, const char* pSparator)
{
	qint32        bin_pos = 0;
	qint32        hex_pos = 0;
	unsigned char c;
	unsigned char c_acc = 0U;
	unsigned char c_num;
	unsigned char c_val;
	unsigned char state = 0U;

	while (hex_pos < i_nStringLen) 
	{
		c = (unsigned char)i_pString[hex_pos];
		if ((c_num = c ^ 48U) < 10U) 
		{
			c_val = c_num;
		}
		else if ((c_num = (c & ~32U)) > 64 && c_num < 71U) {
			c_val = c_num - 55U;
		}
		else if (pSparator != NULL && strchr(pSparator, c) != NULL && state == 0U) 
		{
			hex_pos++;
			continue;
		}
		else {
			break;
		}
		if (bin_pos >= i_nMaxLen) {
			return -1;
		}
		if (state == 0U) {
			c_acc = c_val * 16U;
		}
		else {
			o_ptrBin[bin_pos++] = c_acc | c_val;
		}
		state = ~state;
		hex_pos++;
	}
	return bin_pos;
}
