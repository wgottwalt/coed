#include <QApplication>
#include <QFontDatabase>
#include "MainWindow.hxx"

qint32 main(qint32 argc, char **argv) noexcept(false)
{
    QApplication app(argc, argv);
    MainWindow win;

    QApplication::setApplicationName("CoEd");
    QApplication::setOrganizationName("LinuxAddicted");
    QApplication::setApplicationVersion(QString("0.1.0.0 (") + __DATE__ + " " + __TIME__ + ")");
    QFontDatabase::addApplicationFont(":/Font/SiraCodeRegular");
    QFontDatabase::addApplicationFont(":/Font/SiraMonoRegular");

    win.show();

    return app.exec();
}
