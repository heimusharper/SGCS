#ifndef PLUGINLOADER_H
#define PLUGINLOADER_H

#include "PluginInterface.h"
#include <QAbstractListModel>
#include <QDir>
#include <QPluginLoader>
#include <qqml.h>

class PluginItem
{
public:
    QString _name;
    bool _load;
};

class PluginLoader : public QObject
{
    Q_OBJECT
public:
    static PluginLoader &instance();

    void load(const QDir &dir);
    QList<PluginItem *> list() const;

signals:
    void loaded();

private:
    QList<PluginItem *> m_list;
    PluginLoader();
};

class PluginLoaderListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_DISABLE_COPY(PluginLoaderListModel)
public:
    enum MyRoles
    {
        NameRole = Qt::UserRole + 1,
        IsLoadedRole
    };

    static QObject *singletonProvider(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
    {
        Q_UNUSED(qmlEngine)
        Q_UNUSED(jsEngine);
        return PluginLoaderListModel::instance();
    }
    static PluginLoaderListModel *instance();
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

private:
    explicit PluginLoaderListModel(QObject *parent = nullptr);
    static PluginLoaderListModel *m_instance;
};

#endif // PLUGINLOADER_H
