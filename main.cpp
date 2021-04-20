// Qt includes
#include <QApplication>
#include <QDebug>

// local includes
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app{argc, argv};

    qSetMessagePattern(QStringLiteral("%{time dd.MM.yyyy HH:mm:ss.zzz} "
                                      "["
                                      "%{if-debug}D%{endif}"
                                      "%{if-info}I%{endif}"
                                      "%{if-warning}W%{endif}"
                                      "%{if-critical}C%{endif}"
                                      "%{if-fatal}F%{endif}"
                                      "] "
                                      "%{function}(): "
                                      "%{message}"));

    QCoreApplication::setOrganizationDomain(QStringLiteral("brunner.ninja"));
    QCoreApplication::setOrganizationName(QStringLiteral("feedc0de enterprises"));
    QCoreApplication::setApplicationName(QStringLiteral("qmodbustester"));
    QCoreApplication::setApplicationVersion(QStringLiteral("1.0"));

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
