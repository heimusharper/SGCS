#ifndef SERIALCONNECTIONFABRIC_H
#define SERIALCONNECTIONFABRIC_H
#include <QAbstractListModel>
#include <QDebug>
#include <QMutex>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <qqml.h>

class SerialConnectionFabric : public QAbstractListModel
{
    Q_OBJECT
    Q_DISABLE_COPY(SerialConnectionFabric)
    Q_PROPERTY(int current READ current WRITE setCurrent NOTIFY currentChanged)
    Q_PROPERTY(bool check READ check WRITE setCheck NOTIFY checkChanged)
public:
    enum MyRoles
    {
        NameRole = Qt::UserRole + 1,
        IsBusyRole
    };

    static void create();
    static QObject *singletonProvider(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
    {
        Q_UNUSED(qmlEngine)
        Q_UNUSED(jsEngine);
        return SerialConnectionFabric::instance();
    }
    static SerialConnectionFabric *instance();
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QList<QSerialPortInfo> info() const;

    int current() const;

    bool check() const;

public slots:
    void setCurrent(int current);

    void setCheck(bool check);

signals:
    void currentChanged(int current);

    void checkChanged(bool check);

private slots:

    void updatePorts();

private:
    explicit SerialConnectionFabric(QObject *parent = nullptr);
    static SerialConnectionFabric *m_instance;
    QList<QSerialPortInfo> m_info;
    int m_current = -1;
    bool m_check;
    QTimer *m_timer = nullptr;
};

#endif // SERIALCONNECTIONFABRIC_H
