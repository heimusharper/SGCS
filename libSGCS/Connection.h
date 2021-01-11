#ifndef CONNECTION_H
#define CONNECTION_H

#include <QObject>
#include <QThread>

class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection();
    virtual ~Connection();

public slots:

    virtual void run() final;

protected slots:

    virtual void inittializeObjects() = 0;
    virtual void onReceive(const QByteArray &data) final;
    virtual void onTransmit(const QByteArray &data) = 0;
};

class ConnectionThread : public QObject
{
    Q_OBJECT
public:
    ConnectionThread(QObject *parent = nullptr);
    ~ConnectionThread();

    template <typename T>
    T *create()
    {
        _connection = new T;
        _thread     = new QThread();
        connect(_thread, &QThread::started, _connection, &T::run);
        connect(_thread, &QThread::finished, _connection, &QObject::deleteLater);

        _connection->moveToThread(_thread);
        _thread->start();
        return dynamic_cast<T *>(_connection);
    }

private:
    Connection *_connection = nullptr;
    QThread *_thread        = nullptr;
};

#endif // CONNECTION_H
