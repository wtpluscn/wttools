#ifndef _USB_COM_H_
#define _USB_COM_H_

#include <QString>
#ifdef __arm__
	#define USBCOM_ENABLE
#endif

//PI 四个USB分别对应 UsbCom3 UsbCom4 UsbCom5  UsbCom6

#define USBCOM_PREFIX	"UsbCom"

QString UsbCom_Def2Real(QString strDef);
QStringList UsbComList();


#endif	//_USB_COM_H_
