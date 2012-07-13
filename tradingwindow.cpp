#include "tradingwindow.h"
#include "chart.h"
#include "order.h"
#include <QtCore/QCoreApplication>
#include <QNetworkCookieJar>

TradingWindow::TradingWindow(QWidget *parent, Chart *theChart)
    : QMainWindow(parent)
{
    nam = new QNetworkAccessManager(this);
    cookieString = "";
    loggedIn = false;
    confirmingOrder = false;
    buySellPrice = 700;
    buySellQuantity = 0.0;
    openOrders = new QList<Order>();
    currentVault = new QString("London");
    currentCurrency = new QString("£");
    currentMetal = new QString("Silver");

    silverAvailable = 0, silverReserved = 0, silverTotal = 0, silverValuation = 0;
    dollarAvailable = 0, dollarReserved = 0, dollarTotal = 0, dollarValuation = 0;
    poundAvailable = 0, poundReserved = 0, poundTotal = 0, poundValuation = 0;
    euroAvailable = 0, euroReserved = 0, euroTotal = 0, euroValuation = 0;

    QFont accountFont("Arial", 6);
    accountLabel = new QLabel("Account");
//    confirmButton = new QPushButton("Confirm");
//    confirmButton->setVisible(false);
//    connect(confirmButton, SIGNAL(clicked()), this, SLOT(placeOrderXml()));
    buySellButton = new QPushButton("BUY/SELL");
    connect(buySellButton, SIGNAL(clicked()), this, SLOT(placeOrder()));
    cancelBuySellButton = new QPushButton("Cancel");
    connect(cancelBuySellButton, SIGNAL(clicked()), this, SLOT(hidePlaceOrderDisplay()));
    refreshAllButton = new QPushButton("Refresh");
    connect(refreshAllButton, SIGNAL(clicked()), this, SLOT(refreshAll()));

    accountAvailableLabelTitle = new QLabel("Available"); accountAvailableLabelTitle->setFont(accountFont);
    accountReservedLabelTitle = new QLabel("Reserved"); accountReservedLabelTitle->setFont(accountFont);
    accountTotalLabelTitle = new QLabel("Total"); accountTotalLabelTitle->setFont(accountFont);
    accountValuationLabelTitle = new QLabel("Valuation"); accountValuationLabelTitle->setFont(accountFont);
    accountAvailableLabel = new QLabel(); accountAvailableLabel->setFont(accountFont);
    accountReservedLabel= new QLabel(); accountReservedLabel->setFont(accountFont);
    accountTotalLabel = new QLabel(); accountTotalLabel->setFont(accountFont);
    accountValuationLabel = new QLabel(); accountValuationLabel->setFont(accountFont);

    tradeLabel = new QLabel("Trade");
    tradeCB = new QComboBox();
    tradeCB->addItem("Silver:London",Qt::DisplayRole);
    tradeCB->addItem("Gold:London",Qt::DisplayRole);
    tradeCB->addItem("Gold:Zurich",Qt::DisplayRole);
    tradeCB->addItem("Gold:New York",Qt::DisplayRole);
    dollarButton = new QPushButton("$");
    poundButton = new QPushButton("£");
    euroButton = new QPushButton(QString(QChar(8364)));

    buySellPriceLabel = new QLabel();
    buySellQuantityLabel = new QLabel();
    priceSliderLabel = new QLabel("Price");
    priceSlider = new QSlider(Qt::Horizontal);
    priceSlider->setMinimum(-5); priceSlider->setMaximum(5);
    connect(priceSlider, SIGNAL(sliderMoved(int)), this, SLOT(updateBuySellPriceLabel(int)));
    connect(priceSlider, SIGNAL(sliderReleased()), this, SLOT(updateBuySellPrice()));
    quantitySliderLabel = new QLabel("Position");
    quantitySlider = new QSlider(Qt::Horizontal);
    quantitySlider->setMinimum(0); quantitySlider->setMaximum(100);
    connect(quantitySlider, SIGNAL(sliderMoved(int)), this, SLOT(updateBuySellQuantityLabel(int)));
    connect(quantitySlider, SIGNAL(sliderReleased()), this, SLOT(updateBuySellQuantity()));

    buyOrder3Label = new QLabel(); buyOrder3Label->setFont(accountFont);
    buyOrder2Label = new QLabel(); buyOrder2Label->setFont(accountFont);
    buyOrder1Label = new QLabel(); buyOrder1Label->setFont(accountFont);
    sellOrder1Label = new QLabel(); sellOrder1Label->setFont(accountFont);
    sellOrder2Label = new QLabel(); sellOrder2Label->setFont(accountFont);
    sellOrder3Label = new QLabel(); sellOrder3Label->setFont(accountFont);

    openOrdersCB = new QComboBox();
    killOrderButton = new QPushButton("Kill Order");
    connect(killOrderButton, SIGNAL(clicked()), this, SLOT(killOrder()));

    QWidget *centralWidget = new QWidget(this);
    centralWidget->setMinimumSize(360,640);
    QGridLayout *outerLayout = new QGridLayout(centralWidget);

    QWidget *widget1 = new QWidget(centralWidget);
    QGridLayout *layout1 = new QGridLayout(widget1);
    bottomWidget = new QWidget(centralWidget);
    QGridLayout *layout6 = new QGridLayout(bottomWidget);

    confirmWidget = new QWidget(bottomWidget);
    confirmWidget->setVisible(false);
    initConfirmWidget();
    chartWidget = new QWidget(bottomWidget);
    chart = theChart;
    initChartWidget();

    outerLayout->setContentsMargins(0,0,0,0);
    outerLayout->addWidget(widget1,1,1);
    outerLayout->addWidget(bottomWidget,2,1);

    layout1->addWidget(accountLabel,1,1,1,3);
    layout1->addWidget(cancelBuySellButton,1,7,1,3);
    layout1->addWidget(buySellButton,1,7,1,3);
    layout1->addWidget(refreshAllButton,1,10,1,3);

    layout1->addWidget(accountAvailableLabelTitle,2,1,1,3);
    layout1->addWidget(accountReservedLabelTitle,2,4,1,3);
    layout1->addWidget(accountTotalLabelTitle,2,7,1,3);
    layout1->addWidget(accountValuationLabelTitle,2,10,1,3);

    layout1->addWidget(accountAvailableLabel,3,1,1,3);
    layout1->addWidget(accountReservedLabel,3,4,1,3);
    layout1->addWidget(accountTotalLabel,3,7,1,3);
    layout1->addWidget(accountValuationLabel,3,10,1,3);

    layout1->addWidget(tradeLabel,4,1,1,12);
    layout1->addWidget(tradeCB,5,1,1,6);
    layout1->addWidget(dollarButton,5,7,1,2);
    layout1->addWidget(poundButton,5,9,1,2);
    layout1->addWidget(euroButton,5,11,1,2);

    layout1->addWidget(buySellPriceLabel,6,1,1,3);
    layout1->addWidget(priceSlider,6,4,1,9);
    layout1->addWidget(buySellQuantityLabel,7,1,1,3);
    layout1->addWidget(quantitySlider,7,4,1,9);

    layout1->addWidget(openOrdersCB,8,1,1,8);
    layout1->addWidget(killOrderButton,8,9,1,4);

    layout1->addWidget(buyOrder3Label,9,1,1,2);
    layout1->addWidget(buyOrder2Label,9,3,1,2);
    layout1->addWidget(buyOrder1Label,9,5,1,2);
    layout1->addWidget(sellOrder1Label,9,7,1,2);
    layout1->addWidget(sellOrder2Label,9,9,1,2);
    layout1->addWidget(sellOrder3Label,9,11,1,2);

    layout6->addWidget(chartWidget,1,1);
    layout6->addWidget(confirmWidget,1,1);

    connect(nam, SIGNAL(finished(QNetworkReply*)), this, SLOT(httpReply(QNetworkReply*)));
    connect(dollarButton, SIGNAL(clicked()), this, SLOT(dollarPressed()));
    connect(poundButton, SIGNAL(clicked()), this, SLOT(poundPressed()));
    connect(euroButton, SIGNAL(clicked()), this, SLOT(euroPressed()));
}

