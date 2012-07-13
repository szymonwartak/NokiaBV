#include "constants.h"

Constants* Constants::m_instance = NULL;
Constants* Constants::instance() {
    if (!m_instance)   // Only allow one instance of class to be generated.
        m_instance = new Constants();
    return m_instance;
}

Constants::Constants() {
    metal = new QList<QPair<QString,QString>*>();
    metal->append(new QPair<QString,QString>(QString("AGX"),QString("Silver")));
    metal->append(new QPair<QString,QString>(QString("AUX"),QString("Gold")));

    style = new QList<QPair<QString,QString>*>();
    style->append(new QPair<QString,QString>(QString("CHART_LINE"),QString("Line Chart")));
    style->append(new QPair<QString,QString>(QString("CHART_BAR_HLC"),QString("Bar Chart")));

    /*5 - 10 minutes, 15 - 1 hour, 120 - 6 hours, 600 - day, 3600 - week, 14400 - month,
    43200 - quarter, 172800 - year, 864000 - 5 years, 2592000 - 20 years*/
    dateRange = new QList<QPair<QString,QString>*>();
    dateRange->append(new QPair<QString,QString>(QString("5"),QString("10 minutes")));
    dateRange->append(new QPair<QString,QString>(QString("15"),QString("1 hour")));
    dateRange->append(new QPair<QString,QString>(QString("120"),QString("6 hours")));
    dateRange->append(new QPair<QString,QString>(QString("600"),QString("1 day")));
    dateRange->append(new QPair<QString,QString>(QString("3600"),QString("1 week")));
    dateRange->append(new QPair<QString,QString>(QString("14400"),QString("1 month")));
    dateRange->append(new QPair<QString,QString>(QString("43200"),QString("1 quarter")));
    dateRange->append(new QPair<QString,QString>(QString("172800"),QString("1 year")));
    dateRange->append(new QPair<QString,QString>(QString("864000"),QString("5 years")));
    dateRange->append(new QPair<QString,QString>(QString("2592000"),QString("20 years")));

    currency = new QList<QPair<QString,QString>*>();
    currency->append(new QPair<QString,QString>(QString("USD"),QString("USD")));
    currency->append(new QPair<QString,QString>(QString("EUR"),QString("EUR")));
    currency->append(new QPair<QString,QString>(QString("GBP"),QString("GBP")));
//    currency->append(new QPair<QString,QString>(QString("CHF"),QString("CHF")));
//    currency->append(new QPair<QString,QString>(QString("CAD"),QString("CAD")));
//    currency->append(new QPair<QString,QString>(QString("AUD"),QString("AUD")));
//    currency->append(new QPair<QString,QString>(QString("JPY"),QString("JPY")));
}

QList<QPair<QString,QString>*>* Constants::getMetal() { return Constants::instance()->getCurrentMetal(); }
QList<QPair<QString,QString>*>* Constants::getCurrency() { return Constants::instance()->getCurrentCurrency(); }
QList<QPair<QString,QString>*>* Constants::getStyle() { return Constants::instance()->getCurrentStyle(); }
QList<QPair<QString,QString>*>* Constants::getDateRange() { return Constants::instance()->getCurrentDateRange(); }

int Constants::getMetalIndexByKey(QString key) {
    for(int i1=0; i1<Constants::getMetal()->size(); i1++) {
        if(Constants::getMetal()->at(i1)->first == key)
            return i1;
    }
    return -1;
}

int Constants::getCurrencyIndexByKey(QString key) {
    for(int i1=0; i1<Constants::getCurrency()->size(); i1++) {
        if(Constants::getCurrency()->at(i1)->first == key)
            return i1;
    }
    return -1;
}

int Constants::getStyleIndexByKey(QString key) {
    for(int i1=0; i1<Constants::getStyle()->size(); i1++) {
        if(Constants::getStyle()->at(i1)->first == key)
            return i1;
    }
    return -1;
}

int Constants::getDateRangeIndexByKey(QString key) {
    for(int i1=0; i1<Constants::getDateRange()->size(); i1++) {
        if(Constants::getDateRange()->at(i1)->first == key)
            return i1;
    }
    return -1;
}

