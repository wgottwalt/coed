#include <iostream>
#include <QApplication>
#include <QFontDatabase>

#include "MainWindow.hxx"

qint32 main(qint32 argc, char **argv)
{
    QApplication *app = nullptr;
    MainWindow *win = nullptr;
    qint32 result = 0;

    try
    {
        app = new QApplication{argc, argv};
        win = new MainWindow;

        QApplication::setApplicationName("CoEd");
        QApplication::setOrganizationName("LinuxAddicted");
        QApplication::setApplicationVersion(QString("0.1.0.0 (") + __DATE__ + " " + __TIME__ + ")");
        QFontDatabase::addApplicationFont(":/Font/SiraCodeRegular");
        QFontDatabase::addApplicationFont(":/Font/SiraMonoRegular");

        win->show();
        result = app->exec();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    delete win;
    delete app;

    return result;
}
