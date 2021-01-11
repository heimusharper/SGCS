#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QThread>

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection();

public slots:

    virtual void run() final;

protected slots:

    virtual void inittializeObjects() = 0;
    virtual void onReceive(const QByteArray &data) final;
    virtual void onTransmit(const QByteArray &data) = 0;
signals:
};

class ConnectionThread : public QObject
{
    Q_OBJECT
public:
    template <typename T>
    ConnectionThread(QObject *parent = nullptr) : QObject(parent)
    {
        _connection = new T;
        _thread     = new QThread();
        connect(_thread, &QThread::started, _connection, &T::run);
    }

private:
    Connection *_connection = nullptr;
    QThread *_thread        = nullptr;
};

#endif // CONNECTION_H
