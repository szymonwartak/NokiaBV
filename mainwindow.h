#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui/QMainWindow>
#include <QLabel>
#include <QGridLayout>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHash>
#include <QStringListIterator>
#include <QTimer>
#include <QPushButton>

class MainWindow;
class TradingWindow;
class Chart;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    enum ScreenOrientation {
        ScreenOrientationLockPortrait,
        ScreenOrientationLockLandscape,
        ScreenOrientationAuto
    };
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();
    void setOrientation(ScreenOrientation orientation);
    void showExpanded();
    void showChart();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

private:
    QLabel *buyGoldHead, *buySilverHead, *sellGoldHead, *sellSilverHead;
    QLabel *buyGoldBodyLeft, *buyGoldBodyRight, *buySilverBodyLeft, *buySilverBodyRight, *sellGoldBodyLeft, *sellGoldBodyRight, *sellSilverBodyLeft, *sellSilverBodyRight;
    QLabel *lastUpdatedLabel;
    Chart *chart;
    TradingWindow *tradingWindow;
    QPushButton *chartButton, *updateButton, *tradeButton;
    bool updatingPrices;
    QPoint oldPoint, newPoint;
    QTimer timer;
    QNetworkAccessManager* nam;
    QNetworkReply* reply;
    QUrl url;
    QHash<QString,QString> oldPrices, newPrices;

    void parsePrices(QString page);
    void stopUpdates();
private slots:
    void finishedSlot(QNetworkReply* reply);
    void updateButtonClicked();
    void chartButtonClicked();
    void tradeButtonClicked();
    void updatePrices();

};

#endif // MAINWINDOW_H
