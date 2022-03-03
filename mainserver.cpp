#include "mainserver.h"
#include "Server/udpserver.h"

MainServer::MainServer(QObject *parent)
    : QObject{parent}
{

}

MainServer::~MainServer()
{
    if (nullptr != mUdpServer)
    {
        mUdpServer->stop();
        mUdpServer->deleteLater();
    }

    mWorkThread.terminate();
}

void MainServer::start()
{
    if (nullptr == mUdpServer)
    {
        mUdpServer = new UdpServer;
        connect(&mWorkThread, &QThread::started, mUdpServer, &UdpServer::slot_start_listen, Qt::QueuedConnection);
        mUdpServer->moveToThread(&mWorkThread);
        mWorkThread.start();
    }
}
