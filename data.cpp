#include "data.h"
#include "bar.h"
#include "chart.h"
#include "constants.h"

Data* Data::m_instance = NULL;
Data* Data::instance() {
    if (!m_instance)   // Only allow one instance of class to be generated.
        m_instance = new Data();
    return m_instance;
}

Data::Data() {
    namChart = new QNetworkAccessManager(this);
    QObject::connect(namChart, SIGNAL(finished(QNetworkReply*)),
             this, SLOT(finishedSlotChart(QNetworkReply*)));

    QFile filer("goldapp.txt");
    filer.deleteLater();
    filer.open(QIODevice::ReadOnly | QIODevice::Text);
    QTextStream stream(&filer);
    QString line = stream.readLine();
    filer.close(); // optional, as QFile destructor will already do it

    if(!line.trimmed().isEmpty()) {
        QStringListIterator inputIterator(line.split("/"));
        cstyle = Constants::getStyleIndexByKey(inputIterator.next());
        cmetal = Constants::getMetalIndexByKey(inputIterator.next());
        ccurrency = Constants::getCurrencyIndexByKey(inputIterator.next());
        cdateRange = Constants::getDateRangeIndexByKey(inputIterator.next());
        urlChart->setUrl("http://www.galmarley.com/prices/"+line);
    } else {
        urlChart->setUrl("http://www.galmarley.com/prices/CHART_BAR_HLC/AUX/USD/172800/Full");
    }
}

void Data::setChart(Chart *chart) { Data::instance()->l_setChart(chart); }
void Data::l_setChart(Chart *chart) { this->chart = chart; }

void Data::setUrlChart(QString *urlChart) { Data::instance()->l_setUrlChart(urlChart); }
void Data::l_setUrlChart(QString *urlChart) { this->urlChart->setUrl(*urlChart); }

void Data::updateChartData() { Data::instance()->l_updateChartData(); }
void Data::l_updateChartData() { namChart->get(QNetworkRequest(*urlChart)); }

void Data::finishedSlotChart(QNetworkReply* reply){
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
        QString string(bytes); // string
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

void Data::parsePrices(QString page) {
    bool isPriceUpdated = false; // prices in the feed are from newest to oldest so we want the first
    Data::ctime = QDateTime::fromString(page.section("<latest_timestamp>",1,1).left(14), "yyyyMMddhhmmss");
    if(cstyle%Constants::getStyle()->size() == 1) {
        dataBars = new QList<Bar>();
        QStringListIterator listIterator(page.split("<bar>"));
        if(listIterator.hasNext()) {
            listIterator.next(); // skip the "0" index
            xmin = 999999; xmax = 0; ymin = 99999999; ymax = 0;
        }
        while(listIterator.hasNext()) {
            QString nextBar = listIterator.next();
            QString lowStr = nextBar.section("<low>",1,1).section("</low>",0,0);
            QString highStr = nextBar.section("<high>",1,1).section("</high>",0,0);
            QString closeStr = nextBar.section("<close>",1,1).section("</close>",0,0);
            if(closeStr != "") {
                if(lowStr.toDouble() < ymin) ymin = lowStr.toDouble();
                if(highStr.toDouble() > ymax) ymax = highStr.toDouble();
                dataBars->prepend(Bar(lowStr.toDouble(), highStr.toDouble(), closeStr.toDouble()));
            } else {
                dataBars->prepend(Bar());
            }
            if(!isPriceUpdated && closeStr.toDouble() > 0) {
                isPriceUpdated = true;
                lastChartPrice = closeStr.toDouble();
            }
        }
        xmax = dataBars->size()+1.5;
    } else if(cstyle%Constants::getStyle()->size() == 0) {
        dataPoints = new QList<double>();
        QStringListIterator listIterator(page.split("<point>"));
        if(listIterator.hasNext()) {
            listIterator.next(); // skip the "0" index
            xmin = 999999; xmax = 0; ymin = 999999; ymax = 0;
        }
        while(listIterator.hasNext()) {
            QString nextPoint = listIterator.next();
            QString priceStr = nextPoint.section("<price>",1,1).section("</price>",0,0);
            if(priceStr != "") {
                if(priceStr.toDouble() < ymin) ymin = priceStr.toDouble();
                if(priceStr.toDouble() > ymax) ymax = priceStr.toDouble();
                dataPoints->prepend(priceStr.toDouble());
            } else {
                dataPoints->prepend(-1);
            }
            if(!isPriceUpdated && priceStr.toDouble() > 0) {
                isPriceUpdated = true;
                lastChartPrice = priceStr.toDouble();
            }
        }
        xmax = dataPoints->size()+1.5;
    }
    xmin = 0;
    double ydiff = ymax - ymin;
    ymin = ymin-ydiff/20;
    ymax = ymax+ydiff/20;

    // 'callback' to chart to update once the data is parsed
    chart->update();
}

QList<Bar> *Data::getDataBars() { return Data::instance()->l_getDataBars(); }
QList<Bar> *Data::l_getDataBars() { return dataBars; }

QList<double> *Data::getDataPoints() { return Data::instance()->l_getDataPoints(); }
QList<double> *Data::l_getDataPoints() { return dataPoints; }

int Data::getLastChartPrice() { return Data::instance()->l_getLastChartPrice(); }
int Data::l_getLastChartPrice() { return lastChartPrice; }
