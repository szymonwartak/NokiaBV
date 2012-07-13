#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QList>
#include <QString>
#include <QPair>

class Constants
{
public:
    static QList<QPair<QString,QString>*>* getMetal();
    static QList<QPair<QString,QString>*>* getCurrency();
    static QList<QPair<QString,QString>*>* getStyle();
    static QList<QPair<QString,QString>*>* getDateRange();
    static int getMetalIndexByKey(QString key);
    static int getCurrencyIndexByKey(QString key);
    static int getStyleIndexByKey(QString key);
    static int getDateRangeIndexByKey(QString key);


private:
    QList<QPair<QString,QString>*> *metal, *currency, *style, *dateRange;
    static Constants *m_instance;

    Constants();
    Constants(Constants const&){}
    Constants& operator=(Constants const&){}
    static Constants* instance();
    QList<QPair<QString,QString>*>* getCurrentMetal() { return metal; }
    QList<QPair<QString,QString>*>* getCurrentCurrency() { return currency; }
    QList<QPair<QString,QString>*>* getCurrentStyle() { return style; }
    QList<QPair<QString,QString>*>* getCurrentDateRange() { return dateRange; }
};

#endif // CONSTANTS_H
