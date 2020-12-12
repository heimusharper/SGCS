#ifndef RUNCONFIGURATION_H
#define RUNCONFIGURATION_H
#include <QDebug>
#include <QString>
#include <fstream>
#include <yaml-cpp/yaml.h>

class ConfigNode
{
};

class ConfigInterface : public ConfigNode
{
public:
    ConfigInterface(ConfigInterface *root);
    virtual ~ConfigInterface() = default;

    template <class T>
    T *get()
    {
        T *symbol = nullptr;
        for (ConfigInterface *sym : _nodes)
        {
            symbol = dynamic_cast<T *>(sym);
            if (symbol)
                return symbol;
            else
            {
                symbol = sym->get<T>();
                if (symbol)
                    return symbol;
            }
        }
        // not exists
        T *object           = new T(this);
        ConfigInterface *ci = dynamic_cast<ConfigInterface *>(object);
        if (ci)
        {
            YAML::Node node = nodeAt(ci);
            ci->fromNode(node);
        }
        _nodes.append(object);
        return object;
    }

    virtual QString name() const = 0;

    virtual YAML::Node toNode(const YAML::Node &file) const = 0;
    virtual void fromNode(const YAML::Node &node)           = 0;

    ConfigInterface *parent() const;

    void clear();

protected:
    virtual void save();

private:
    YAML::Node nodeAt(ConfigInterface *node);

protected:
    ConfigInterface *_parent = nullptr;
    QVector<ConfigInterface *> _nodes;
};

class ApplicationConfiguration : public ConfigInterface
{
public:
    ApplicationConfiguration(ConfigInterface *parent);
    virtual ~ApplicationConfiguration() = default;

    virtual QString name() const override final;

    virtual YAML::Node toNode(const YAML::Node &file) const override final;
    virtual void fromNode(const YAML::Node &node) override final;

    QString profile() const;
    void setProfile(const QString &profile);

private:
    QString m_profile;
};

class RunConfiguration : public ConfigInterface
{
public:
    virtual ~RunConfiguration();
    static RunConfiguration &instance();

    void create(const QString &filename);

    virtual QString name() const override final;

    virtual YAML::Node toNode(const YAML::Node &file) const override final;
    virtual void fromNode(const YAML::Node &node) override final;

    YAML::Node getFromFile(const QStringList &tree);

    void forceSave();

protected:
    virtual void save() override final;

private:
    YAML::Node _yaml;
    QString _filename;
    RunConfiguration(ConfigInterface *root = nullptr);
};

#endif // RUNCONFIGURATION_H
