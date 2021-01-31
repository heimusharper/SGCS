#ifndef MAVLINKPROTOCOL_H
#define MAVLINKPROTOCOL_H

#include <QObject>
#include <UavProtocol.h>

class MavlinkProtocol : public uav::UavProtocol
{
    Q_OBJECT
public:
    explicit MavlinkProtocol(QObject *parent = nullptr);

signals:
};

#endif // MAVLINKPROTOCOL_H