TradingWindow::~TradingWindow()
{
}

void TradingWindow::initConfirmWidget() {
    QGridLayout *confirmLayout = new QGridLayout(confirmWidget);
    confirmLayout->setContentsMargins(0,0,0,0);
    confirmLabel = new QLabel("");
    confirmLayout->addWidget(confirmLabel,1,1);
    confirmWidget->setFixedSize(350,220);
}

void TradingWindow::initChartWidget() {
    QGridLayout *chartLayout = new QGridLayout(chartWidget);
    chartLayout->setContentsMargins(0,0,0,0);
    //chart = new Chart();
    chart->setSmallTheme();
    chart->showSized(350,220);
    chartWidget->setFixedSize(350,220);
    chartLayout->addWidget(chart);
}

void TradingWindow::login() {
    QNetworkRequest request(QUrl("https://live.bullionvault.com/secure/j_security_check?j_username=xxxx&j_password=xxxx"));
    nam->get(request);
}

void TradingWindow::httpReply(QNetworkReply* reply)
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
        //qDebug() << "reply: " << bytes.data();
        //parsePrices(bytes.data());
        if(!loggedIn) {
            loggedIn = true;
            getBalanceXml();
            getOrdersXml();
            getMarketXml();
        } else {
            if(reply->rawHeader("Set-Cookie").size()>0 && reply->rawHeader("Set-Cookie").indexOf("viewOrders")>=0) {
                parseOrders(bytes.data());
            } else if(QString(bytes.data()).indexOf("Client account balance") >= 0){
                parseBalance(bytes.data());
            } else if(QString(bytes.data()).indexOf("ORDERS_A") >= 0){ // return from view_orders_xml
                parseOrdersXml(bytes.data());
            } else if(QString(bytes.data()).indexOf("CLIENT_BALANCE_A") >= 0){
                parseBalanceXml(bytes.data());
            } else if(QString(bytes.data()).indexOf("MARKET_DEPTH_A") >= 0){
                parseMarketXml(bytes.data());
            } else if(QString(bytes.data()).indexOf("PLACE_ORDER_A") >= 0){
                getBalanceXml();
                getOrdersXml();
            }

        }
