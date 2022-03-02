#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QObject>
#include <QUdpSocket>

class UdpServer : public QObject
{
    Q_OBJECT
public:
    explicit UdpServer(QObject *parent = nullptr);

    void stop();

public slots:
    void slot_start_listen();

signals:

private slots:
    void slot_command_socket_error_occurred(QAbstractSocket::SocketError error);

    void slot_socket_connected();

    void slot_socket_disconnected();

    void slot_recv_command_data();

private:
    QUdpSocket *mSocketCommand = nullptr;
};

#endif // UDPSERVER_H
