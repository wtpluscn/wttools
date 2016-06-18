#WtCom
WtCom是一个跨平台的串口调试工具。
在Qt4.8，Qt5.4下编译通过。windows和raspbian调试通过。

针对raspbian进行特殊处理（在树莓派2+测试通过）：
linux下，采用usb转串口时，第一个插入的端口是ttyUSB0，第二个是ttyUSB1，依次类推。端口的设备名和插入顺序有关，和具体端口无关。为调试方便，增加对端口的绑定，即设备usbcom2~usbcom5分别对应树莓派（2+）的右下角、右上角、左下角、左上角的USB端口。
对应方法是依据/sys/class/tty/中链接文件的名称。