//        foreach( QByteArray b, reply->rawHeaderList() )
//            qDebug() << b << reply->rawHeader(b);

    }
    // Some http error received
    else
    {
        // handle errors here
        qDebug() << "errors in HTTP request";
    }

    // We receive ownership of the reply object
    // and therefore need to handle deletion.
    reply->deleteLater();
}

void TradingWindow::updateBuySellPrice() {
    buySellPrice = buySellPriceLabel->text().right(buySellPriceLabel->text().size()-1).toInt();
    priceSlider->setValue(0);
    totalValueAtPrice = silverAvailable*buySellPrice+poundAvailable;
    if((buySellQuantity-silverAvailable)*buySellPrice > poundAvailable) {
        buySellQuantity = poundAvailable/buySellPrice+silverAvailable;
        buySellQuantityLabel->setText(QString("%1kg").arg(buySellQuantity-silverAvailable,5,'d',3));
        //qDebug() << "adj quantity:" << buySellQuantity;
    }
    qDebug() << "price:" << buySellPrice;
}
void TradingWindow::updateBuySellPriceLabel(int value) {
    buySellPriceLabel->setText(QString("£%1").arg(buySellPrice+value));
}
// buySellQuantity refers to (buying + available metal), while the label display (buying +/-)
void TradingWindow::updateBuySellQuantity() {
    buySellQuantity = buySellQuantityLabel->text().left(buySellQuantityLabel->text().size()-2).toDouble()+silverAvailable;
    qDebug() << "quantity:" << buySellQuantity;
}
void TradingWindow::updateBuySellQuantityLabel(int value) {
    buySellQuantityLabel->setText(QString("%1kg").arg(totalValueAtPrice*value/100/buySellPrice-silverAvailable,5,'d',3));
}
void TradingWindow::resetBuySellQuantity() {
    buySellQuantity = silverAvailable;
    totalValueAtPrice = silverAvailable*buySellPrice+poundAvailable;
    if((buySellQuantity-silverAvailable)*buySellPrice > poundAvailable) {
        buySellQuantity = poundAvailable/buySellPrice+silverAvailable;
        qDebug() << "adj quantity:" << buySellQuantity;
    }
    quantitySlider->setValue((totalValueAtPrice-poundAvailable)/totalValueAtPrice*100);
    buySellPriceLabel->setText(QString("£%1").arg(buySellPrice));
    buySellQuantityLabel->setText(QString("%1kg").arg(buySellQuantity-silverAvailable,5,'d',3));
    qDebug() << buySellQuantity << " " << buySellPrice;
}

void TradingWindow::refreshAll() {
    getBalanceXml();
    getOrdersXml();
    getMarketXml();
    confirmWidget->setVisible(false);
    chartWidget->setVisible(true);
    cancelBuySellButton->setVisible(false);
    buySellButton->setVisible(true);

    buySellPrice = chart->getLastPrice();
    resetBuySellQuantity();
}

