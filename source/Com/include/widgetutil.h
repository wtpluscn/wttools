#ifndef _WIDGET_UTIL_H_
#define _WIDGET_UTIL_H_

#include <QCheckBox>
#include <QComboBox>

void WU_InitCbCom(QComboBox* cbCom);
void WU_InitCbBaudRate(QComboBox* cbBaudRate);
void WU_InitCbDataBits(QComboBox* cbDataBits);
void WU_InitCbCrcBits(QComboBox* cbCrcBits);
void WU_InitCbStopBits(QComboBox* cbStopBits);
void WU_InitCbFlowCtrl(QComboBox* cbFlowCtrl);
QVariant WU_CbCurrentData(QComboBox* cb);
qint32	Wu_Bin2HexString(char* o_pString, qint32 i_nMaxLen, const quint8* i_ptrBin, int i_nBinLen, quint8 nSparator);
qint32 Wu_HexString2Bin(quint8* o_ptrBin, qint32 i_nMaxLen, const char* i_pString, qint32 i_nStringLen, const char* pSparator);
#endif	//_WIDGET_UTIL_H_
