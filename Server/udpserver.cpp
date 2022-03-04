#include "udpserver.h"
#include "Config/softconfig.h"
#include "Log/logger.h"
#include "Common/common.h"

#include <QHostAddress>
#include <QByteArray>
#include <QNetworkInterface>
#include <thread>
#include <chrono>
#include <QNetworkDatagram>
#include <QDir>

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
    disconnect(mSocketCommand, &QUdpSocket::errorOccurred, this, &UdpServer::slot_command_socket_error_occurred);
    mSocketCommand->deleteLater();

    LOG_DEBUG("停止监听端口");
}

void UdpServer::slot_start_listen()
{
    if (nullptr != mSocketCommand) return;

    // 初始化命令通道套接字
    mSocketCommand = new  QUdpSocket(this);
    connect(mSocketCommand, &QUdpSocket::errorOccurred, this, &UdpServer::slot_command_socket_error_occurred);
    connect(mSocketCommand, &QUdpSocket::readyRead, this, &UdpServer::slot_recv_command_data);

    uint32_t port = SoftConfig::getInstance()->getValue("Server", "port").toULong();
    if (port == 0)
    {
        LOG_DEBUG("配置文件异常，找不到默认监听端口");
        stop();
        return;
    }

    // 根据配置文件获取需要监听的网卡网关
    QString address = findBroadcastAddress();
    while (address.isEmpty())
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
        address = findBroadcastAddress();
    }

    LOG_DEBUG(QString("开始监听数据 地址： %1 端口： %2").arg(address, QString::number(port)).toStdString());
    // 默认监听所有网卡
    mSocketCommand->bind(QHostAddress(address), port);
}

QString UdpServer::findBroadcastAddress()
{
    LOG_DEBUG("开始检测网卡状态");
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    QString defaultGate = SoftConfig::getInstance()->getValue("Server", "gate");
    if (defaultGate.isEmpty())
    {
        LOG_DEBUG("配置文件异常，找不到指定网关");
        return defaultGate;
    }
    QString address;
    for (auto &addr : ipAddressesList)
    {
       if (addr != QHostAddress::LocalHost && addr.toIPv4Address())
       {
            if (defaultGate.split('.').sliced(0, 3) == addr.toString().split('.').sliced(0, 3))
            {
                address = addr.toString();
                break;
            }
       }
    }

    mBroadcastAddress = address;
    return address;
}

void UdpServer::parsePackage(const QByteArray &array)
{
    uint32_t cmd = 0;
    memcpy(&cmd, array.data(), 2);

    // 日志记录
    LOG_DEBUG(QString("接收数据包 %1").arg(array.toHex()).toStdString());

    switch (cmd) {
    case CMD_PASSIVE_REGISTRATE:
    {
        auto func = std::bind(&UdpServer::start_task_3001_in_thread, this, array.right(array.length() - 8));
        std::thread th(func);
        th.detach();
        break;
    }
    case CMD_START_PROGRAM:
    {
        auto func = std::bind(&UdpServer::start_task_3003_in_thread, this);
        std::thread th(func);
        th.detach();
        break;
    }
        break;
    case CMD_STOP_PROGRAM:
    {
        auto func = std::bind(&UdpServer::start_task_3005_in_thread, this);
        std::thread th(func);
        th.detach();
        break;
    }
    case CMD_KEYBOARD_PRESS:
    {
        auto func = std::bind(&UdpServer::start_task_3007_in_thread, this, array.right(array.length() - 8));
        std::thread th(func);
        th.detach();
        break;
    }
    case CMD_MOVE_MOUSE:
    {
        auto func = std::bind(&UdpServer::start_task_3009_in_thread, this, array.right(array.length() - 8));
        std::thread th(func);
        th.detach();
        break;
    }
    case CMD_CLICK_MOUSEM:
    {
        auto func = std::bind(&UdpServer::start_task_3011_in_thread, this);
        std::thread th(func);
        th.detach();
        break;
    }
    case CMD_SHUTDOWN_COMPUTER:
    {
        auto func = std::bind(&UdpServer::start_task_3013_in_thread, this, array.right(array.length() - 8));
        std::thread th(func);
        th.detach();
        break;
    }
    case CMD_HEARTBEAT_DETECTION:
    {
        auto func = std::bind(&UdpServer::start_task_3015_in_thread, this);
        std::thread th(func);
        th.detach();
        break;
    }
    default:
        break;
    }
}