void TradingWindow::getBalance() {
    QNetworkRequest balanceRequest(QUrl("https://live.bullionvault.com/secure/view_balance.do"));
    qDebug() << "getting balance";
    nam->get(balanceRequest);
}

void TradingWindow::getBalanceXml() {
    QNetworkRequest balanceRequest(QUrl("https://live.bullionvault.com/secure/view_balance_xml.do"));
    nam->get(balanceRequest);
}

void TradingWindow::getOrders() {
    QNetworkRequest ordersRequest(QUrl("https://live.bullionvault.com/secure/view_orders.do"));
    ordersRequest.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");
    QByteArray data;
    QUrl params;
    params.addQueryItem("status","OPEN");
    data = params.encodedQuery();
    nam->post(ordersRequest,data);
}

void TradingWindow::getOrdersXml() {
    QNetworkRequest ordersRequest(QUrl("https://live.bullionvault.com/secure/view_orders_xml.do"));
    QByteArray data;
    QUrl params;
    params.addQueryItem("status","OPEN");
    data = params.encodedQuery();
    nam->post(ordersRequest,data);
}

void TradingWindow::getMarket() {
    QNetworkRequest marketRequest(QUrl("https://live.bullionvault.com/secure/view_market_depth.do?considerationCurrency=GBP&securityId=AGXLN&marketWidth=4&priceInterval=1"));
    nam->get(marketRequest);
}

void TradingWindow::getMarketXml() {
    QNetworkRequest marketRequest(QUrl("http://live.bullionvault.com/view_market_xml.do?considerationCurrency=GBP&securityId=AGXLN&marketWidth=3&quantity=0.01"));
    nam->get(marketRequest);
}

void TradingWindow::parseOrders(QString page) {
    openOrdersCB->clear();
    QString ordersSection = page.section("<tbody>",1,1).section("</tbody>",0,0);
    QStringListIterator listIterator(ordersSection.split("<tr"));
    if(listIterator.hasNext()) {
        listIterator.next(); // skip the "0" index and dummy first row
        listIterator.next();
    }
    while(listIterator.hasNext()) {
        QString orderSection = listIterator.next();
        QRegExp sep("<td[^>]*>");
        Order order(orderSection.section("id=\"",1,1).section("\"",0,0).toLong(),
                    orderSection.section(sep,3,3).section("</td>",0,0),
                    orderSection.section(sep,4,4).section("</td>",0,0),
                    orderSection.section(sep,5,5).section("</td>",0,0),
                    orderSection.section(sep,6,6).section("</td>",0,0).toInt(),
                    orderSection.section(sep,7,7).section("</td>",0,0).toDouble(),
                    orderSection.section(sep,9,9).section("</td>",0,0).trimmed());
        addOrder(order);
    }
}

void TradingWindow::parseOrdersXml(QString page) {
    openOrders->clear();
    openOrdersCB->clear();

    QDomDocument doc;
    doc.setContent(page);
    QDomNodeList positions=doc.elementsByTagName("order");
    for(int i1=0; i1<positions.size(); i1++) {
        QDomNamedNodeMap attrs = positions.at(i1).attributes();
        poundAvailable = attrs.namedItem("available").toAttr().value().toDouble();

        Order order(attrs.namedItem("orderId").toAttr().value().toLong(),
                    attrs.namedItem("actionIndicator").toAttr().value(),
                    attrs.namedItem("securityId").toAttr().value(),
                    attrs.namedItem("considerationCurrency").toAttr().value(),
                    attrs.namedItem("limit").toAttr().value().toInt(),
                    attrs.namedItem("quantity").toAttr().value().toDouble(),
                    attrs.namedItem("quantityMatched").toAttr().value());
        addOrder(order);
    }
}

void TradingWindow::killOrder() {
    qDebug() << "killing: " << openOrders->at(openOrdersCB->currentIndex()).id();
    QNetworkRequest killOrderRequest(QUrl(QString("https://live.bullionvault.com/secure/cancel_order.do?orderId=%1").arg(openOrders->at(openOrdersCB->currentIndex()).id())));
    nam->get(killOrderRequest);
    removeOrder(openOrdersCB->currentIndex());
}

