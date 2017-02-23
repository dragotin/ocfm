/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2
 */
/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtGui module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
//END_COMMON_COPYRIGHT_HEADER

/*************************************************************************
 It's fixes the following bugs:
   * QIcon::fromTheme returns pixmaps that are bigger than requested
        https://bugreports.qt.nokia.com/browse/QTBUG-17953

   * Qt should honor /usr/share/pixmaps as a valid icon directory on Linux
        https://bugreports.qt.nokia.com/browse/QTBUG-12874

  *************************************************************************/

#ifndef QDESKTOPICON_P_H
#define QDESKTOPICON_P_H

#ifndef QT_NO_ICON
//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

//#include <QtGui/QIcon>
#include <QtGui/QIconEngine>
//#include <QtGui/QPixmapCache>
//#include "qt/qicon_p.h"
//#include "qt/qfactoryloader_p.h"
#include <QtCore/QHash>

QT_BEGIN_NAMESPACE

class QIconLoader;

struct QIconDirInfo
{
    enum Type { Fixed, Scalable, Threshold };
    QIconDirInfo(const QString &_path = QString()) :
            path(_path),
            size(0),
            maxSize(0),
            minSize(0),
            threshold(0),
            type(Threshold) {}
    QString path;
    short size;
    short maxSize;
    short minSize;
    short threshold;
    Type type : 4;
};

class QIconLoaderEngineEntry
 {
public:
    virtual ~QIconLoaderEngineEntry() {}
    virtual QPixmap pixmap(const QSize &size,
                           QIcon::Mode mode,
                           QIcon::State state) = 0;
    QString filename;
    QIconDirInfo dir;
    static int count;

};

struct ScalableEntry : public QIconLoaderEngineEntry
{
    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state);
    QIcon svgIcon;
};

struct PixmapEntry : public QIconLoaderEngineEntry
{
    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state);
    QPixmap basePixmap;
};


typedef QList<QIconLoaderEngineEntry*> QThemeIconEntries;

class QIconLoaderEngineFixed : public QIconEngine
{
public:
    QIconLoaderEngineFixed(const QString& iconName = QString(),const QString& FillBack = QString());
    ~QIconLoaderEngineFixed();

void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state);
    QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state);
    QSize actualSize(const QSize &size, QIcon::Mode mode, QIcon::State state);
    QIconEngine *clone() const;
    bool read(QDataStream &in);
    bool write(QDataStream &out) const;

private:
    QString key() const;
    bool hasIcon() const;
    void ensureLoaded();
    void virtual_hook(int id, void *data);
    QIconLoaderEngineEntry *entryForSize(const QSize &size);
    QIconLoaderEngineFixed(const QIconLoaderEngineFixed &other);
    QThemeIconEntries m_entries;
    QString m_iconName;
    QString m_iconFallBack;
    QString m_themName;
    uint m_key;

    friend class QIconLoader;
};

class QIconTheme
{
public:
    QIconTheme(const QString &name);
    QIconTheme() : m_valid(false) {}
    QStringList parents() { return m_parents; }
    QList <QIconDirInfo> keyList() { return m_keyList; }
    QString contentDir() { return m_contentDir; }
    QStringList contentDirs() { return m_contentDirs; }
    bool isValid() { return m_valid; }

private:
    QString m_contentDir;
    QStringList m_contentDirs;
    QList <QIconDirInfo> m_keyList;
    QStringList m_parents;
    bool m_valid;
};

class QIconLoader : public QObject
{
public:
    QIconLoader();
    QThemeIconEntries loadIcon(const QString &themeName,const QString &iconName) const;
    uint themeKey() const { return m_themeKey; }
    uint themeKeyOld() const { return m_themeKeyOld; }
    QString themeName() const { return m_userTheme.isEmpty() ? m_systemTheme : m_userTheme; }

    void setThemeName(const QString &themeName);
    QIconTheme theme() { return themeList.value(themeName()); }
    void setThemeSearchPath(const QStringList &searchPaths);
    QStringList themeSearchPaths() const;
    QIconDirInfo dirInfo(int dirindex);
    static QIconLoader *instance();
    void updateSystemTheme();
    void invalidateKey() {m_themeKeyOld=m_themeKey; m_themeKey++; }
    void ensureInitialized();

private:
    QThemeIconEntries findIconHelper(const QString &themeName,
                                     const QString &iconName,
                                     QStringList &visited) const;

    QThemeIconEntries entries(const QString &path,
                                     const QString &iconName, QThemeIconEntries m_entries) const;
    uint m_themeKey;
    uint m_themeKeyOld;
    bool m_supportsSvg;
    bool m_initialized;

    mutable QString m_userTheme;
    mutable QString m_systemTheme;
    mutable QStringList m_iconDirs;
    mutable QHash <QString, QIconTheme> themeList;
};

QT_END_NAMESPACE

#endif // QDESKTOPICON_P_H

#endif //QT_NO_ICON
