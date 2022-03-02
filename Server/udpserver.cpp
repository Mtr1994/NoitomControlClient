#include "udpserver.h"

#include <QHostAddress>
#include <QByteArray>

// test
#include <QDebug>

UdpServer::UdpServer(QObject *parent)
    : QObject{parent}
{

}

void UdpServer::stop()
{
    if (nullptr == mSocketCommand) return;
    mSocketCommand->close();
}

void UdpServer::slot_start_listen()
{
    if (nullptr != mSocketCommand) return;

    // 初始化命令通道套接字
    mSocketCommand = new  QUdpSocket(this);
    connect(mSocketCommand, &QUdpSocket::errorOccurred, this, &UdpServer::slot_command_socket_error_occurred);
    connect(mSocketCommand, &QUdpSocket::connected, this, &UdpServer::slot_socket_connected);
    connect(mSocketCommand, &QUdpSocket::disconnected, this, &UdpServer::slot_socket_disconnected);
    connect(mSocketCommand, &QUdpSocket::readyRead, this, &UdpServer::slot_recv_command_data);

    // 根据读取配置文件
    mSocketCommand->bind(QHostAddress("0.0.0.0"), 60101);
}

void UdpServer::slot_command_socket_error_occurred(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
    // 需要日志记录
}

void UdpServer::slot_socket_connected()
{
    qDebug() << "connect";
}

void UdpServer::slot_socket_disconnected()
{
    disconnect(mSocketCommand, &QUdpSocket::errorOccurred, this, &UdpServer::slot_command_socket_error_occurred);
    disconnect(mSocketCommand, &QUdpSocket::connected, this, &UdpServer::slot_socket_connected);
    disconnect(mSocketCommand, &QUdpSocket::disconnected, this, &UdpServer::slot_socket_disconnected);

    mSocketCommand->deleteLater();
}

void UdpServer::slot_recv_command_data()
{
    QByteArray array = mSocketCommand->readAll();

    qDebug() << array;
}