void TradingWindow::addOrder(Order order) {
    openOrders->append(order);
    openOrdersCB->addItem(QString("%1 %2@%3 (%4)")
                          .arg(order.buysell()).arg(order.quantity()).arg(order.price()).arg(order.percentFulfilled()) ,Qt::DisplayRole);
    openOrdersCB->setCurrentIndex(openOrders->size()-1);
}
void TradingWindow::removeOrder(int index) {
    openOrders->removeAt(index);
    openOrdersCB->removeItem(index);
    openOrdersCB->setCurrentIndex(0);
}

void TradingWindow::placeOrder() {
    if(buySellQuantity-silverAvailable < 0) {
        finalBuySellQuantity = -1*(buySellQuantity-silverAvailable);
        confirmLabel->setText("sell "+QString::number(finalBuySellQuantity,'d',3)+" @ "+QString::number(buySellPrice,'d',2));
        qDebug() << "sell " << -1*(buySellQuantity-silverAvailable) << "@" << buySellPrice;
    } else {
        finalBuySellQuantity = (buySellQuantity-silverAvailable) * 0.98;
        confirmLabel->setText("buy "+QString::number(finalBuySellQuantity,'d',3)+" @ "+QString::number(buySellPrice,'d',2));
        qDebug() << "buy " << buySellQuantity-silverAvailable << "@" << buySellPrice;
    }
    confirmingOrder = true;
    showPlaceOrderDisplay();
}

QString TradingWindow::getConsiderationCurrency() {
    if(currentCurrency==QString("$")) {
        return QString("USD");
    } else if (currentCurrency==QString("£")) {
        return QString("GBP");
    } else if (currentCurrency==QString(QChar(8364))) {
        return QString("EUR");
    }
}

void TradingWindow::showPlaceOrderDisplay() {
    chartWidget->setVisible(false);
    confirmWidget->setVisible(true);

    //confirmButton->setVisible(true);
    cancelBuySellButton->setVisible(true);
    buySellButton->setVisible(false);
}

void TradingWindow::hidePlaceOrderDisplay() {
    chartWidget->setVisible(true);
    confirmWidget->setVisible(false);

    //confirmButton->setVisible(false);
    cancelBuySellButton->setVisible(false);
    buySellButton->setVisible(true);

    confirmingOrder = false;
}

void TradingWindow::placeOrderXml() {
    qDebug() << "ordering ... " << finalBuySellQuantity << "@" << buySellPrice;
    confirmWidget->setVisible(false);
    chartWidget->setVisible(true);

    QNetworkRequest request(QUrl("https://live.bullionvault.com/secure/place_order_xml.do"));

    QByteArray data;
    QUrl params;

    if(buySellQuantity-silverAvailable < 0)
        params.addQueryItem("actionIndicator","S");
    else if(buySellQuantity-silverAvailable > 0)
        params.addQueryItem("actionIndicator","B");
    else
        return;

    qDebug() << "b/s" << qAbs(buySellQuantity-silverAvailable);
    params.addQueryItem("considerationCurrency",getConsiderationCurrency());
    params.addQueryItem("securityId","AGXLN");
    params.addQueryItem("quantity",QString::number(finalBuySellQuantity,'d',3));
    params.addQueryItem("limit",QString::number(buySellPrice));
    params.addQueryItem("typeCode","TIL_CANCEL");
    params.addQueryItem("clientTransRef",QDateTime::currentDateTime().toString(QString("yyMMddhhmmsszzz")));
    params.addQueryItem("confirmed","true");
    params.addQueryItem("goodUntil","");
    data = params.encodedQuery();
    qDebug() << "query string: " << data;

    hidePlaceOrderDisplay();
    nam->post(request,data);
}