QByteArray UdpServer::createPackage(uint16_t cmd, const QByteArray &data)
{
    QByteArray array;
    array.resize(8 + data.length());
    // 命令
    memcpy(array.data(), &cmd, 2);
    // 长度
    uint64_t size = 8 + data.length();
    memcpy(array.data() + 2, &size, 2);

    auto list = mBroadcastAddress.split('.');
    if (list.size() != 4)
    {
        mBroadcastAddress = "0.0.0.0";
        list = mBroadcastAddress.split('.');
    }
    // 地址
    uint8_t ip = list.at(0).toUInt();
    memcpy(array.data() + 4, &ip, 1);
    ip = list.at(1).toUInt();
    memcpy(array.data() + 5, &ip, 1);
    ip = list.at(2).toUInt();
    memcpy(array.data() + 6, &ip, 1);
    ip = list.at(3).toUInt();
    memcpy(array.data() + 7, &ip, 1);

    // 数据
    memcpy(array.data() + 8, data.data(), data.length());
    return array;
}

void UdpServer::sendPackage(const QByteArray &data)
{
    // 日志记录
    LOG_DEBUG(QString("发送数据包 %1").arg(data.toHex()).toStdString());
    mSocketCommand->writeDatagram(data, QHostAddress(mServerAddress), mServerPort);
}

void UdpServer::start_task_3001_in_thread(const QByteArray &array)
{
    mServerAddress = QString("%1.%2.%3.%4").arg(QString::number(array.at(0)),
                                                QString::number(array.at(1)),
                                                QString::number(array.at(2)),
                                                QString::number(array.at(3)));
    memcpy(&mServerPort, array.data() + 4, 2);

    if (mBroadcastAddress.isEmpty())
    {
        mBroadcastAddress = "0.0.0.0";
    }

    auto list = mBroadcastAddress.split('.', Qt::SkipEmptyParts);
    if (list.count() != 4)
    {
        QByteArray message = createPackage(CMD_PASSIVE_REGISTRATE_RESPONSE, QByteArray::fromStdString(QString("client address is illegal ").arg(mBroadcastAddress).toStdString()));
        sendPackage(message);
        return;
    }

    QByteArray clientAddressArray;
    clientAddressArray.resize(4);

    uint8_t ip = list.at(0).toUInt();
    memcpy(clientAddressArray.data(), &ip, 1);
    ip = list.at(1).toUInt();
    memcpy(clientAddressArray.data() + 1, &ip, 1);
    ip = list.at(2).toUInt();
    memcpy(clientAddressArray.data() + 2, &ip, 1);
    ip = list.at(3).toUInt();
    memcpy(clientAddressArray.data() + 3, &ip, 1);

    QByteArray message = createPackage(CMD_PASSIVE_REGISTRATE_RESPONSE, clientAddressArray);
    sendPackage(message);
}

