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

/*!
 * \brief The ConfigInterface class
 * \warning Do not create objects manually,
 * use RunConfiguration::instance().get<YourImplementationName>()
 *
 * example implementation is ApplicationConfiguration
 */
class ConfigInterface
{
public:
    /*!
     * \brief ConfigInterface
     * \param root
     */
    ConfigInterface(ConfigInterface *root);
    virtual ~ConfigInterface() = default;

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

protected:
    /*!
     * \brief save save to file
     */
    virtual void save();

protected:
    ConfigInterface *_parent = nullptr;
};
/*!
 * \brief The ApplicationConfiguration class
 * Main application configuration
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
    /*!
     * \brief name ApplicationConfiguration
     * \return ApplicationConfiguration
     */
    virtual QString name() const override final;
    /*!
     * \brief toNode to yaml node
     * \param file root
     * \return
     */
    virtual YAML::Node toNode(const YAML::Node &file) const override final;
    /*!
     * \brief fromNode from yaml node
     * \param node node
     */
    virtual void fromNode(const YAML::Node &node) override final;
    /*!
     * \brief profile get application run profile name
     * \return profile name
     */
    QString profile() const;
    /*!
     * \brief setProfile change application profile name
     * \param profile profile name
     */
    void setProfile(const QString &profile);
    /*!
     * \brief versionMajor version major
     * \return version major
     * \warning do not save to false
     */
    int versionMajor() const;
    /*!
     * \brief versionMinor version major
     * \return version major
     * \warning do not save to false
     */
    int versionMinor() const;
    /*!
     * \brief versionPath version major
     * \return version major
     * \warning do not save to false
     */
    int versionPath() const;
    /*!
     * \brief versionHash version major
     * \return version major
     * \warning do not save to false
     */
    QString versionHash() const;
    /*!
     * \brief startDatasource autostart datasource plugin name
     * \return
     */
    QString startDatasource() const;
    /*!
     * \brief setStartDatasource autostart datasource plugin name
     * \param startDatasource
     */
    void setStartDatasource(const QString &startDatasource);

private:
    QString m_profile;
    int m_versionMajor    = 0;
    int m_versionMinor    = 0;
    int m_versionPath     = 0;
    QString m_versionHash = 0;
    QString m_startDatasource;
};
/*!
 * \brief The RunConfiguration class
 * Main configuration object
 * 1. try open file ```RunConfiguration::instance().create("default.yaml")```
 * 2. if failed, create new
 *   ```RunConfiguration::instance().get<ApplicationConfiguration>()->setProfile("default");
 *      RunConfiguration::instance().forceSave();```
 * 3. access to nodes RunConfiguration::instance().get<Node defenition object name>()
 * RunConfiguration automatically parse yaml for nodes in get<> function
 */
class RunConfiguration : public ConfigInterface
{
public:
    virtual ~RunConfiguration();
    /*!
     * \brief instance RunConfiguration singleton instance
     * \return
     */
    static RunConfiguration &instance();

    template <class T>
    /*!
     * \brief get create or get ConfigInterface implementation
     * \return
     */
    T *get()
    {
        T *symbol = get_if_exists<T>();
        if (symbol)
            return symbol;
        // not exists
        // create new
        T *object           = new T(this);
        ConfigInterface *ci = dynamic_cast<ConfigInterface *>(object);
        if (ci)
        {
            //  read configuration from file
            YAML::Node node = this->getFromFile(ci->name());
            if (node)
                ci->fromNode(node);
        }
        _nodes.append(object);
        return object;
    }
    template <class T>
    T *get_if_exists()
    {
        for (ConfigInterface *sym : _nodes)
        {
            T *symbol = dynamic_cast<T *>(sym);
            if (symbol)
                return symbol;
        }
        return nullptr;
    }
    /*!
     * \brief create create root from file
     * \param filename filename
     * \return true if file exist and yaml is valid
     */
    bool create(const QString &filename);
    /*!
     * \brief name ConfigInterface implementation name
     * \return name
     */
    virtual QString name() const override final;
    /*!
     * \brief toNode create yaml node
     * \param file root
     * \return
     */
    virtual YAML::Node toNode(const YAML::Node &file) const override final;
    /*!
     * \brief fromNode parse yaml node
     * \param node yaml node
     */
    virtual void fromNode(const YAML::Node &node) override final;
    /*!
     * \brief getFromFile get node from file
     * \param tree parent nodes
     * \return node or empty
     */
    YAML::Node getFromFile(const QString &link);
    /*!
     * \brief forceSave save to yaml file
     */
    void forceSave();
    /*!
     * \brief clear delete all nodes
     * \warning unsafe, do not use
     */
    void clear();

protected:
    /*!
     * \brief save save node
     */
    virtual void save() override final;

private:
    QVector<ConfigInterface *> _nodes;
    /*!
     * \brief _yaml root
     */
    YAML::Node _yaml;
    /*!
     * \brief _filename save filename
     */
    QString _filename;
    /*!
     * \brief RunConfiguration
     * \param root
     */
    RunConfiguration(ConfigInterface *root = nullptr);
};

#endif // RUNCONFIGURATION_H