void TradingWindow::parseBalance(QString page) {
    QString silver = page.section("silverRow",2,2);
    QString dollars = page.section("currencyRow",2,2);
    QString pounds = page.section("currencyRow",3,3);
    QString euros = page.section("currencyRow",4,4);

    QRegExp sep("<td align=\"(right|left)\">");

    silverAvailableStr = convertHtmlCurrencySymbols(silver.section(sep,2,2).section("</td>",0,0));
    silverReservedStr = convertHtmlCurrencySymbols(silver.section(sep,3,3).section("</td>",0,0));
    silverTotalStr = convertHtmlCurrencySymbols(silver.section(sep,4,4).section("</td>",0,0));
    silverValuationStr = convertHtmlCurrencySymbols(silver.section(sep,5,5).section("</td>",0,0));
    dollarAvailableStr = convertHtmlCurrencySymbols(dollars.section(sep,2,2).section("</td>",0,0));
    dollarReservedStr = convertHtmlCurrencySymbols(dollars.section(sep,3,3).section("</td>",0,0));
    dollarTotalStr = convertHtmlCurrencySymbols(dollars.section(sep,4,4).section("</td>",0,0));
    dollarValuationStr = convertHtmlCurrencySymbols(dollars.section(sep,5,5).section("</td>",0,0));
    poundAvailableStr = convertHtmlCurrencySymbols(pounds.section(sep,2,2).section("</td>",0,0));
    poundReservedStr = convertHtmlCurrencySymbols(pounds.section(sep,3,3).section("</td>",0,0));
    poundTotalStr = convertHtmlCurrencySymbols(pounds.section(sep,4,4).section("</td>",0,0));
    poundValuationStr = convertHtmlCurrencySymbols(pounds.section(sep,5,5).section("</td>",0,0));
    euroAvailableStr = convertHtmlCurrencySymbols(euros.section(sep,2,2).section("</td>",0,0));
    euroReservedStr = convertHtmlCurrencySymbols(euros.section(sep,3,3).section("</td>",0,0));
    euroTotalStr = convertHtmlCurrencySymbols(euros.section(sep,4,4).section("</td>",0,0));
    euroValuationStr = convertHtmlCurrencySymbols(euros.section(sep,5,5).section("</td>",0,0));

    setAccountDataXml();

    silverValuation = silverValuationStr.right(silverValuationStr.size()-1).toDouble();
    dollarValuation = dollarValuationStr.right(dollarValuationStr.size()-1).toDouble();
    poundValuation = poundValuationStr.right(poundValuationStr.size()-1).toDouble();
    euroValuation = euroValuationStr.right(euroValuationStr.size()-1).toDouble();

    silverAvailable = silverAvailableStr.left(silverAvailableStr.size()-2).toDouble();
    silverReserved = silverReservedStr.left(silverReservedStr.size()-2).toDouble();
    silverTotal = silverTotalStr.left(silverTotalStr.size()-2).toDouble();
    silverAvailable = silverAvailable==0 ? silverTotal : silverAvailable;

    dollarAvailable = dollarAvailableStr.right(dollarAvailableStr.size()-1).toDouble();
    dollarReserved = dollarReservedStr.right(dollarReservedStr.size()-1).toDouble();
    dollarTotal = dollarTotalStr.right(dollarTotalStr.size()-1).toDouble();
    dollarAvailable = dollarAvailable==0 ? dollarTotal : dollarAvailable;

    poundAvailable = poundAvailableStr.right(poundAvailableStr.size()-1).toDouble();
    poundReserved = poundReservedStr.right(poundReservedStr.size()-1).toDouble();
    poundTotal = poundTotalStr.right(poundTotalStr.size()-1).toDouble();
    poundAvailable = poundAvailable==0 ? poundTotal : poundAvailable;

    euroAvailable = euroAvailableStr.right(euroAvailableStr.size()-1).toDouble();
    euroReserved = euroReservedStr.right(euroReservedStr.size()-1).toDouble();
    euroTotal = euroTotalStr.right(euroTotalStr.size()-1).toDouble();
    euroAvailable = euroAvailable==0 ? euroTotal : euroAvailable;

//    qDebug() << silverAvailable << " " << silverReserved << " " << silverTotal << " " << silverValuation;
//    qDebug() << dollarAvailable << " " << dollarReserved << " " << dollarTotal << " " << dollarValuation;
//    qDebug() << poundAvailable << " " << poundReserved << " " << poundTotal << " " << poundValuation;
//    qDebug() << euroAvailable << " " <<  euroReserved << " " << euroTotal << " " << euroValuation;

    initTrading();
    resetBuySellQuantity();
}

