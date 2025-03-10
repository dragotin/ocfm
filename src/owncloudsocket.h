/******************************************************************************
 *   Copyright (C) 2014 by Olivier Goffart <ogoffart@woboq.com>               *
 *                                                                            *
 *   This program is free software; you can redistribute it and/or modify     *
 *   it under the terms of the GNU General Public License as published by     *
 *   the Free Software Foundation; either version 2 of the License, or        *
 *   (at your option) any later version.                                      *
 *                                                                            *
 *   This program is distributed in the hope that it will be useful,          *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *   GNU General Public License for more details.                             *
 *                                                                            *
 *   You should have received a copy of the GNU General Public License        *
 *   along with this program; if not, write to the                            *
 *   Free Software Foundation, Inc.,                                          *
 *   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA               *
 ******************************************************************************/

#pragma once
#include <QObject>
#include <QBasicTimer>
#include <QLocalSocket>
#include <QRegularExpression>

class ownCloudSocket : public QObject {
    Q_OBJECT
public:
    const static QString DehydSuffix;

    static ownCloudSocket *instance();

    bool isConnected() const;
    void sendCommand(const char *data);
    QVector<QString> paths() const { return _paths; }

    QString contextMenuTitle() const
    {
        return _strings.value("CONTEXT_MENU_TITLE", "ownCloud");
    }
    QString shareActionTitle() const
    {
        return _strings.value("SHARE_MENU_TITLE", "Share...");
    }

    QString copyPrivateLinkTitle() const { return _strings["COPY_PRIVATE_LINK_MENU_TITLE"]; }
    QString emailPrivateLinkTitle() const { return _strings["EMAIL_PRIVATE_LINK_MENU_TITLE"]; }

    QByteArray version() { return _version; }

public slots:
    void slotOwnCloudHydrate(const QStringList& list);
    void slotOwnCloudDehydrate(const QStringList& list);
    void tryConnect();

signals:
    void commandRecieved(const QByteArray &cmd);
    void connectionStatus(bool status);

protected:
    void timerEvent(QTimerEvent*) override;

private slots:
    void slotConnected();
    void slotDisconnected();
    void slotReadyRead();

private:
    ownCloudSocket();

    void ownCloudSocketFileListCall(const QStringList& fileList, const QByteArray& cmd);

    QLocalSocket _socket;
    QByteArray _line;
    QVector<QString> _paths;
    QBasicTimer _connectTimer;

    QMap<QString, QString> _strings;
    QByteArray _version;
};
