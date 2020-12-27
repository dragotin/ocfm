#ifndef OWNCLOUDCFG_H
#define OWNCLOUDCFG_H

#include <QObject>
#include <QSettings>

class ownCloudCfg
{
public:
    ownCloudCfg();

    bool isOwnCloudPath(const QString& path) const;
    bool isSuffixVfs(const QString& path) const;

private:
    QMap<QString, QString> _ocPaths;
};

#endif // OWNCLOUDCFG_H
