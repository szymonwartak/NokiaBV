#include "mainwindow.h"
#include "tradingwindow.h"
#include "chart.h"
#include <QtCore/QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    chart = new Chart();
    chart->setSmallTheme();
    chart->showSized(350,220);
    tradingWindow = new TradingWindow(this, chart);
    //tradingWindow->setChart(chart); // we want the trading window and the main window to share one chart

    lastUpdatedLabel = new QLabel("Last updated");

    buyGoldHead = new QLabel("Buy Gold London");
    buySilverHead = new QLabel("Buy Silver London");
    sellGoldHead = new QLabel("Sell Gold London");
    sellSilverHead = new QLabel("Sell Silver London");
    buyGoldBodyLeft = new QLabel();
    buyGoldBodyRight = new QLabel();
    buySilverBodyLeft = new QLabel();
    buySilverBodyRight = new QLabel();
    sellGoldBodyLeft = new QLabel();
    sellGoldBodyRight = new QLabel();
    sellSilverBodyLeft = new QLabel();
    sellSilverBodyRight = new QLabel();

    updateButton = new QPushButton("Update");
    updateButton->setFixedHeight(50);
    chartButton = new QPushButton("Gold chart");
    chartButton->setFixedHeight(50);
    //tradeButton = new QPushButton("Trade");

    QWidget *windowWidget = new QWidget(this);
    windowWidget->setMinimumSize(360,640);
    QGridLayout *windowLayout = new QGridLayout(windowWidget);

    QWidget *centralWidget = new QWidget(windowWidget);
    centralWidget->setFixedSize(360,420);
    QGridLayout *centralLayout = new QGridLayout(centralWidget);

    windowLayout->setContentsMargins(0,0,0,0);
    windowLayout->addWidget(centralWidget,2,1);
    windowLayout->addWidget(chart,1,1);

    centralLayout->setContentsMargins(2,2,2,150);
    centralLayout->addWidget(lastUpdatedLabel,1,1,1,4);
    centralLayout->addWidget(buyGoldHead,2,1,1,2);
    centralLayout->addWidget(sellGoldHead,2,3,1,2);
    centralLayout->addWidget(buyGoldBodyLeft,3,1);
    centralLayout->addWidget(buyGoldBodyRight,3,2);
    centralLayout->addWidget(sellGoldBodyLeft,3,3);
    centralLayout->addWidget(sellGoldBodyRight,3,4);
    centralLayout->addWidget(buySilverHead,4,1,1,2);
    centralLayout->addWidget(sellSilverHead,4,3,1,2);
    centralLayout->addWidget(buySilverBodyLeft,5,1);
    centralLayout->addWidget(buySilverBodyRight,5,2);
    centralLayout->addWidget(sellSilverBodyLeft,5,3);
    centralLayout->addWidget(sellSilverBodyRight,5,4);

    centralLayout->addWidget(updateButton,6,1,1,2);
    centralLayout->addWidget(chartButton,6,3,1,2);
    //centralLayout->addWidget(tradeButton,6,4,1,1);


//    connect(&timer, SIGNAL(timeout()), this, SLOT(updatePrices()));

    nam = new QNetworkAccessManager(this);
    QObject::connect(nam, SIGNAL(finished(QNetworkReply*)),
             this, SLOT(finishedSlot(QNetworkReply*)));

    connect(updateButton, SIGNAL(clicked()), this, SLOT(updateButtonClicked()));
    connect(chartButton, SIGNAL(clicked()), this, SLOT(chartButtonClicked()));
    //connect(tradeButton, SIGNAL(clicked()), this, SLOT(tradeButtonClicked()));

    url.setUrl("http://www.bullionvault.com/view_market_update.do?timeZone=&considerationCurrency=GBP&securityId=&quantity=0.001&marketWidth=3");
    updatePrices();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setOrientation(ScreenOrientation orientation)
{
#if defined(Q_OS_SYMBIAN)
    // If the version of Qt on the device is < 4.7.2, that attribute won't work
    if (orientation != ScreenOrientationAuto) {
        const QStringList v = QString::fromAscii(qVersion()).split(QLatin1Char('.'));
        if (v.count() == 3 && (v.at(0).toInt() << 16 | v.at(1).toInt() << 8 | v.at(2).toInt()) < 0x040702) {
            qWarning("Screen orientation locking only supported with Qt 4.7.2 and above");
            return;
        }
    }
#endif // Q_OS_SYMBIAN

    Qt::WidgetAttribute attribute;
    switch (orientation) {
#if QT_VERSION < 0x040702
    // Qt < 4.7.2 does not yet have the Qt::WA_*Orientation attributes
    case ScreenOrientationLockPortrait:
        attribute = static_cast<Qt::WidgetAttribute>(128);
        break;
    case ScreenOrientationLockLandscape:
        attribute = static_cast<Qt::WidgetAttribute>(129);
        break;
    default:
    case ScreenOrientationAuto:
        attribute = static_cast<Qt::WidgetAttribute>(130);
        break;
#else // QT_VERSION < 0x040702
    case ScreenOrientationLockPortrait:
        attribute = Qt::WA_LockPortraitOrientation;
        break;
    case ScreenOrientationLockLandscape:
        attribute = Qt::WA_LockLandscapeOrientation;
        break;
    default:
    case ScreenOrientationAuto:
        attribute = Qt::WA_AutoOrientation;
        break;
#endif // QT_VERSION < 0x040702
    };
    setAttribute(attribute, true);
}