void UdpServer::start_task_3003_in_thread()
{
    QString path = SoftConfig::getInstance()->getValue("TargetPath", "path");

    if (path.isEmpty())
    {
        LOG_DEBUG("配置文件异常，未找到目标配置项[TargetPath]->[path]");
        QByteArray message = createPackage(CMD_START_PROGRAM_RESPONSE, QByteArray::fromStdString("can not find config item [TargetPath]->[path]"));
        sendPackage(message);
        return;
    }

    QDir dir(path);
    if(!dir.exists())
    {
        QByteArray message = createPackage(CMD_START_PROGRAM_RESPONSE, QByteArray::fromStdString("目标程序路径不存在"));
        sendPackage(message);
        return;
    }

    dir.setFilter(QDir::Files);
    QFileInfoList list = dir.entryInfoList();
    for (auto &info : list)
    {
        if (info.isSymLink())
        {
            PROCESS_INFORMATION pi;

            QString filePath = info.canonicalFilePath();
            if (mMapProgram.contains(filePath))
            {
                DWORD exitCode = -1;
                GetExitCodeProcess(mMapProgram.value(filePath).hProcess, &exitCode);

                if (exitCode == STILL_ACTIVE)  continue;
                auto program = mMapProgram.take(filePath);
                TerminateProcess(program.hProcess, 0);
                CloseHandle(program.hThread);
                CloseHandle(program.hProcess);
            }

            QString fileDir = info.canonicalPath();
            STARTUPINFO si;
            memset(&si, 0, sizeof(STARTUPINFO));
            si.cb = sizeof(STARTUPINFO);
            si.dwFlags = STARTF_RUNFULLSCREEN;
            si.wShowWindow = SW_SHOWMAXIMIZED;

            bool status = CreateProcess(NULL, LPWSTR(filePath.toStdWString().data()) , NULL, NULL, FALSE, 0, NULL, LPWSTR(fileDir.toStdWString().data()), &si, &pi);
            if (status)
            {
                mMapProgram.insert(filePath, pi);
            }
            else
            {
                QByteArray message = createPackage(CMD_START_PROGRAM_RESPONSE, QByteArray::fromStdString(QString("启动程序失败  %1").arg(filePath).toStdString()));
                sendPackage(message);
            }

            // 找到窗口句柄需要枚举，过程较为耗时，不建议在此处处理
        }
    }

    QByteArray message = createPackage(CMD_START_PROGRAM_RESPONSE, QByteArray::fromStdString("启动程序动作完成"));
    sendPackage(message);
}

void UdpServer::start_task_3005_in_thread()
{
    bool status = true;
    for (auto &program : mMapProgram)
    {
        TerminateProcess(program.hProcess, 0);
        DWORD exitCode = 65535;
        GetExitCodeProcess(program.hProcess, &exitCode);

        CloseHandle(program.hThread);
        CloseHandle(program.hProcess);

        if (exitCode != 0) status = false;
    }

    // 清空缓存
    mMapProgram.clear();

    QByteArray message = createPackage(CMD_STOP_PROGRAM_RESPONSE, QByteArray::fromStdString(QString("%1").arg(status ? "关闭程序动作完成" : "关闭程序动作完成，存在未能关闭应用").toStdString()));
    sendPackage(message);
}

void UdpServer::start_task_3007_in_thread(const QByteArray &array)
{
    if (array.size() < 2)
    {
        QByteArray message = createPackage(CMD_KEYBOARD_PRESS_RESPONSE, QByteArray::fromStdString(QString("按键标志异常").toStdString()));
        sendPackage(message);
        return;
    }

    INPUT input[6];
    memset(input, 0, sizeof(input));
    input[0].type = input[1].type = input[2].type = input[3].type = input[4].type = input[5].type = INPUT_KEYBOARD;

    uint8_t modifierCount = array.at(0);
    if (modifierCount > 2)
    {
        QByteArray message = createPackage(CMD_KEYBOARD_PRESS_RESPONSE, QByteArray::fromStdString(QString("修饰键超过两个").toStdString()));
        sendPackage(message);
        return;
    }

    for (uint8_t i = 0; i < modifierCount; i++)
    {
        input[i].ki.wVk = array.at(1 + i);
        input[i + (modifierCount + 1)].ki.wVk = array.at(1 + i);
    }

    uint8_t key = array.at(1 + modifierCount);
    input[modifierCount].ki.wVk = key;

    input[modifierCount + (modifierCount + 1)].ki.wVk = key;
    for (uint8_t i = 0; i < modifierCount + 1; i++)
    {
        input[modifierCount + 1 + i].ki.dwFlags = KEYEVENTF_KEYUP;
    }

    SendInput((1 + modifierCount) * 2, input, sizeof(INPUT));

    QByteArray message = createPackage(CMD_KEYBOARD_PRESS_RESPONSE, QByteArray::fromStdString(QString("按键完成").toStdString()));
    sendPackage(message);
}

