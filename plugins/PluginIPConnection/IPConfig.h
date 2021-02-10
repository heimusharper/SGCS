#ifndef SERIALCONFIG_H
#define SERIALCONFIG_H
#include <config/RunConfiguration.h>

class IPConfig : public ConfigInterface
{
#if defined(SerialConfig_def)
#error Redefenition IPConfig_def
#else
#define IPConfig_def
#endif
public:
    IPConfig(ConfigInterface *parent);
    virtual ~IPConfig() = default;
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

    std::string hostName() const;
    void setHostName(const std::string &hostName);

    uint16_t port() const;
    void setPort(const uint16_t &porti);

private:
    std::string m_hostName;
    uint16_t m_port;
};

#endif // SERIALCONFIG_H
