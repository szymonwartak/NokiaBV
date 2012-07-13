#ifndef ORDER_H
#define ORDER_H

class Order {


public:
    Order(long id, QString buysell, QString metalLoc, QString currency,
          int price, double quantity, QString percentFulfilled) {
        m_id=id; m_price=price; m_quantity=quantity; m_metalLoc=metalLoc; m_buysell=buysell;
        m_currency=currency; m_percentFulfilled=percentFulfilled; }
    long id() const { return m_id; }
    int price() const { return m_price; }
    double quantity() const { return m_quantity; }
    QString currency() const { return m_currency; }
    QString metalLoc() const { return m_metalLoc; }
    QString buysell() const { return m_buysell; }
    QString percentFulfilled() const { return m_percentFulfilled; }

private:
    long m_id;
    int m_price;
    double m_quantity;
    QString m_metalLoc, m_buysell, m_currency, m_percentFulfilled;
};

#endif // ORDER_H
