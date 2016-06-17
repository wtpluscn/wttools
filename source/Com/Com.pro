QT       += core gui widgets

TARGET = wtcom
TEMPLATE = app

include (../def.pri)

SOURCES += 	source/main.cpp \
			source/comdialog.cpp \
			source/qextserialport.cpp \
			source/widgetutil.cpp \
			source/usbcom.cpp 
			

HEADERS  += include/comdialog.h \
			include/qextserialport.h \
			include/qextserialport_p.h \
			include/qextserialport_global.h \
			include/widgetutil.h \
			include/usbcom.h
			

win32 { SOURCES += source/qextserialport_win.cpp }
unix { SOURCES += source/qextserialport_unix.cpp }

FORMS    += ui/comdialog.ui  

INCLUDEPATH	+= ./include 


win32:RC_FILE=rc/main.rc

RESOURCES += rc/main.qrc
	
TRANSLATIONS = lang/wtcom_zh.ts

