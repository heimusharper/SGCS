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
#ifndef SERIALCONNECTIONSELECTORLISTMODEL_H
#define SERIALCONNECTIONSELECTORLISTMODEL_H
#include <QAbstractListModel>
#include <QDebug>
#include <QMutex>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <qqml.h>
/*!
 * \brief The SerialConnectionFabric class
 * Configure serial connection
 * \todo delete object
 */
class SerialConnectionSelectorListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_DISABLE_COPY(SerialConnectionSelectorListModel)
    Q_PROPERTY(int current READ current WRITE setCurrent NOTIFY currentChanged)
    Q_PROPERTY(bool check READ check WRITE setCheck NOTIFY checkChanged)
public:
    enum MyRoles
    {
        NameRole = Qt::UserRole + 1,
        IsBusyRole
    };
    /*!
     * \brief SerialConnectionFabric
     * \param parent
     */
    explicit SerialConnectionSelectorListModel(QObject *parent = nullptr);
    /*!
     * \brief roleNames roles
     * \return
     * name - serial port name
     * busy - is busy
     */
    QHash<int, QByteArray> roleNames() const override;
    /*!
     * \brief rowCount
     * \param parent
     * \return
     */
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    /*!
     * \brief setData
     * \param index
     * \param value
     * \param role
     * \return
     */
    bool setData(const QModelIndex &index, const QVariant &value, int role) override;
    /*!
     * \brief data
     * \param index
     * \param role
     * \return
     */
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    /*!
     * \brief info current ports info
     * \return
     */
    QList<QSerialPortInfo> info() const;
    /*!
     * \brief current current port index
     * \return
     */
    int current() const;
    /*!
     * \brief check is check on timer
     * \return
     */
    bool check() const;

public slots:
    /*!
     * \brief setCurrent set current serial port index
     * \param current
     * from info()
     */
    void setCurrent(int current);
    /*!
     * \brief setCheck set check on timer
     * \param check true - start checks, false - stop
     */
    void setCheck(bool check);

signals:
    /*!
     * \brief currentChanged current serial port index changed
     * \param current
     */
    void currentChanged(int current);
    /*!
     * \brief checkChanged on timer check changed
     * \param check
     */
    void checkChanged(bool check);

private slots:
    /*!
     * \brief updatePorts get new serial ports list
     */
    void updatePorts();

private:
    /*!
     * \brief m_info current serial ports list
     */
    QList<QSerialPortInfo> m_info;
    /*!
     * \brief m_current current serial port index
     */
    int m_current = -1;
    /*!
     * \brief m_check check on timer
     */
    bool m_check = false;
    /*!
     * \brief m_timer check timer
     */
    QTimer *m_timer = nullptr;
};

#endif // SERIALCONNECTIONSELECTORLISTMODEL_H
