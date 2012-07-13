#ifndef DATA_H
#define DATA_H

#include <QObject>
#include <QUrl>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QFile>
#include <QTextStream>
#include <QStringListIterator>
#include <QVariant>
#include <QDateTime>

class Bar;
class Chart;
class Data : public QObject
{
    Q_OBJECT

public:
    static void setChart(Chart *chart);
    static void setUrlChart(QString *chartUrl);
    static void updateChartData();
    static int getLastChartPrice();
    static QList<Bar>* getDataBars();
    static QList<double>* getDataPoints();
    static Data* instance();

    static int cmetal, ccurrency, cstyle, cdateRange;
    static double xmin, xmax, ymin, ymax;
    static QDateTime ctime;

private slots:
    void finishedSlotChart(QNetworkReply* reply);

private:
    Data();
    Data(Data const&){}
    Data& operator=(Data const&){}
    static Data *m_instance;

    void l_setChart(Chart *chart);
    void l_setUrlChart(QString *chartUrl);
    void l_updateChartData();
    int l_getLastChartPrice();
    QList<Bar>* l_getDataBars();
    QList<double>* l_getDataPoints();
    void parsePrices(QString page);

    QList<Bar> *dataBars;
    QList<double> *dataPoints;
    QNetworkAccessManager *namChart;
    QUrl *urlChart;
    Chart *chart;
    double lastChartPrice;
};

#endif // DATA_H
