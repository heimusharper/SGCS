#ifndef UAVFABRIC_H
#define UAVFABRIC_H
#include "UavObject.h"
#include <QDebug>
#include <QJSValue>
#include <QObject>

class UavFabric : public QObject
{
    Q_OBJECT
public:
    explicit UavFabric(QObject *parent = nullptr);

    Q_INVOKABLE static void createObject(QJSValue *obj);
signals:
};

#endif // UAVFABRIC_H
