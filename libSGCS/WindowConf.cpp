#include "WindowConf.h"

WindowConf::WindowConf(QObject *parent) : QObject(parent)
{
    int major    = RunConfiguration::instance().get<ApplicationConfiguration>()->versionMajor();
    int minor    = RunConfiguration::instance().get<ApplicationConfiguration>()->versionMinor();
    int path     = RunConfiguration::instance().get<ApplicationConfiguration>()->versionPath();
    QString hash = RunConfiguration::instance().get<ApplicationConfiguration>()->versionHash();
    m_version    = QString("%1.%2.%3-%4").arg(major).arg(minor).arg(path).arg(hash);
}

QString WindowConf::version() const
{
    return m_version;
}

void WindowConf::setVersion(QString version)
{
    if (m_version == version)
        return;

    m_version = version;
    emit versionChanged(m_version);
}