void MainWindow::showExpanded() {
    setOrientation(MainWindow::ScreenOrientationLockPortrait);
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    showMaximized();
#else
    show();
#endif
    //chart->setLargeTheme();
}

void MainWindow::showChart() {
    chart->showSized(350,220);
}

void MainWindow::tradeButtonClicked() {
    hide();
    tradingWindow->setMainWindow(this);
    tradingWindow->showExpanded();
}

void MainWindow::updateButtonClicked() {
//    if(!updatingPrices) {
//        updatingPrices = true;
//        updateButton->setText("Stop price updates");
//        reply = nam->get(QNetworkRequest(url));
//        timer.start(3000);
//    } else {
//        updatingPrices = false;
//        timer.stop();
//        updateButton->setText("Start price updates");
//    }
    updatePrices();
}

void MainWindow::updatePrices() {
    chart->updatePrices();
    reply = nam->get(QNetworkRequest(url));
}

void MainWindow::chartButtonClicked() {
    setOrientation(MainWindow::ScreenOrientationLockLandscape);
    chart->setMainWindow(this);
    chart->showExpanded();
}

void MainWindow::finishedSlot(QNetworkReply* reply)
{
    // Reading attributes of the reply
    // e.g. the HTTP status code
    QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    // Or the target URL if it was a redirect:
    QVariant redirectionTargetUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    // see CS001432 on how to handle this

    // no error received?
    if (reply->error() == QNetworkReply::NoError)
    {
        QByteArray bytes = reply->readAll();  // bytes
        //QString string(bytes); // string
        //qDebug(bytes.data());
        parsePrices(bytes.data());
    }
    // Some http error received
    else
    {
        // handle errors here
    }

    // We receive ownership of the reply object
    // and therefore need to handle deletion.
    reply->deleteLater();
}

void MainWindow::parsePrices(QString page) {
    oldPrices = newPrices;
    newPrices = QHash<QString,QString>();
    page.replace("\\u00A3",QString(QChar(0xA3)));
    QStringListIterator listIterator(page.split("setOrderBox"));
    if(listIterator.hasNext())
        listIterator.next(); // skip the "0" index
    while(listIterator.hasNext()) {
        QString nextStr = listIterator.next();
        // types are "[B/S] [GOLD/SILVER] [London/New York/Zurick] [Price/Quantity]"
        QString type = nextStr.section("','",3,3)+" "+nextStr.section("','",4,4)+" "+nextStr.section("','",5,5);
        if(nextStr.section("','",3,3) == "B") {
            newPrices[type+" Quantity"] = newPrices[type+" Quantity"] + nextStr.section("','",7,7)+"\n";
            newPrices[type+" Price"] = newPrices[type+" Price"] + nextStr.section("','",6,6)+"\n";
        } else {
            newPrices[type+" Quantity"] = nextStr.section("','",7,7)+"\n"+newPrices[type+" Quantity"];
            newPrices[type+" Price"] = nextStr.section("','",6,6)+"\n"+newPrices[type+" Price"];
        }
    }
    lastUpdatedLabel->setText(QDateTime::currentDateTime().toString("hh:mm:ss ddd MMMM d yyyy"));

    buyGoldBodyLeft->setText(newPrices["B GOLD London Quantity"]);
    buyGoldBodyRight->setText(newPrices["B GOLD London Price"]);
    buySilverBodyLeft->setText(newPrices["B SILVER London Quantity"]);
    buySilverBodyRight->setText(newPrices["B SILVER London Price"]);
    sellGoldBodyLeft->setText(newPrices["S GOLD London Quantity"]);
    sellGoldBodyRight->setText(newPrices["S GOLD London Price"]);
    sellSilverBodyLeft->setText(newPrices["S SILVER London Quantity"]);
    sellSilverBodyRight->setText(newPrices["S SILVER London Price"]);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    oldPoint = event->pos();
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    QPoint movePoint = event->pos();
    if(oldPoint.x() > 320 && movePoint.x() < 40) {
        tradeButtonClicked();
    }
}
