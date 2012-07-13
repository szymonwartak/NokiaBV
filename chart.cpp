#include "chart.h"
#include "bar.h"
#include "constants.h"
#include "data.h"

Chart::Chart(QWidget *parent, Data *theData) :
    QWidget(parent)
{
    windowOffset = new QPoint(0,0);
    moving = false;
    setSmallTheme();
    showCloseButton = true;
    refreshWidth = menuHeight;

    metal = Constants::getMetal();
    style = Constants::getStyle();
    dateRange = Constants::getDateRange();
    currency = Constants::getCurrency();
}

Chart::~Chart()
{
}

void Chart::setSmallTheme() {
    theme = 1;
    axisLabelWidth = 50;
    axisLabelHeight = 15;
    menuHeight = 30;
    menuWidth = 75;
    axisFontSize = 5;
    xAxisTick = 50;
    yAxisTick = 25;

    showCloseButton = false;
}

void Chart::setLargeTheme() {
    theme = 9;
    axisLabelWidth = 75;
    axisLabelHeight = 20;
    menuHeight = 40;
    menuWidth = 75;
    axisFontSize = 7;
    xAxisTick = 100;
    yAxisTick = 50;

    showCloseButton = true;
}

void Chart::showExpanded() {
#if defined(Q_OS_SYMBIAN) || defined(Q_WS_SIMULATOR)
    showFullScreen();
#elif defined(Q_WS_MAEMO_5)
    showMaximized();
#else
    show();
#endif
    // these dimensions assume the landscape orientation on the C7
    windowWidth = 640;//this->window()->width();
    windowHeight = 360;//this->window()->height();
    menuWidth = (windowWidth-refreshWidth)/4;
    update();
}

void Chart::showSized(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    menuWidth = (windowWidth-refreshWidth)/4;
    show();
    update();

}

void Chart::setMainWindow(MainWindow *theMainWindow) { mainWindow = theMainWindow; }

void Chart::setOrientation(MainWindow::ScreenOrientation orientation)
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

QColor Chart::getArrowColour(int arrowIndex) {
    if(oldPoint.y() > chartHeight()+axisLabelHeight && oldPoint.x()*2/menuWidth == arrowIndex)
        return Qt::black;
    return Qt::white;
}

QString Chart::getXLabel(double part, int whole) {
    if(Data::cdateRange==0) { // 10 min
        return Data::ctime.addSecs((part-whole)/whole*10*60).toString("hh:mm:ss");
    } else if(Data::cdateRange==1) { // 1 hour
        return Data::ctime.addSecs((part-whole)/whole*60*60).toString("hh:mm");
    } else if(Data::cdateRange==2) { // 6 hours
        return Data::ctime.addSecs((part-whole)/whole*6*60*60).toString("hh:mm");
    } else if(Data::cdateRange==3) { // 1 day
        return Data::ctime.addSecs((part-whole)/whole*24*60*60).toString("hh:mm");
    } else if(Data::cdateRange==4) { // 1 week
        return Data::ctime.addSecs((part-whole)/whole*7*24*60*60).toString("dd/MM");
    } else if(Data::cdateRange==5) { // 1 month
        return Data::ctime.addDays((part-whole)/whole*28).toString("dd/MM");
    } else if(Data::cdateRange==6) { // 1 quarter
        return Data::ctime.addDays((part-whole)/whole*59).toString("dd/MM");
    } else if(Data::cdateRange==7) { // 1 year
        return Data::ctime.addDays((part-whole)/whole*365).toString("dd/MM");
    } else if(Data::cdateRange==8) { // 5 years
        return Data::ctime.addDays((part-whole)/whole*5*365).toString("MM/yy");
    } else if(Data::cdateRange==9) { // 20 years
        return Data::ctime.addYears((part-whole)/whole*20).toString("yyyy");
    }
    return QString("");
}

