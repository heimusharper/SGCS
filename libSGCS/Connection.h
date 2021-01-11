#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(QObject *parent = nullptr);

protected slots:
    virtual void onReceive(const QByteArray &data) final;
    virtual void onTransmit(const QByteArray &data) = 0;
signals:
};

#endif // CONNECTION_H
