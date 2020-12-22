#include "PluginLoader.h"

PluginLoader &PluginLoader::instance()
{
    static PluginLoader pl;
    return pl;
}

void PluginLoader::load(const QDir &dir, MainWindow *window)
{
    for (auto pluginFilename : dir.entryList())
    {
        QString fullName = dir.absoluteFilePath(pluginFilename);
        if (QLibrary::isLibrary(fullName))
        {
            QPluginLoader loader(fullName);
            if (auto instance = loader.instance())
            {
                auto plugin = qobject_cast<PluginInterface *>(instance);
                if (plugin)
                {
                    PluginItem *item = new PluginItem;
                    item->_name      = plugin->name();
                    plugin->setWindow(window);
                    if (plugin->isLoad())
                    {
                        plugin->create();
                        item->_load = true;
                    }
                    else
                    {
                        item->_load = false;
                        loader.unload();
                    }
                    m_list.append(item);
                    qDebug() << "Done load lugin" << fullName;
                }
                else
                {
                    qDebug() << "qobject_cast<> returned nullptr";
                }
            }
            else
            {
                qDebug() << loader.errorString();
            }
        }
        else
        {
            qWarning() << "Failed: " << fullName << " is not a library/plugin";
        }
    }
}

QList<PluginItem *> PluginLoader::list() const
{
    return m_list;
}

PluginLoader::PluginLoader()
{
}

//
//
PluginLoaderListModel *PluginLoaderListModel::m_instance = nullptr;

PluginLoaderListModel::PluginLoaderListModel(QObject *parent) : QAbstractListModel(parent)
{
}

void PluginLoaderListModel::create()
{
    qmlRegisterSingletonType<PluginLoaderListModel>("Ru.SGCS", 1, 0, "PluginLoaderListModel", PluginLoaderListModel::singletonProvider);
}

PluginLoaderListModel *PluginLoaderListModel::instance()
{
    if (m_instance == nullptr)
        m_instance = new PluginLoaderListModel();
    return m_instance;
}

QHash<int, QByteArray> PluginLoaderListModel::roleNames() const
{
    return {{NameRole, "name"}, {IsLoadedRole, "visible"}};
}

int PluginLoaderListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;
    return PluginLoader::instance().list().size();
}

bool PluginLoaderListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!hasIndex(index.row(), index.column(), index.parent()) || !value.isValid())
        return false;

    PluginItem *item = PluginLoader::instance().list().at(index.row());
    if (role == IsLoadedRole)
        item->_load = value.toBool();
    else
        return false;

    emit dataChanged(index, index, {role});

    return true;
}

QVariant PluginLoaderListModel::data(const QModelIndex &index, int role) const
{
    if (!hasIndex(index.row(), index.column(), index.parent()))
        return {};
    const PluginItem *item = PluginLoader::instance().list().at(index.row());
    if (role == NameRole)
        return item->_name;
    if (role == IsLoadedRole)
        return item->_load;
    return {};
}
