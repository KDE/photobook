/*
 *   Copyright 2012 Daniel Nicoletti <dantti12@gmail.com>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#include "Importer.h"

#include "Importer/Database.h"

#include <QTimer>

#include <KDebug>

Importer::Importer() :
    m_cancel(false)
{
}

void Importer::importFromFileList(const QString &uuid, const QStringList &fileList, const QString &eventname, bool splitEvents)
{
    bool ret;
    ret = !QMetaObject::invokeMethod(Database::global(),
                                     "import",
                                     Qt::QueuedConnection,
                                     Q_ARG(QString, uuid),
                                     Q_ARG(QStringList, fileList),
                                     Q_ARG(QString, eventname),
                                     Q_ARG(bool, splitEvents));
    if (ret) {
        kWarning() << "Failed to invoke method: importFromFileList";
        emit finished();
    }
}

void Importer::importFromPath(const QString &path)
{
    if (QThread::currentThread() == Database::global()) {
        Database::global()->importFromPath(path);
    } else {
        // Move the code to the writer thread
        moveToThread(Database::global());
        bool ret;
        ret = !QMetaObject::invokeMethod(this,
                                         "importFromPath",
                                         Qt::QueuedConnection,
                                         Q_ARG(QString, path));
        if (ret) {
            kWarning() << "Failed to invoke method: importFromPath";
            emit finished();
        }
    }
}

void Importer::cancel()
{
    m_cancel = true;
}

