#ifndef WINDOWCONF_H
#define WINDOWCONF_H

#include "RunConfiguration.h"
#include <QObject>

class WindowConf : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString version READ version WRITE setVersion NOTIFY versionChanged)

public:
    explicit WindowConf(QObject *parent = nullptr);

    QString version() const;

public slots:
    void setVersion(QString version);

private:
    QString m_version;

signals:
    void versionChanged(QString version);
};

#endif // WINDOWCONF_H
