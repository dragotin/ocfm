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

#include <QtNetwork/QLocalSocket>
#include <qcoreevent.h>
#include <QStandardPaths>
#include <QFile>
#include "owncloudsocket.h"

#define APPLICATION_SHORTNAME "ownCloud"

ownCloudSocket* ownCloudSocket::instance()
{
    static ownCloudSocket self;
    return &self;
}

ownCloudSocket::ownCloudSocket()
{
    connect(&_socket, &QLocalSocket::connected, this, &ownCloudSocket::slotConnected);
    connect(&_socket, &QLocalSocket::disconnected, this, &ownCloudSocket::slotDisconnected);
    connect(&_socket, &QLocalSocket::readyRead, this, &ownCloudSocket::slotReadyRead);
    _connectTimer.start(45 * 1000, Qt::VeryCoarseTimer, this);
    tryConnect();
}

void ownCloudSocket::timerEvent(QTimerEvent *e)
{
    if (e->timerId() == _connectTimer.timerId()) {
        tryConnect();
        return;
    }
    QObject::timerEvent(e);
}

bool ownCloudSocket::isConnected() const
{
    return _socket.state() == QLocalSocket::ConnectedState;
}

void ownCloudSocket::sendCommand(const char* data)
{
    _socket.write(data);
    _socket.flush();
}

void ownCloudSocket::slotConnected()
{
    sendCommand("VERSION:\n");
    sendCommand("GET_STRINGS:\n");
    emit connectionStatus(true);

}

void ownCloudSocket::slotDisconnected()
{
    emit connectionStatus(false);
}

void ownCloudSocket::tryConnect()
{
    if (_socket.state() != QLocalSocket::UnconnectedState) {
        return;
    }
    
    QString socketPath = QStandardPaths::locate(QStandardPaths::RuntimeLocation,
                                                APPLICATION_SHORTNAME,
                                                QStandardPaths::LocateDirectory);
    if(socketPath.isEmpty())
        return;

    _socket.connectToServer(socketPath + QLatin1String("/socket"));
}

void ownCloudSocket::slotReadyRead()
{
    while (_socket.bytesAvailable()) {
        _line += _socket.readLine();
        if (!_line.endsWith("\n"))
            continue;
        QByteArray line;
        qSwap(line, _line);
        line.chop(1);
        if (line.isEmpty())
            continue;

        if (line.startsWith("REGISTER_PATH:")) {
            auto col = line.indexOf(':');
            QString file = QString::fromUtf8(line.constData() + col + 1, line.size() - col - 1);
            _paths.append(file);
            continue;
        } else if (line.startsWith("STRING:")) {
            auto args = QString::fromUtf8(line).split(QLatin1Char(':'));
            if (args.size() >= 3) {
                _strings[args[1]] = args.mid(2).join(QLatin1Char(':'));
            }
            continue;
        } else if (line.startsWith("VERSION:")) {
            auto args = line.split(':');
            auto version = args.value(2);
            _version = version;
            if (!version.startsWith("1.")) {
                // Incompatible version, disconnect forever
                _connectTimer.stop();
                _socket.disconnectFromServer();
                return;
            }
        }
        emit commandRecieved(line);
    }
}


void ownCloudSocket::ownCloudSocketFileListCall(const QStringList& fileList, const QByteArray& cmd)
{
    QByteArray buf{cmd};
    if (fileList.size() > 0) {
        const QString allFiles = fileList.join(QChar('\x1e'));
        buf.append(allFiles.toUtf8());
        buf.append("\n");
        sendCommand(buf.data());
    }

}

void ownCloudSocket::slotOwnCloudHydrate(const QStringList& list)
{
    const QByteArray buf {"MAKE_AVAILABLE_LOCALLY:"};
    ownCloudSocketFileListCall(list, buf);
}

void ownCloudSocket::slotOwnCloudDehydrate(const QStringList& list)
{
    const QByteArray buf {"MAKE_ONLINE_ONLY:"};
    ownCloudSocketFileListCall(list, buf);
}
