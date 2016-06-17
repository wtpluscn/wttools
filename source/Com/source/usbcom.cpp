#include "usbcom.h"
#include <QDir>
#include <QStringList>

//ttyUSB0 -> ../../devices/ff540000.usb/usb3/3-1/3-1.1/3-1.1:1.0/ttyUSB0/tty/ttyUSB0
//ttyUSB1 -> ../../devices/ff540000.usb/usb3/3-1/3-1.2/3-1.2:1.0/ttyUSB1/tty/ttyUSB1
QString UsbCom_Def2Real(QString strDef)
{
	QDir dir("/sys/class/tty");
	QStringList filterList;
	filterList << "ttyUsb*";
	QFileInfoList fileList = dir.entryInfoList(filterList);
	if(fileList.count() == 0)	return "";
	
	foreach(QFileInfo fileInfo, fileList)
	{
		QString strLink = fileInfo.symLinkTarget();
		if(strLink.isEmpty())	continue;
		QStringList strLinkPart = strLink.split("/");
		int nPart = strLinkPart.count();
		
		if(nPart < 6)	continue;
		QString strID = strLinkPart.at(nPart - 5);
		strID = strID.section(1, 1);
		if(strDef.compare(QString("%1%2").arg(USBCOM_PREFIX).arg(strID), Qt::CaseInsensitive) == 0)
		{
			return fileInfo.fileName();
		}
	}
	
	return "";
}

QStringList UsbComList()
{
	QStringList list;
	list << "UsbCom3" << "UsbCom4" << "UsbCom4" << "UsbCom5";
	return list;
}
