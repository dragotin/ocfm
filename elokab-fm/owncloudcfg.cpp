#include "owncloudcfg.h"

#include <QStandardPaths>
#include <QString>
#include <QFileInfo>
#include <QDebug>

ownCloudCfg::ownCloudCfg()
{
    QString ocCfgPath = QStandardPaths::locate(QStandardPaths::ConfigLocation, "ownCloud/owncloud.cfg");

    QFileInfo fi(ocCfgPath);
    if (fi.exists()) {
        QSettings settings(ocCfgPath, QSettings::IniFormat);
        auto ak = settings.allKeys();
        settings.beginGroup(QStringLiteral("Accounts"));

        for( const auto &folderAlias: settings.childGroups()) {
            settings.beginGroup(folderAlias);
            settings.beginGroup(QStringLiteral("FoldersWithPlaceholders"));
            for( const auto &id : settings.childGroups()) {
                settings.beginGroup(id);
                const auto path = settings.value(QLatin1String("localPath")).toString();
                const auto vfsModeString = settings.value(QStringLiteral("virtualFilesMode"), "real").toString();
                _ocPaths[path] = vfsModeString;
                settings.endGroup();
            }
            settings.endGroup();
            settings.beginGroup(QStringLiteral("Folders"));
            for( const auto &id : settings.childGroups()) {
                settings.beginGroup(id);
                const auto path = settings.value(QLatin1String("localPath")).toString();
                _ocPaths[path] = "real";
                settings.endGroup();
            }
            settings.endGroup();
            settings.endGroup();
        }
    }
    qDebug() << "Loaded" << _ocPaths.size() << "owncloud folder";
}

bool ownCloudCfg::isOwnCloudPath(const QString& path) const
{
    if (path.isEmpty()) return false;

    if (path.contains("ownCloud2"))
        qDebug() << "Stop here";

    for(const auto &ocp : _ocPaths.keys()) {
        if (path.startsWith(ocp) || ocp == path+"/") {
            return true;
        }
    }
    return false;
}
