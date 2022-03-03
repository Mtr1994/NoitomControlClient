#include <QCoreApplication>
#include "mainserver.h"
#include "Config/softconfig.h"
#include "Log/logger.h"

#include <QDir>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // 修改当前工作目录
    QDir::setCurrent(qApp->applicationDirPath());

    // 初始化日志系统
    Logger logger;
    logger.init();

    // 初始化配置文件
    SoftConfig::getInstance()->init(qApp->applicationDirPath());

    MainServer server;
    server.start();

    return a.exec();
}
