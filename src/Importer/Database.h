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

#ifndef DATABASE_H
#define DATABASE_H

#include <QThread>
#include <QStringList>
#include <QFileInfo>
#include <QTimer>
#include <QVector>
#include <QPair>

#include <KDirWatch>

#undef slots
#include <xapian.h>

//namespace Xapian {
//    class WritableDatabase;
//    class Database;
//    class Document;
//}

typedef QPair<QString, QDateTime> FileDateTime;

class Database : public QThread
{
    Q_OBJECT
public:
    enum {
        OriginalFilePath,
        OriginalMTime,
        OriginalSize,
        ImportedDateTime,
        FilePath,
        Hash,
        LastModified,
        Year,
        Month,
        Day,
        Event,
        EventCover,
        Rating,
        Comment,
        SizeHW
    } XapianRole;
    explicit Database(QObject *parent = 0);
    ~Database();

    static Database* global();

    void init(const QString &dbPath, bool readOnly);
    void validateEntries();


    Xapian::Document createDocument(const QString &encodedPath,
                                    const QFileInfo &fileInfo,
                                    const QString &eventName,
                                    const QDateTime &importDateTime,
                                    const QString &originalFilePath = QString());
    bool alreadyImported(const QString &filePath);
    Xapian::Document findDocument(const QString &uniqueId);
    QString uniqueId(const QString &filePath) const;

    Xapian::Database* database() const;
    QVector<QString> notImported() const;

public Q_SLOTS:
    void import(const QString &uuid,
                const QStringList &files,
                const QString &baseEvenName,
                bool splitEvents);
    void importFromPath(const QString &path);

private Q_SLOTS:
    void created(const QString &path);
    void deleted(const QString &path);
    void dirty(const QString &path);
    void commit();

private:
    void importAsEvent(const QString &eventName, const QStringList &files, const QDateTime &dateTime = QDateTime::currentDateTime());
    /**
      * Imported means it was copied to PhotoBook tree,
      * this way we remove the file prefix to allow moving the DB
      * to another place
      */
    void importFile(const QString &filePath, const QString &eventName, const QDateTime &impotDateTime, const QString &importedFilePath = QString());
    void scanDir(const QString &path);
    void scanFile(const QFileInfo &fileInfo);
    static bool photoOlderThan(const FileDateTime &s1, const FileDateTime &s2);
    QDateTime photoDateTime(const QString &filename) const;

    static Database* m_instance;
    KDirWatch *m_directory;
    Xapian::Database *m_xapianDB;
    Xapian::WritableDatabase *m_rwXapianDB;
    QTimer *m_commitTimer;
    QStringList m_filters;
    QString m_imagesPath;
    QString m_photoBookPath;
    QString m_xapianPath;

    QVector<QString> m_notImported;
};

#endif // DATABASE_H
