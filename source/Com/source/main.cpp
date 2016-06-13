#include "comdialog.h"
#include <QApplication>	
#include <QTranslator>
#include <QMessageBox>

int main(int argc, char *argv[])
{
	QApplication::addLibraryPath("./plugins");
	if(argc == 2)
	{
		QApplication::addLibraryPath(argv[1]);
	}
	QApplication::setApplicationName("WtCom");
	QApplication::setOrganizationName("WtPlus");
	
	QApplication app(argc, argv);

	QTranslator* translator = new QTranslator(0);
	translator->load(qApp->applicationDirPath() + "/wtcom_zh.qm");
	qApp->installTranslator(translator);

    ComDialog w(0);
    w.show();
    return app.exec();
}
