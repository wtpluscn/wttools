QT       += core gui widgets network

TARGET = wttcpgate
TEMPLATE = app

include (../def.pri)

SOURCES += 	source/main.cpp \
			source/tcpgatedialog.cpp \
			source/widgetutil.cpp 
			

HEADERS  += include/tcpgatedialog.h \
			include/widgetutil.h 
			

FORMS    += ui/tcpgatedialog.ui  

INCLUDEPATH	+= ./include 


win32:RC_FILE=rc/main.rc

RESOURCES += rc/main.qrc
	
TRANSLATIONS = lang/wttcpgate_zh.ts