void UdpServer::start_task_3009_in_thread(const QByteArray &array)
{
    uint16_t x = 0;
    memcpy(&x, array.data(), 2);
    uint16_t y = 0;
    memcpy(&y, array.data() + 2, 2);

    RECT rect;
    ::GetWindowRect(::GetDesktopWindow(), &rect);

    INPUT input[2];
    input[0].type = INPUT_MOUSE;
    input[0].mi.dx = x * (65535 / float(rect.right));
    input[0].mi.dy = y * (65535 / float(rect.bottom));
    input[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;
    SendInput(1, input, sizeof(INPUT));

    QByteArray message = createPackage(CMD_MOVE_MOUSE_RESPONSE, QByteArray::fromStdString(QString("鼠标移动完成").toStdString()));
    sendPackage(message);
}

void UdpServer::start_task_3011_in_thread()
{
    INPUT input[2];
    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
    SendInput(1, input, sizeof(INPUT));


    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;
    SendInput(1, input + 1, sizeof(INPUT));

    QByteArray message = createPackage(CMD_CLICK_MOUSEM_RESPONSE, QByteArray::fromStdString(QString("鼠标点击完成").toStdString()));
    sendPackage(message);
}

void UdpServer::start_task_3013_in_thread(const QByteArray &array)
{
    uint16_t delay = 0;
    memcpy(&delay, array.data(), 2);

    std::this_thread::sleep_for(std::chrono::milliseconds(delay * 1000));

    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;

    // Get a token for this process.
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        QByteArray message = createPackage(CMD_SHUTDOWN_COMPUTER_RESPONSE, QByteArray::fromStdString(QString("获取权限失败").toStdString()));
        sendPackage(message);
        return;
    }

    // Get the LUID for the shutdown privilege.
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);

    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);

    if (GetLastError() != ERROR_SUCCESS)
    {
        QByteArray message = createPackage(CMD_SHUTDOWN_COMPUTER_RESPONSE, QByteArray::fromStdString(QString("权限添加失败").toStdString()));
        sendPackage(message);
        return;
    }

    // Shut down the system and force all applications to close.
    if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0))
    {
        QByteArray message = createPackage(CMD_SHUTDOWN_COMPUTER_RESPONSE, QByteArray::fromStdString(QString("系统关机异常").toStdString()));
        sendPackage(message);
    }
    else
    {
        QByteArray message = createPackage(CMD_SHUTDOWN_COMPUTER_RESPONSE, QByteArray::fromStdString(QString("系统关机，再见").toStdString()));
        sendPackage(message);
    }
}

void UdpServer::start_task_3015_in_thread()
{
    QByteArray message = createPackage(CMD_HEARTBEAT_DETECTION_RESPONSE, QByteArray::fromStdString(QString("心跳").toStdString()));
    sendPackage(message);
}

void UdpServer::slot_command_socket_error_occurred(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error);
    // 需要日志记录
    LOG_DEBUG(QString("网络数据通道出现异常，已关闭, [错误码: %1]").arg(QString::number(error)).toStdString().data());
}

void UdpServer::slot_recv_command_data()
{
    while (mSocketCommand->hasPendingDatagrams())
    {
        QNetworkDatagram datagram = mSocketCommand->receiveDatagram();
        mByteArray .append(datagram.data());
    }

    uint32_t length = 0;
    while (mByteArray.size() != 0)
    {
        if (mByteArray.length() < 4)
        {
            break;
        }
        else
        {
            memcpy(&length, mByteArray.data() + 2, 2);
        }
        if (length == mByteArray.length())
        {
            parsePackage(mByteArray);
            mByteArray.clear();
        }
        else if (length > mByteArray.length())
        {
            break;
        }
        else
        {
            parsePackage(mByteArray.left(length));
            mByteArray = mByteArray.right(mByteArray.length() - length);
        }
    }
}
