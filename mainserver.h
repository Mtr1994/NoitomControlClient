#ifndef MAINSERVER_H
#define MAINSERVER_H

#include <QObject>
#include <QThread>

class UdpServer;
class MainServer : public QObject
{
    Q_OBJECT
public:
    explicit MainServer(QObject *parent = nullptr);
    ~MainServer();

    void start();

signals:

private:
    QThread mWorkThread;

    UdpServer *mUdpServer = nullptr;
};

#endif // MAINSERVER_H
