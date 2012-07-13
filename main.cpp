#include "mainwindow.h"
#include "tradingwindow.h"

#include <QtGui/QApplication>
#include <QSslSocket>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    MainWindow mainWindow;
    mainWindow.setOrientation(MainWindow::ScreenOrientationAuto);
    mainWindow.showExpanded();

//    TradingWindow tradingWindow;
//    tradingWindow.setOrientation(TradingWindow::ScreenOrientationLockPortrait);
//    tradingWindow.showExpanded();

    return app.exec();
}
