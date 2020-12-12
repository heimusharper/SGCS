#ifndef UAV_H
#define UAV_H
#include "plugins/PluginInterface.h"

class PluginSerialConnectionSymbol : public ConfigInterface
{
#if defined(PluginSerialConnectionSymbol_def)
#error Redefenition PluginSerialConnectionSymbol_def
#else
#define PluginSerialConnectionSymbol_def
#endif
public:
    PluginSerialConnectionSymbol(ConfigInterface *parent);
    virtual ~PluginSerialConnectionSymbol() = default;
    virtual QString name() const override final;
    virtual YAML::Node toNode(const YAML::Node &file) const override final;
    virtual void fromNode(const YAML::Node &node) override final;

    bool load() const;
    void setLoad(bool load);

private:
    bool m_load = true;
};

class PluginSerialConnection : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PluginInterface_id)
    Q_INTERFACES(PluginInterface)

public:
    PluginSerialConnection();
    virtual ~PluginSerialConnection();

    virtual bool load() const;
    virtual QString name() const;
};

#endif // SGCS_H
