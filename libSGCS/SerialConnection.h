#ifndef SERIALCONNECTION_H
#define SERIALCONNECTION_H

#include "Connection.h"
#include <QObject>

class SerialConnection : public Connection
{
    Q_OBJECT
public:
    explicit SerialConnection(QObject *parent = nullptr);

protected slots:
    virtual void onTransmit(const QByteArray &data) override final;
signals:
};

#endif // SERIALCONNECTION_H
