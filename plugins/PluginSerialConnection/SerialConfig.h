#ifndef SERIALCONFIG_H
#define SERIALCONFIG_H
#include <config/RunConfiguration.h>

class SerialConfig : public ConfigInterface
{
#if defined(SerialConfig_def)
#error Redefenition SerialConfig_def
#else
#define SerialConfig_def
#endif
public:
    SerialConfig(ConfigInterface *parent);
    virtual ~SerialConfig() = default;
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

    QString portName() const;
    void setPortName(const QString &portName);

    quint16 baudRate() const;
    void setBaudRate(const quint16 &baudRate);

private:
    QString m_portName;
    quint16 m_baudRate;
};

#endif // SERIALCONFIG_H
