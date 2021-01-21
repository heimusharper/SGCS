#ifndef UAVOBJECT_H
#define UAVOBJECT_H

#include <QObject>

class UavObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool connected READ connected WRITE setConnected NOTIFY connectedChanged)
    Q_PROPERTY(int id READ id WRITE setId NOTIFY idChanged)

public:
    explicit UavObject(QObject *parent = nullptr);

    bool connected() const;

    int id() const;

public slots:
    void setConnected(bool connected);

    void setId(int id);

signals:
    void connectedChanged(bool connected);
    void idChanged(int id);

private:
    bool m_connected = false;
    int m_id         = -1;
};

#endif // UAVOBJECT_H
