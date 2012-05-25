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

#ifndef IMPORTER_H
#define IMPORTER_H

#include <QObject>

class Importer : public QObject
{
    Q_OBJECT
public:
    explicit Importer();

    Q_INVOKABLE void importFromFileList(const QString &uuid,
                                        const QStringList &fileList,
                                        const QString &eventname,
                                        bool splitEvents);
    Q_INVOKABLE void importFromPath(const QString &path);
    Q_INVOKABLE void cancel();
    
signals:
    void imported(const QString &iconPath);
    void importedPercent(int percent);
    void finished();

private:
    bool m_cancel;
};

#endif // IMPORTER_H
