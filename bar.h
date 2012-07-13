#ifndef BAR_H
#define BAR_H

class Bar
{
public:
    Bar(double low, double high, double close);
    Bar();
    double getLow();
    double getHigh();
    double getClose();
    bool hasData();

private:
    double low, high, close;
    bool nodata;
};

#endif // BAR_H
