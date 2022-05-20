#include "softconfig.h"

#include "Log/logger.h"

#include <QTextCodec>
#include <string>
#include <QStringList>
#include <QFile>

using namespace std;

SoftConfig::SoftConfig(QObject *parent) : QObject(parent)
{

}

bool SoftConfig::init(const QString &path)
{
    LOG_DEBUG("初始化配置文件");
    QString file = QString("%1/%2").arg(path, "conf.ini");
    if (!QFile::exists(file))
    {
        LOG_DEBUG("配置文件丢失，程序结束，请联系管理员");
        return false;
    }

    mSetting = new QSettings(file, QSettings::IniFormat);
    return true;
}

SoftConfig *SoftConfig::getInstance()
{
    static SoftConfig config;
    return &config;
}

QString SoftConfig::getValue(const QString& entry, const QString& item)
{
    QString value = mSetting->value(entry + "/" + item).toString();
    return value;
}

void SoftConfig::setValue(const QString& pEntry, const QString& pItem, const QString& pValue)
{
    mSetting->setValue(pEntry + "/" + pItem, pValue);
}
