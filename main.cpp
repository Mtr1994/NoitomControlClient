#include <QCoreApplication>
#include "mainserver.h"
#include "Config/softconfig.h"
#include "Log/logger.h"

#include <QDir>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    // 初始化日志系统
    Logger logger;
    logger.init();
    LOG_DEBUG("日志系统就绪");

    // 修改当前工作目录
    QDir::setCurrent(qApp->applicationDirPath());
    LOG_DEBUG(QString("切换到工作目录 %1").arg(qApp->applicationDirPath()).toStdString());

    // 初始化配置文件
    bool status = SoftConfig::getInstance()->init(qApp->applicationDirPath());
    if (!status) return -1;

    MainServer server;
    server.start();

    return a.exec();
}
