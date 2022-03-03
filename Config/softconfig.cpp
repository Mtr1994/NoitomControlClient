#include "softconfig.h"

#include "Log/logger.h"

#include <QTextCodec>
#include <string>
#include <QStringList>

using namespace std;

SoftConfig::SoftConfig(QObject *parent) : QObject(parent)
{

}

void SoftConfig::init(const QString &path)
{
    LOG_DEBUG("初始化配置文件");
    mSetting = new QSettings(QString("%1/%2").arg(path, "conf.ini"), QSettings::IniFormat);
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
