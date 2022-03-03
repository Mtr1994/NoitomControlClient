#ifndef UDPSERVER_H
#define UDPSERVER_H

#include <QObject>
#include <QUdpSocket>
#include <Windows.h>
#include <QMap>

class UdpServer : public QObject
{
    Q_OBJECT
public:
    explicit UdpServer(QObject *parent = nullptr);

    void stop();

public slots:
    void slot_start_listen();

signals:
    void sgl_get_local_address_finish(const QString &address);

private:
    QString findBroadcastAddress();

    void parsePackage(const QByteArray &array);

    QByteArray createPackage( uint16_t cmd, const QByteArray &data);

    void sendPackage(const QByteArray &data);

    // 命令消息处理
    void start_task_3001_in_thread(const QByteArray &array);
    void start_task_3003_in_thread();
    void start_task_3005_in_thread();
    void start_task_3007_in_thread(const QByteArray &array);
    void start_task_3009_in_thread(const QByteArray &array);
    void start_task_3011_in_thread();
    void start_task_3013_in_thread(const QByteArray &array);
    void start_task_3015_in_thread();

private slots:
    void slot_command_socket_error_occurred(QAbstractSocket::SocketError error);

    void slot_recv_command_data();

private:
    QUdpSocket *mSocketCommand = nullptr;

    QByteArray mByteArray;

    QString mBroadcastAddress;

    QString mServerAddress = "192.168.1.111";
    uint16_t mServerPort = 13579;

    QMap<QString, PROCESS_INFORMATION> mMapProgram;
};

#endif // UDPSERVER_H
