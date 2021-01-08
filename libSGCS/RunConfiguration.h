/*
 * This file is part of the SGCS distribution (https://github.com/heimusharper/SGCS).
 * Copyright (c) 2020 Andrey Stepanov.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, version 3.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef RUNCONFIGURATION_H
#define RUNCONFIGURATION_H
#include <QDebug>
#include <QString>
#include <fstream>
#include <yaml-cpp/yaml.h>

class ConfigNode
{
};
/*!
 * \brief The ConfigInterface class
 * \warning Do not create objects manually,
 * use RunConfiguration::instance().get<YourImplementationName>()
 *
 * example implementation is ApplicationConfiguration
 */
class ConfigInterface : public ConfigNode
{
public:
    /*!
     * \brief ConfigInterface
     * \param root
     */
    ConfigInterface(ConfigInterface *root);
    virtual ~ConfigInterface() = default;

    template <class T>
    /*!
     * \brief get create or get ConfigInterface implementation
     * \return
     */
    T *get()
    {
        T *symbol = nullptr;
        // search existing
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
        // create new
        T *object           = new T(this);
        ConfigInterface *ci = dynamic_cast<ConfigInterface *>(object);
        if (ci)
        {
            //  read configuration from file
            YAML::Node node = nodeAt(ci);
            ci->fromNode(node);
        }
        _nodes.append(object);
        return object;
    }
    /*!
     * \brief name Implementation name
     * \return
     */
    virtual QString name() const = 0;
    /*!
     * \brief toNode write to file new configuration
     * \param file
     * \return
     */
    virtual YAML::Node toNode(const YAML::Node &file) const = 0;
    /*!
     * \brief fromNode get configuration from node
     * \param node
     */
    virtual void fromNode(const YAML::Node &node) = 0;
    /*!
     * \brief parent node parent
     * \return
     */
    ConfigInterface *parent() const;
    /*!
     * \brief clear delete all nodes
     * \warning unsafe, do not use
     */
    void clear();

protected:
    /*!
     * \brief save save to file
     */
    virtual void save();

private:
    /*!
     * \brief nodeAt get node at
     * \param node
     * \return
     */
    YAML::Node nodeAt(ConfigInterface *node);

protected:
    ConfigInterface *_parent = nullptr;
    QVector<ConfigInterface *> _nodes;
};
/*!
 * \brief The ApplicationConfiguration class
 */
class ApplicationConfiguration : public ConfigInterface
{
#if defined(ApplicationConfiguration_def)
#error Redefenition ApplicationConfiguration_def
#else
#define ApplicationConfiguration_def
#endif
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
/*!
 * \brief The RunConfiguration class
 */
class RunConfiguration : public ConfigInterface
{
public:
    virtual ~RunConfiguration();
    static RunConfiguration &instance();

    bool create(const QString &filename);

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
