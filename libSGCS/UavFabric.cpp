#include "UavFabric.h"

UavFabric::UavFabric(QObject *parent) : QObject(parent)
{
}

void UavFabric::createObject(QJSValue *obj)
{
    qDebug() << "Create object" << obj->toString();
}
