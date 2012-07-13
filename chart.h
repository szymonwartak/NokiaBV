#ifndef CHART_H
#define CHART_H

#include <QWidget>
#include <QPainter>
#include <QtGui>
#include <QTimer>
#include <mainwindow.h>
#include <data.h>

class Bar;
class Chart : public QWidget
{
    Q_OBJECT

public:

    explicit Chart(QWidget *parent = 0, Data *theData = 0);
    ~Chart();
    void showExpanded();
    void showSized(int width, int height);
    void setOrientation(MainWindow::ScreenOrientation orientation);
    void setMainWindow(MainWindow *theMainWindow);
    void setSmallTheme();
    void setLargeTheme();
    void updatePrices();
    void showPrices(); // shows the chart without refetching data
    int getLastPrice();

protected:
    void paintEvent(QPaintEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseDoubleClickEvent(QMouseEvent *event);

private slots:
    void closeChart();

private:
    void drawGrid(QPainter *painter);
    void drawBars(QPainter *painter);
    void drawPoints(QPainter *painter);
    void drawLine(double x1, double y1, double x2, double y2, QPainter *painter);
    int chartHeight();
    int chartWidth();
    QString getXLabel(double part, int whole);
    QColor getArrowColour(int arrowIndex);

    MainWindow *mainWindow;
    QList<QPair<QString,QString>*> *metal, *currency, *style, *dateRange;
    QPoint newPoint, oldPoint;
    QPoint *windowOffset; // offset of the crop window over the chart
    QTimer timer;
    int theme; // 1 - small, 9 - large (full window)
    int axisFontSize, menuFontSize;
    int menuWidth, menuHeight, refreshWidth;
    int windowWidth, windowHeight;
    int xAxisTick, yAxisTick;
    int axisLabelWidth, axisLabelHeight;
    bool moving, showCloseButton;

};

#endif // CHART_H
