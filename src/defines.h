#ifndef DEFINES_H
#define DEFINES_H

#include <QtGlobal>

#define D_OWNCLOUD      Qt::UserRole+6
#define D_OWNCLOUD_DEHYDRATED Qt::UserRole+7
#define D_MTYPE         Qt::UserRole+3
#define D_MSize         Qt::UserRole+4
#define D_MFPATH        Qt::UserRole+1
#define D_MMIM          Qt::UserRole
#define D_MARGINS       3

#define D_APPNAME       QStringLiteral("oCfm")
#define D_APPNAME_GUI   QStringLiteral("ownCloud File Manager")
#define D_APPVERSION    QStringLiteral("0.5")
#define D_APPORGNAME    QStringLiteral("ownCloud")

#define D_TRASH         ":/trash"
#define D_SEARCH        ":/search"

#define D_IMAGE_TYPE    "image"
#define D_PDF_TYPE      "pdf"
#define D_VIDEO_TYPE    "video"

#define THUMB_LAST_MODIFIED QStringLiteral("Thumb::MTime")
#define THUMB_URI           QStringLiteral("Thumb::URI")
#define THUMB_MIMETYPE      QStringLiteral("Thumb::Mimetype")
#define THUMB_SIZE          QStringLiteral("Thumb::Size")

#define D_COL_NAME      0
#define D_COL_SIZE      1
#define D_COL_TYPE      2
#define D_COL_DATE      3
#define D_COL_TRASHED   4
#define D_COL_ORIGPATH  5

#if QT_VERSION < QT_VERSION_CHECK(5, 15, 0)
#define ENDL endl
#else
#define ENDL Qt::endl
#endif

#endif // DEFINES_H