void TradingWindow::parseBalanceXml(QString page) {
    QDomDocument doc;
    doc.setContent(page);
    QDomNodeList positions=doc.elementsByTagName("clientPosition");
    for(int i1=0; i1<positions.size(); i1++) {
        QDomNamedNodeMap attrs = positions.at(i1).attributes();
        if(attrs.namedItem("securityId").toAttr().value() == "GBP") {
            poundAvailable = attrs.namedItem("available").toAttr().value().toDouble();
            poundTotal = attrs.namedItem("total").toAttr().value().toDouble();
            poundValuation = attrs.namedItem("totalValuation").toAttr().value().toDouble();
            poundReserved = poundTotal-poundAvailable;
        } else if(attrs.namedItem("securityId").toAttr().value() == "USD") {
            dollarAvailable = attrs.namedItem("available").toAttr().value().toDouble();
            dollarTotal = attrs.namedItem("total").toAttr().value().toDouble();
            dollarValuation = attrs.namedItem("totalValuation").toAttr().value().toDouble();
            dollarReserved = dollarTotal-dollarAvailable;
        } else if(attrs.namedItem("securityId").toAttr().value() == "EUR") {
            euroAvailable = attrs.namedItem("available").toAttr().value().toDouble();
            euroTotal = attrs.namedItem("total").toAttr().value().toDouble();
            euroValuation = attrs.namedItem("totalValuation").toAttr().value().toDouble();
            euroReserved = euroTotal-euroAvailable;
        } else if(attrs.namedItem("securityId").toAttr().value() == "AGXLN") {
            silverAvailable = attrs.namedItem("available").toAttr().value().toDouble();
            silverTotal = attrs.namedItem("total").toAttr().value().toDouble();
            silverValuation = attrs.namedItem("totalValuation").toAttr().value().toDouble();
            silverReserved = silverTotal-silverAvailable;
        }
    }
//    qDebug() << silverAvailable << " " << silverReserved << " " << silverTotal << " " << silverValuation;
//    qDebug() << dollarAvailable << " " << dollarReserved << " " << dollarTotal << " " << dollarValuation;
//    qDebug() << poundAvailable << " " << poundReserved << " " << poundTotal << " " << poundValuation;
//    qDebug() << euroAvailable << " " <<  euroReserved << " " << euroTotal << " " << euroValuation;
    setAccountDataXml();

    initTrading();
    resetBuySellQuantity();
}

void TradingWindow::parseMarketXml(QString page) {
    QDomDocument doc;
    doc.setContent(page);
    QDomNodeList prices=doc.elementsByTagName("price");

    buyOrder3Label->setText(prices.at(2).attributes().namedItem("quantity").toAttr().value()+"kg\n£"+
                             prices.at(2).attributes().namedItem("limit").toAttr().value());
    buyOrder2Label->setText(prices.at(1).attributes().namedItem("quantity").toAttr().value()+"kg\n£"+
                             prices.at(1).attributes().namedItem("limit").toAttr().value());
    buyOrder1Label->setText(prices.at(0).attributes().namedItem("quantity").toAttr().value()+"kg\n£"+
                             prices.at(0).attributes().namedItem("limit").toAttr().value());
    sellOrder1Label->setText(prices.at(3).attributes().namedItem("quantity").toAttr().value()+"kg\n£"+
                             prices.at(3).attributes().namedItem("limit").toAttr().value());
    sellOrder2Label->setText(prices.at(4).attributes().namedItem("quantity").toAttr().value()+"kg\n£"+
                             prices.at(4).attributes().namedItem("limit").toAttr().value());
    sellOrder3Label->setText(prices.at(5).attributes().namedItem("quantity").toAttr().value()+"kg\n£"+
                             prices.at(5).attributes().namedItem("limit").toAttr().value());
}

