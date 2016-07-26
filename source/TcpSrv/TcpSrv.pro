QT       += core gui widgets network

TARGET = wttcpsrv
TEMPLATE = app

include (../def.pri)

SOURCES += 	source/main.cpp \
			source/tcpsrvdialog.cpp \
			source/widgetutil.cpp 
			

HEADERS  += include/tcpsrvdialog.h \
			include/widgetutil.h 
			

FORMS    += ui/tcpsrvdialog.ui  

INCLUDEPATH	+= ./include 


win32:RC_FILE=rc/main.rc

RESOURCES += rc/main.qrc
	
TRANSLATIONS = lang/wttcpsrv_zh.ts