void Chart::drawGrid(QPainter *painter) {
    painter->setPen(QPen(Qt::white, 1, Qt::DotLine, Qt::SquareCap, Qt::MiterJoin));
    painter->setFont(QFont("Arial", axisFontSize));

    // x-axis
    QString xlabel;
    for(double t1=-(windowOffset->x()%xAxisTick); t1<windowWidth-axisLabelWidth/2; t1+=xAxisTick) {
        if(t1>axisLabelWidth) {
            painter->drawLine(t1,0,t1,chartHeight());
            //xlabel = QString("%1").arg(Data::xmin+(Data::xmax-Data::xmin)*(t1/chartWidth()), 6, 'f', 0);
            xlabel = getXLabel(t1-axisLabelWidth, chartWidth());
            painter->drawText(t1-axisLabelWidth/2,chartHeight(),axisLabelWidth,axisLabelHeight, Qt::AlignCenter,xlabel);
        }
    }

    // y-axis
    for(double t1=-(windowOffset->y()%yAxisTick); t1<chartHeight(); t1+=yAxisTick) {
        if(t1>axisLabelHeight) {
            painter->drawLine(windowWidth-chartWidth(),t1,windowWidth,t1);
            painter->drawText(0,t1-axisLabelHeight/2,axisLabelWidth,axisLabelHeight, Qt::AlignCenter,QString("%1").arg(Data::ymax-(Data::ymax-Data::ymin)*(t1/chartHeight()), 6, 'f', Data::cmetal==0 ? 1 : 0));
        }
    }

    // menu
    int padding = 2; QColor buttonColor = QColor::fromRgb(200,200,0);
    painter->fillRect(0*menuWidth+padding, chartHeight()+axisLabelHeight+padding, menuWidth-2*padding, menuHeight-2*padding, buttonColor);
    painter->fillRect(1*menuWidth+padding, chartHeight()+axisLabelHeight+padding, menuWidth-2*padding, menuHeight-2*padding, buttonColor);
    painter->fillRect(2*menuWidth+padding, chartHeight()+axisLabelHeight+padding, menuWidth-2*padding, menuHeight-2*padding, buttonColor);
    painter->fillRect(3*menuWidth+padding, chartHeight()+axisLabelHeight+padding, menuWidth-2*padding, menuHeight-2*padding, buttonColor);
    painter->drawText(0*menuWidth, chartHeight()+axisLabelHeight, menuWidth, menuHeight, Qt::AlignCenter, metal->at(Data::cmetal%metal->size())->second);
    painter->drawText(1*menuWidth, chartHeight()+axisLabelHeight, menuWidth, menuHeight, Qt::AlignCenter, currency->at(Data::ccurrency%currency->size())->second);
    painter->drawText(2*menuWidth, chartHeight()+axisLabelHeight, menuWidth, menuHeight, Qt::AlignCenter, dateRange->at(Data::cdateRange%dateRange->size())->second);
    painter->drawText(3*menuWidth, chartHeight()+axisLabelHeight, menuWidth, menuHeight, Qt::AlignCenter, style->at(Data::cstyle%style->size())->second);

    // refresh "button"
    painter->setPen(QPen(Qt::white, 3, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    int refreshPadding = 4*padding;
    painter->fillRect(4*menuWidth+padding, chartHeight()+axisLabelHeight+padding, refreshWidth-2*padding, menuHeight-2*padding, buttonColor);
    painter->drawArc(windowWidth-refreshWidth+refreshPadding, chartHeight()+axisLabelHeight+refreshPadding, refreshWidth-2*refreshPadding, menuHeight-2*refreshPadding, 135*16, 135*16);
    painter->drawArc(windowWidth-refreshWidth+refreshPadding, chartHeight()+axisLabelHeight+refreshPadding, refreshWidth-2*refreshPadding, menuHeight-2*refreshPadding, 315*16, 135*16);
    painter->drawLine(windowWidth-refreshWidth/2, chartHeight()+axisLabelHeight+refreshPadding, windowWidth-refreshWidth/2+10, chartHeight()+axisLabelHeight+refreshPadding-2);
    painter->drawLine(windowWidth-refreshWidth/2, chartHeight()+axisLabelHeight+refreshPadding, windowWidth-refreshWidth/2+4, chartHeight()+axisLabelHeight+refreshPadding+8);
    painter->drawLine(windowWidth-refreshWidth/2, chartHeight()+axisLabelHeight+menuHeight-refreshPadding, windowWidth-refreshWidth/2-10, chartHeight()+axisLabelHeight+menuHeight-refreshPadding+2);
    painter->drawLine(windowWidth-refreshWidth/2, chartHeight()+axisLabelHeight+menuHeight-refreshPadding, windowWidth-refreshWidth/2-4, chartHeight()+axisLabelHeight+menuHeight-refreshPadding-8);

    if(showCloseButton) {
        // "close" button
        int closePadding = 6*padding;
        painter->fillRect(windowWidth-menuHeight+padding, padding, menuHeight-2*padding, menuHeight-2*padding, buttonColor);
        painter->drawLine(windowWidth-menuHeight+closePadding, closePadding, windowWidth-closePadding, menuHeight-closePadding);
        painter->drawLine(windowWidth-closePadding, closePadding, windowWidth-menuHeight+closePadding, menuHeight-closePadding);
    }
}

void Chart::drawBars(QPainter *painter) {
    painter->setPen(QPen(Qt::white, 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    int lastIndex = -1;
    for(int i1=0; i1<Data::instance()->getDataBars()->size(); i1++) {
        Bar bar = Data::instance()->getDataBars()->at(i1);
        if(bar.hasData() && lastIndex!=-1) {
            drawLine(i1+2, bar.getLow(), i1+2, bar.getHigh(), painter);
            Bar lastBar = Data::instance()->getDataBars()->at(lastIndex);
            drawLine(lastIndex+2, lastBar.getClose(), i1+2, lastBar.getClose(), painter);
            lastIndex = i1;
        } else if (bar.hasData()){
            drawLine(0, bar.getClose(), i1+2, bar.getClose(), painter);
            lastIndex = i1;
        }
    }
}

void Chart::drawPoints(QPainter *painter) {
    painter->setPen(QPen(Qt::white, 2, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    int lastIndex = -1;
    for(int i1=0; i1<Data::instance()->getDataPoints()->size(); i1++) {
        if(Data::instance()->getDataPoints()->at(i1)!=-1 && lastIndex!=-1) {
            drawLine(lastIndex+2, Data::instance()->getDataPoints()->at(lastIndex), i1+2, Data::instance()->getDataPoints()->at(i1), painter);
            lastIndex = i1;
        } else if(Data::instance()->getDataPoints()->at(i1)!=-1) { // for the first bar
            drawLine(0, Data::instance()->getDataPoints()->at(i1), i1+2, Data::instance()->getDataPoints()->at(i1), painter);
            lastIndex = i1;
        }
    }
}

void Chart::drawLine(double x1, double y1, double x2, double y2, QPainter *painter) {
    int x1i = windowWidth-chartWidth()+(x1-Data::xmin)/(Data::xmax-Data::xmin)*chartWidth() - windowOffset->x();
    int y1i = chartHeight() - (y1-Data::ymin)/(Data::ymax-Data::ymin)*chartHeight() - windowOffset->y();
    int x2i = windowWidth-chartWidth()+(x2-Data::xmin)/(Data::xmax-Data::xmin)*chartWidth() - windowOffset->x();
    int y2i = chartHeight() - (y2-Data::ymin)/(Data::ymax-Data::ymin)*chartHeight() - windowOffset->y();
    painter->drawLine(x1i, y1i, x2i, y2i);
}

int Chart::chartHeight() { return windowHeight - menuHeight - axisLabelHeight; }
int Chart::chartWidth() { return windowWidth - axisLabelWidth; }

void Chart::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    drawGrid(&painter);
    if(Data::cstyle%style->size() == 1)
        drawBars(&painter);
    else if (Data::cstyle%style->size() == 0)
        drawPoints(&painter);
}

void Chart::mouseMoveEvent(QMouseEvent *event) {
    if(moving) {
        newPoint = event->pos();
        windowOffset->setX(windowOffset->x()+(oldPoint.x()-newPoint.x()));
        windowOffset->setY(windowOffset->y()+(oldPoint.y()-newPoint.y()));
        oldPoint = newPoint;
        update();
    }
}

void Chart::mousePressEvent(QMouseEvent *event) {
    //moving = true;
    oldPoint = event->pos();
    //update(); // will be used for highlighting buttons clicked (redrawing needs to be improved for this to not block)
}

void Chart::mouseReleaseEvent(QMouseEvent *event) {
    //moving = false;
    // "button clicks" to change currency, dateRange, style and metal
    if(oldPoint.y() > chartHeight()+axisLabelHeight && event->y() > chartHeight()+axisLabelHeight
            && oldPoint.x()*2/menuWidth==event->x()*2/menuWidth) {
        if(oldPoint.x()*2/menuWidth == 0) { Data::cmetal--; if(Data::cmetal<0) Data::cmetal+=metal->size(); }
        if(oldPoint.x()*2/menuWidth == 1) Data::cmetal++;
        else if(oldPoint.x()*2/menuWidth == 2) { Data::ccurrency--; if(Data::ccurrency<0) Data::ccurrency+=currency->size(); }
        else if(oldPoint.x()*2/menuWidth == 3) Data::ccurrency++;
        else if(oldPoint.x()*2/menuWidth == 4) { Data::cdateRange--; if(Data::cdateRange<0) Data::cdateRange+=dateRange->size(); }
        else if(oldPoint.x()*2/menuWidth == 5) Data::cdateRange++;
        else if(oldPoint.x()*2/menuWidth == 6) { Data::cstyle--; if(Data::cstyle<0) Data::cstyle+=style->size(); }
        else if(oldPoint.x()*2/menuWidth == 7) Data::cstyle++;
        //else qDebug() << "no change = refresh";

        QString *newurl = new QString("http://www.galmarley.com/prices/" + style->at(Data::cstyle%style->size())->first
                                 + "/" + metal->at(Data::cmetal%metal->size())->first
                                 + "/" + currency->at(Data::ccurrency%currency->size())->first
                                 + "/" + dateRange->at(Data::cdateRange%dateRange->size())->first + "/Full");
        Data::setUrlChart(newurl);
        //url.setUrl(newurl);
    } else if(oldPoint.y() < menuHeight && event->y() < menuHeight
              && (windowWidth-oldPoint.x())/menuHeight==0 && (windowWidth-event->x())/menuHeight==0) {
        closeChart();
    }
    oldPoint = QPoint();
}

void Chart::mouseDoubleClickEvent(QMouseEvent *event) {
    if(event->y() > chartHeight()) {
        // ignore - close only when double tap on chart
        //qDebug() << "x: " << event->x();
    } else {
        closeChart();
    }
}

void Chart::closeChart() {
    //timer.stop();
    hide();
    mainWindow->setOrientation(MainWindow::ScreenOrientationLockPortrait);
    mainWindow->showChart();
    //mainWindow->showExpanded();

    QFile filew("goldapp.txt");
    filew.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&filew);
    out << style->at(Data::cstyle%style->size())->first
        << "/" << metal->at(Data::cmetal%metal->size())->first
        << "/" << currency->at(Data::ccurrency%currency->size())->first
        << "/" << dateRange->at(Data::cdateRange%dateRange->size())->first << "/Full";
    filew.close();

}

void Chart::updatePrices() {
    Data::updateChartData();
}

int Chart::getLastPrice() {
    return Data::getLastChartPrice();
}