void TradingWindow::setAccountDataXml() {
    accountAvailableLabel->setText(
                                   (currentMetal==QString("Silver") ? QString::number(silverAvailable, 'd',3)+"kg" : "") +
                                   (currentCurrency==QString("$") ? "\n$"+QString::number(dollarAvailable, 'd',2) : "") +
                                   (currentCurrency==QString("£") ? "\n£"+QString::number(poundAvailable, 'd',2) : "") +
                                   (currentCurrency==QString(QChar(8364)) ? "\n"+QString(QChar(8364))+QString::number(euroAvailable, 'd',2) : ""));
    accountReservedLabel->setText(
                                   (currentMetal==QString("Silver") ? QString::number(silverReserved, 'd',3)+"kg" : "") +
                                   (currentCurrency==QString("$") ? "\n$"+QString::number(dollarReserved, 'd',2) : "") +
                                   (currentCurrency==QString("£") ? "\n£"+QString::number(poundReserved, 'd',2) : "") +
                                   (currentCurrency==QString(QChar(8364)) ? "\n"+QString(QChar(8364))+QString::number(euroReserved, 'd',2) : ""));
    accountTotalLabel->setText(
                                   (currentMetal==QString("Silver") ? QString::number(silverTotal, 'd',3)+"kg" : "") +
                                   (currentCurrency==QString("$") ? "\n$"+QString::number(dollarTotal, 'd',2) : "") +
                                   (currentCurrency==QString("£") ? "\n£"+QString::number(poundTotal, 'd',2) : "") +
                                   (currentCurrency==QString(QChar(8364)) ? "\n"+QString(QChar(8364))+QString::number(euroTotal, 'd',2) : ""));
    accountValuationLabel->setText(
                                   (currentMetal==QString("Silver") ? QString::number(silverValuation, 'd',2) : "") +
                                   (currentCurrency==QString("$") ? "\n£"+QString::number(dollarValuation, 'd',2) : "") +
                                   (currentCurrency==QString("£") ? "\n£"+QString::number(poundValuation, 'd',2) : "") +
                                   (currentCurrency==QString(QChar(8364)) ? "\n£"+QString::number(euroValuation, 'd',2) : ""));
}

void TradingWindow::dollarPressed() { currentCurrency = new QString("$"); setAccountDataXml(); }
void TradingWindow::poundPressed() { currentCurrency = new QString("£"); setAccountDataXml(); }
void TradingWindow::euroPressed() { currentCurrency = new QString(QChar(8364)); setAccountDataXml(); }

QString TradingWindow::convertHtmlCurrencySymbols(QString string) {
    string.replace(QString("&euro;"),QString(QChar(8364)));
    string.replace(QString("&pound;"),QString("£"));
    string.replace(QString("&nbsp;"),QString(" "));
    string.replace(",","");
    return string.trimmed()=="" ? "--" : string;;
}

void TradingWindow::initTrading() {
    double totalBuying = 0.0;
    double totalSelling = 0.0;
    for(int i1=0;i1<openOrders->size();i1++) {
        if(openOrders->at(i1).buysell()=="BUY")
            totalBuying += openOrders->at(i1).quantity();
        else
            totalSelling += openOrders->at(i1).quantity();
    }
}

/************************** GUI **************************/

void TradingWindow::setOrientation(MainWindow::ScreenOrientation orientation)
{
#if defined(Q_OS_SYMBIAN)
    // If the version of Qt on the device is < 4.7.2, that attribute won't work
    if (orientation != MainWindow::ScreenOrientationAuto) {
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
    case MainWindow::ScreenOrientationLockPortrait:
        attribute = static_cast<Qt::WidgetAttribute>(128);
        break;
    case MainWindow::ScreenOrientationLockLandscape:
        attribute = static_cast<Qt::WidgetAttribute>(129);
        break;
    default:
    case MainWindow::ScreenOrientationAuto:
        attribute = static_cast<Qt::WidgetAttribute>(130);
        break;
#else // QT_VERSION < 0x040702
    case MainWindow::ScreenOrientationLockPortrait:
        attribute = Qt::WA_LockPortraitOrientation;
        break;
    case MainWindow::ScreenOrientationLockLandscape:
        attribute = Qt::WA_LockLandscapeOrientation;
        break;
    default:
    case MainWindow::ScreenOrientationAuto:
        attribute = Qt::WA_AutoOrientation;
        break;
#endif // QT_VERSION < 0x040702
    };
    setAttribute(attribute, true);
}

void TradingWindow::showExpanded() {
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    showMaximized();
#else
    show();
#endif
    qDebug() << "showing chart?";
    chart->showSized(350, 220);
    login();
}

void TradingWindow::mousePressEvent(QMouseEvent *event) {
    oldPoint = event->pos();
}

void TradingWindow::mouseReleaseEvent(QMouseEvent *event) {
    QPoint newPoint = event->pos();
    if(confirmingOrder && oldPoint.x()<90 && oldPoint.y()<40
            && newPoint.x()>90 && oldPoint.x()<180 && newPoint.y()<40)
        placeOrderXml();
    else if(oldPoint.x()<90 && oldPoint.y()<40 &&
            newPoint.x()<90 && newPoint.y()<40) {
        hide();
        mainWindow->showExpanded();
    }
}

void TradingWindow::setMainWindow(MainWindow *theMainWindow) { mainWindow = theMainWindow; }

void TradingWindow::setChart(Chart *theChart) {
    chart = theChart;
}

