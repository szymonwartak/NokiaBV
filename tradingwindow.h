#ifndef TRADINGWINDOW_H
#define TRADINGWINDOW_H

#include <QtGui/QMainWindow>
#include <QLabel>
#include <QGridLayout>
#include <QNetworkAccessManager>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QAuthenticator>
#include <QHash>
#include <QStringListIterator>
#include <QTimer>
#include <QPushButton>
#include <QComboBox>
#include <QSslError>
#include <QSlider>
#include <QtXml>
#include <QMouseEvent>
#include <mainwindow.h>

class TradingWindow;
class Chart;
class Order;

class TradingWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit TradingWindow(QWidget *parent = 0, Chart *theChart = 0);
    virtual ~TradingWindow();
    void setOrientation(MainWindow::ScreenOrientation orientation);
    void showExpanded();
    void setMainWindow(MainWindow *theMainWindow);
    void setChart(Chart *theChart);

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    MainWindow *mainWindow;
    Chart *chart;
    QTimer *timer;
    QNetworkAccessManager* nam;
    QNetworkReply* reply;
    QUrl url;
    QString cookieString;
    QPoint oldPoint;
    QPushButton *refreshAllButton;
    QLabel *sellOrder3Label, *sellOrder2Label, *sellOrder1Label, *buyOrder1Label, *buyOrder2Label, *buyOrder3Label;
    QLabel *accountLabel, *tradeLabel;
    QLabel *accountAvailableLabel, *accountReservedLabel, *accountTotalLabel, *accountValuationLabel;
    QLabel *accountAvailableLabelTitle, *accountReservedLabelTitle, *accountTotalLabelTitle, *accountValuationLabelTitle;
    QLabel *confirmLabel;
    QPushButton *dollarButton, *poundButton, *euroButton;
    QComboBox *tradeCB, *openOrdersCB;
    QLabel *priceSliderLabel, *quantitySliderLabel;
    QSlider *priceSlider, *quantitySlider;
    QLabel *buySellQuantityLabel, *buySellPriceLabel;
    QPushButton *buySellButton, *cancelBuySellButton, *confirmButton;
    QWidget *bottomWidget, *confirmWidget, *chartWidget;
    QList<Order> *openOrders;
    QPushButton *killOrderButton;
    QString *currentCurrency, *currentVault, *currentMetal;
    QString silverAvailableStr, silverReservedStr, silverTotalStr, silverValuationStr;
    QString dollarAvailableStr, dollarReservedStr, dollarTotalStr, dollarValuationStr;
    QString poundAvailableStr, poundReservedStr, poundTotalStr, poundValuationStr;
    QString euroAvailableStr, euroReservedStr, euroTotalStr, euroValuationStr;
    int cookieCount;

    double silverAvailable, silverReserved, silverTotal, silverValuation;
    double dollarAvailable, dollarReserved, dollarTotal, dollarValuation;
    double poundAvailable, poundReserved, poundTotal, poundValuation;
    double euroAvailable, euroReserved, euroTotal, euroValuation;
    double totalValueAtPrice, finalBuySellQuantity;
    int buySellPrice;
    double buySellQuantity;

    bool loggedIn, confirmingOrder;

    void initTrading();
    void getOrders();
    void getOrdersXml();
    void getMarket();
    void getMarketXml();
    void setAccountData();
    void setAccountDataXml();
    void parseBalance(QString page);
    void parseBalanceXml(QString page);
    void parseOrders(QString page);
    void parseOrdersXml(QString page);
    void parseMarketXml(QString page);
    void addOrder(Order order);
    void removeOrder(int index);
    QString getConsiderationCurrency();

    void login();
    void initChartWidget();
    void initConfirmWidget();
    QString convertHtmlCurrencySymbols(QString string);

private slots:
    void httpReply(QNetworkReply* reply);
    void killOrder();
    void placeOrder();
    void showPlaceOrderDisplay();
    void hidePlaceOrderDisplay();
    void placeOrderXml();
    void getBalanceXml();
    void getBalance();
    void refreshAll();
    void updateBuySellPrice();
    void updateBuySellQuantity();
    void resetBuySellQuantity();
    void updateBuySellPriceLabel(int value);
    void updateBuySellQuantityLabel(int value);
    void poundPressed();
    void euroPressed();
    void dollarPressed();

};

#endif // TRADINGWINDOW_H


