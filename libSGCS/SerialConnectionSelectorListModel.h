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
#include "ConnectionThread.h"
#include "SerialConnection.h"
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
    Q_PROPERTY(int baudrate READ baudrate WRITE setBaudrate NOTIFY baudrateChanged)
    Q_PROPERTY(bool check READ check WRITE setCheck NOTIFY checkChanged)
    Q_PROPERTY(bool connected READ connected WRITE setConnected NOTIFY connectedChanged)
    Q_PROPERTY(QString serialError READ serialError NOTIFY serialErrorChanged)

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
    ~SerialConnectionSelectorListModel();
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
    /*!
     * \brief baudrate serial baudrate
     * \return
     */
    int baudrate() const;
    /*!
     * \brief connected serial port is connected
     * \return
     */
    bool connected() const;
    /*!
     * \brief serialError last serial error
     * \return error string
     */
    QString serialError() const;

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
    /*!
     * \brief setBaudrate serial baudrate
     * \param baudrate
     */
    void setBaudrate(int baudrate);
    /*!
     * \brief setConnected serial port is connected
     * \param connected
     */
    void setConnected(bool connected);

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
    /*!
     * \brief baudrateChanged baudrate is changed
     * \param baudrate
     */
    void baudrateChanged(int baudrate);
    /*!
     * \brief connectedChanged serial is connected changed
     * \param connected
     */
    void connectedChanged(bool connected);
    /*!
     * \brief tryConnect try connect to port
     * \param portName port name
     * \param baudrate baudrate
     */
    void tryConnect(const QString &portName, int baudrate);
    /*!
     * \brief tryDisconnect disconnect from port
     */
    void tryDisconnect();
    /*!
     * \brief serialErrorChanged last serial error changed
     * \param serialError last serial error
     */
    void serialErrorChanged(QString serialError);

private slots:
    /*!
     * \brief updatePorts get new serial ports list
     */
    void updatePorts();
    /*!
     * \brief onSerialConnectedTo serial port connected
     * \param portName port name
     * \param baudRate baudrate
     */
    void onSerialConnectedTo(const QString &portName, int baudRate);
    /*!
     * \brief onSerialDisconnected serial disconnected
     * \param error last  error, empty if not
     */
    void onSerialDisconnected(const QString &error);

private:
    /*!
     * \brief timer get timer
     * \return
     */
    QTimer *timer();
    /*!
     * \brief m_info current serial ports list
     */
    QList<QSerialPortInfo> m_info;
    /*!
     * \brief m_current current serial port index
     */
    int m_current;
    /*!
     * \brief m_check check on timer
     */
    bool m_check;
    /*!
     * \brief m_baudrate serial baudrate
     */
    int m_baudrate;
    /*!
     * \brief m_connected setial port is connected
     */
    bool m_connected;
    /*!
     * \brief m_timer check timer
     * \warning use timer() function
     */
    QTimer *m_timer = nullptr;
    /*!
     * \brief _connection serial connection object
     */
    connection::ConnectionThread *_connection = nullptr;
    /*!
     * \brief m_serialError last serial error
     */
    QString m_serialError;
};

#endif // SERIALCONNECTIONSELECTORLISTMODEL_H
