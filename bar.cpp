#include "bar.h"

Bar::Bar(double low, double high, double close)
    : low(low), high(high), close(close)
{ nodata = false; }
Bar::Bar() { nodata = true; }

double Bar::getLow() { return low; }
double Bar::getHigh() { return high; }
double Bar::getClose() { return close; }
bool Bar::hasData() { return !nodata; }
