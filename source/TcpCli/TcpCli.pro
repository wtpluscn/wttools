QT       += core gui widgets network

TARGET = wttcpcli
TEMPLATE = app

include (../def.pri)

SOURCES += 	source/main.cpp \
			source/tcpclidialog.cpp \
			source/widgetutil.cpp 
			

HEADERS  += include/tcpclidialog.h \
			include/widgetutil.h 
			

FORMS    += ui/tcpclidialog.ui  

INCLUDEPATH	+= ./include 


win32:RC_FILE=rc/main.rc

RESOURCES += rc/main.qrc
	
TRANSLATIONS = lang/wttcpcli_zh.ts

