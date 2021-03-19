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
    virtual std::string name() const override final;
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

    std::string portName() const;
    void setPortName(const std::string &portName);

    uint32_t baudRate() const;
    void setBaudRate(const uint32_t &baudRate);

    bool autoName() const;
    void setAutoName(bool autoName);

private:
    std::string m_portName;
    uint32_t m_baudRate;
    bool m_autoName;
};

#endif // SERIALCONFIG_H
