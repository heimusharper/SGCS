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
#ifndef SERIALCONNECTIONFABRIC_H
#define SERIALCONNECTIONFABRIC_H
#include <QAbstractListModel>
#include <QDebug>
#include <QMutex>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <qqml.h>

class SerialConnectionFabric : public QAbstractListModel
{
    Q_OBJECT
    Q_DISABLE_COPY(SerialConnectionFabric)
    Q_PROPERTY(int current READ current WRITE setCurrent NOTIFY currentChanged)
    Q_PROPERTY(bool check READ check WRITE setCheck NOTIFY checkChanged)
public:
    enum MyRoles
    {
        NameRole = Qt::UserRole + 1,
        IsBusyRole
    };

    static void create();
    static QObject *singletonProvider(QQmlEngine *qmlEngine, QJSEngine *jsEngine)
    {
        Q_UNUSED(qmlEngine)
        Q_UNUSED(jsEngine);
        return SerialConnectionFabric::instance();
    }
    static SerialConnectionFabric *instance();
    QHash<int, QByteArray> roleNames() const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QList<QSerialPortInfo> info() const;

    int current() const;

    bool check() const;

public slots:
    void setCurrent(int current);

    void setCheck(bool check);

signals:
    void currentChanged(int current);

    void checkChanged(bool check);

private slots:

    void updatePorts();

private:
    explicit SerialConnectionFabric(QObject *parent = nullptr);
    static SerialConnectionFabric *m_instance;
    QList<QSerialPortInfo> m_info;
    int m_current = -1;
    bool m_check;
    QTimer *m_timer = nullptr;
};

#endif // SERIALCONNECTIONFABRIC_H
