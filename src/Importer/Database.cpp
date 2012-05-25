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

#include "Database.h"

#include "../ImageRequest.h"

#include <libkexiv2/kexiv2.h>
#include <libkexiv2/kexiv2previews.h>

#include <QStringBuilder>
#include <QDir>
#include <QDirIterator>
#include <QCryptographicHash>
#include <QApplication>

#include <KDebug>


Database* Database::m_instance = 0;

Database *Database::global()
{
    if (!m_instance) {
        m_instance = new Database(qApp);
    }

    return m_instance;
}

Database::Database(QObject *parent) :
    QThread(parent),
    m_xapianDB(0),
    m_rwXapianDB(0)
{
    m_directory = new KDirWatch(this);
    connect(m_directory, SIGNAL(created(QString)), this, SLOT(created(QString)));
    connect(m_directory, SIGNAL(deleted(QString)), this, SLOT(deleted(QString)));
    connect(m_directory, SIGNAL(dirty(QString)), this, SLOT(dirty(QString)));

    // Files we can work with
    // They must be capable of writing metadata
    m_filters << "*.jpg"
              << "*.jpeg"
              << "*.png";

    m_commitTimer = new QTimer(this);
    m_commitTimer->setInterval(500);
    m_commitTimer->setSingleShot(true);
    connect(m_commitTimer, SIGNAL(timeout()), this, SLOT(commit()));
}

Database::~Database()
{
    if (m_rwXapianDB) {
        delete m_rwXapianDB;
    }

    if (m_xapianDB) {
        delete m_xapianDB;
    }
}

void Database::init(const QString &dbPath, bool readOnly)
{
//    m_db.init(qApp->property("PicturesPath").toString(), true);
    kDebug() << dbPath;
    m_photoBookPath = dbPath;
    QDir photoBookDir(m_photoBookPath);
    if (!photoBookDir.exists()) {
        photoBookDir.mkpath(m_photoBookPath);
    }
    m_imagesPath = dbPath % QLatin1String("Imported/");
    m_xapianPath = dbPath % QLatin1String("index");

    if (readOnly == false) {
        kDebug() << "Open write";
        try {
            m_rwXapianDB = new Xapian::WritableDatabase(m_xapianPath.toStdString(),
                                                        Xapian::DB_CREATE_OR_OPEN);
        } catch (const Xapian::Error &error) {
            kWarning() << "Exception: " << QString::fromStdString(error.get_msg());
            return;
        }

        m_directory->addDir(m_imagesPath, KDirWatch::WatchSubDirs);

    }

    // load the read only db
    kDebug() << "Open read";
    try {
        m_xapianDB = new Xapian::Database(m_xapianPath.toStdString());
    } catch (const Xapian::Error &error) {
        kWarning() << "Exception: " << QString::fromStdString(error.get_msg());
        return;
    }

    // scan the files, validating the DB's entries
//    if (readOnly == false) {
//        kDebug() << m_imagesPath << QDateTime::currentDateTime();

//        scanDir(dbPath);

//        kDebug() << dbPath << QDateTime::currentDateTime();
//    }
}

void Database::validateEntries()
{
//    // Verify if the file is still valid or if it has been updated
//    Xapian::PostingIterator it = m_rwXapianDB->postlist_begin(std::string());
//    while (it != m_rwXapianDB->postlist_end(std::string())) {
//        Xapian::Document doc = m_rwXapianDB->get_document(*it);

//        QString filePath = QString::fromStdString(doc.get_value(PathFile));
//        QString lastModified = QString::fromStdString(doc.get_value(LastModified));
//        if (!filePath.isEmpty() && !lastModified.isEmpty()) {
//            QFileInfo fileInfo(filePath);
//            if (fileInfo.exists()) {
//                if (fileInfo.lastModified().toMSecsSinceEpoch() != lastModified.toLongLong()) {
//                    // File has changed, update it's entry
//                    kDebug() << "File Changed" << filePath;
//                    m_rwXapianDB->replace_document(*it,
//                                                   createDocument(uniqueId(filePath), fileInfo));

//                    // Start the commit timer as this document has been updated
//                    m_commitTimer->start();
//                }
//            } else {
//                // Remove the entry as the file does not exist
//                kDebug() << "File Removed" << filePath;
//                m_rwXapianDB->delete_document(*it);
//            }
//        } else {
//            // Remove the entry as it is invalid (all entries must have PathFile and LastModified
//            kDebug() << "Invalid entry" << *it;
//            m_rwXapianDB->delete_document(*it);
//        }
//    }
}

void Database::import(const QString &uuid, const QStringList &files, const QString &baseEvenName, bool splitEvents)
{
    int interval = 7200; // 2 hours TODO add some sort of configuration
    // Make sure the files to be imported
    // have the parent dirs in a common order
    QHash<int, QStringList> eventFiles;
    if (splitEvents) {
        QList<FileDateTime> datedFiles;
        foreach (const QString &file, files) {
            // Make a pair file name and it's dateTime
            datedFiles << qMakePair(file, photoDateTime(file));
        }

        // Order the files so we can easily separate the events
        qSort(datedFiles.begin(), datedFiles.end(), photoOlderThan);

        QDateTime lastImported;
        foreach (const FileDateTime &datedFile, datedFiles) {
            if (lastImported.isNull()) {
                eventFiles[0] << datedFile.first;
            } else {
                int difference;
                difference = datedFile.second.toTime_t() - lastImported.toTime_t();
                if (difference > interval) {
                    eventFiles[eventFiles.count()] << datedFile.first;
                } else {
                    eventFiles[eventFiles.count() - 1] << datedFile.first;
                }
            }
            lastImported = datedFile.second;
        }
        kDebug() << eventFiles;
    }

    QDateTime dateTime = QDateTime::currentDateTime();
    QHash<int, QStringList>::const_iterator it = eventFiles.constBegin();
    while (it != eventFiles.constEnd()) {
        // Import event name, fileList, import date time
        QString eventname;
        if (baseEvenName.isEmpty()) {
            eventname = dateTime.toString();
        } else {
            if (eventFiles.size() == 1) {
                eventname = baseEvenName;
            } else {
                eventname = QString("%1 %2").arg(baseEvenName, QString::number(it.key() + 1));
            }
        }

        importAsEvent(eventname, it.value(), dateTime);
        dateTime.addSecs(1);
        ++it;
    }
}

void Database::importFromPath(const QString &path)
{
    QTime t;
    t.start();
    QStringList files = ImageRequest::scanDirReturning(path);
    qDebug("Time elapsed: %d ms, found %d images", t.elapsed(), files.size());

    // EventName/files
    QHash<QString, QStringList> eventFiles;
    foreach (const QString &file, files) {
        QString eventName;
        eventName = file.section(QLatin1Char('/'), -2, -2);

        // Add the file to the event files hash
        eventFiles[eventName] << file;
    }

    QDateTime dateTime = QDateTime::currentDateTime();
    QHash<QString, QStringList>::const_iterator it = eventFiles.constBegin();
    while (it != eventFiles.constEnd()) {
        // Import event name, fileList, import date time
        importAsEvent(it.key(), it.value(), dateTime);
        dateTime.addSecs(1);
        ++it;
    }
}

void Database::importAsEvent(const QString &eventName, const QStringList &files, const QDateTime &dateTime)
{
    QString basePath = qApp->property("PicturesPath").toString();
    QString photoPath;
    photoPath = basePath
            % QLatin1String("Imported/")
            % dateTime.toString(QLatin1String("yyyy/MM/yyyy-MM-dd hh:mm:ss/"));
    kDebug() << "ImportedPath" << photoPath;
    QString thumbPath;
    thumbPath = basePath
            % QLatin1String("Thumbs/")
            % dateTime.toString(QLatin1String("yyyy/MM/yyyy-MM-dd hh:mm:ss/"));
    kDebug() << "Thumbs Path" << photoPath;

    // Make sure the photos dir exists
    QDir photoDir(photoPath);
    if (!photoDir.exists() && !photoDir.mkpath(photoPath)) {
        kWarning() << "Failed to create photo path" << photoPath;
        return;
    }

    // Make sure the thumbs dir exists
    QDir thumbDir(thumbPath);
    if (!thumbDir.exists() && !thumbDir.mkpath(thumbPath)) {
        kWarning() << "Failed to create photo path" << thumbPath;
        return;
    }

    foreach (const QString &file, files) {
        // Copy the file
        QFileInfo fileInfo(file);
        if (alreadyImported(file))
        kDebug() << "File was imported";
        QString newName = photoPath % fileInfo.fileName();
        if (QFile::copy(file, newName)) {
            // Index the file
            importFile(newName, eventName, dateTime, file);
        } else {
            kWarning() << "Failed to copy file to database" << file << newName;
            // TODO
            // do not commit? rollback?
        }
    }
}

void Database::importFile(const QString &filePath,
                          const QString &eventName,
                          const QDateTime &impotDateTime,
                          const QString &importedFilePath)
{
    // Make sure the file path is created
    QString basePath;
    basePath = qApp->property("PicturesPath").toString();

    // Get the unique id generated from the file path
    QString encodedPath;
    if (!importedFilePath.isNull()) {
        // If the file was imported remove the basePath
        // for the so that we can move our db to another place
        encodedPath = uniqueId(filePath.right(basePath.length()));
        kDebug() << "Creating unique ID for " << basePath << filePath <<  filePath.mid(basePath.size());
    } else {
        // Just encode the whole path
        encodedPath = uniqueId(filePath);
        kDebug() << "Creating unique ID for " << filePath;
    }

    // Check if the document is on the DataBase
    Xapian::Document document = findDocument(encodedPath);
    if (document.get_docid() == 0) {
//        kDebug() << "No document with id" << encodedPath;
        QFileInfo fileInfo(filePath);

        // It's said that adding the docs sorted it faster.. (change in future)
        m_rwXapianDB->replace_document(encodedPath.toStdString(),
                                       createDocument(encodedPath, fileInfo, eventName, impotDateTime, importedFilePath));
    } else {
        kWarning() << "Something really odd happened, the doc was already imported" << filePath;

//        kDebug() << "Document with id" << encodedPath << "has docid" << document.get_docid();

//        if (!document.get_value(Rating).empty()) {
//            kDebug() << "Has RATING" << QString::fromStdString(document.get_value(Rating));
//        }

//        if (fileInfo.lastModified().toMSecsSinceEpoch() !=
//                QString::fromStdString(document.get_value(LastModified)).toLongLong()) {
//            // File has changed, update it's entry
//            kDebug() << "StatIfo Changed"
//                     << QString::fromStdString(document.get_value(LastModified)).toLongLong()
//                     << fileInfo.lastModified().toMSecsSinceEpoch();
//            m_rwXapianDB->replace_document(encodedPath.toStdString(),
//                                           createDocument(encodedPath, fileInfo));

//            // Start the commit timer as this document has been updated
//            m_commitTimer->start();
//        }
    }

    // Start the commit timer as this document has been updated
    m_commitTimer->start();
}

void Database::scanDir(const QString &path)
{
//    kDebug() << path;
    QDirIterator it(path, QDirIterator::Subdirectories);
     while (it.hasNext()) {
         QString filePath = it.next();
         // Skip the index dir
         if (filePath.startsWith(m_xapianPath)) {
             continue;
         }

         // Update or Insert the file on the index
         scanFile(it.fileInfo());
    }
}

// make sure fileInfo reffers to a file before calling this function
void Database::scanFile(const QFileInfo &fileInfo)
{
    kDebug() << fileInfo.filePath();
//    QString fileName = fileInfo.fileName();
//    if ((!fileName.endsWith(QLatin1String(".jpg"), Qt::CaseInsensitive) &&
//         !fileName.endsWith(QLatin1String(".jpeg"), Qt::CaseInsensitive) &&
//         !fileName.endsWith(QLatin1String(".png"), Qt::CaseInsensitive)) ||
//        fileInfo.filePath().startsWith(m_photoBookPath)) {
//        // Skip non image files
//        return;
//    }

//    // Get the unique id generated from the file path
//    QString encodedPath = uniqueId(fileInfo.filePath());

//    // Check if the document is on the DataBase
//    Xapian::Document document = findDocument(encodedPath);
//    if (document.get_docid() == 0) {
//        kDebug() << "No document with id" << encodedPath;
//        m_notImported << fileInfo.filePath();
//        // emit notImported()

//        // It's said that adding the docs sorted it faster.. (change in future)
////        m_rwXapianDB->replace_document(encodedPath.toStdString(),
////                                       createDocument(encodedPath, fileInfo));
//    } else {
////        kDebug() << "Document with id" << encodedPath << "has docid" << document.get_docid();

//        if (!document.get_value(Rating).empty()) {
//            kDebug() << "Has RATING" << QString::fromStdString(document.get_value(Rating));
//        }

//        if (fileInfo.lastModified().toMSecsSinceEpoch() !=
//                QString::fromStdString(document.get_value(LastModified)).toLongLong()) {
//            // File has changed, update it's entry
//            kDebug() << "StatIfo Changed"
//                     << QString::fromStdString(document.get_value(LastModified)).toLongLong()
//                     << fileInfo.lastModified().toMSecsSinceEpoch();
//            m_rwXapianDB->replace_document(encodedPath.toStdString(),
//                                           createDocument(encodedPath, fileInfo));

//            // Start the commit timer as this document has been updated
//            m_commitTimer->start();
//        }
    //    }
}

bool Database::photoOlderThan(const FileDateTime &s1, const FileDateTime &s2)
{
    return s1.second < s2.second;
}

QDateTime Database::photoDateTime(const QString &filename) const
{
    KExiv2Iface::KExiv2	image(filename);

    // Get the digitalization time, falling back to creation time
    QDateTime dateTime = image.getDigitizationDateTime(true);

    // If any of the returned dates is null fallback to
    // the creation time of the file
    if (dateTime.isNull()) {
        QFileInfo fileInfo(filename);
        dateTime = fileInfo.created();
    }
    return dateTime;
}

Xapian::Document Database::createDocument(const QString &encodedPath,
                                          const QFileInfo &fileInfo,
                                          const QString &eventName,
                                          const QDateTime &importDateTime,
                                          const QString &originalFilePath)
{
    // Grab the image info and setup a document
    KExiv2Iface::KExiv2	image(fileInfo.filePath());
    Xapian::Document document;
    document.add_term(encodedPath.toStdString());

    // Store the information about the original file
    if (!originalFilePath.isNull()) {
        QFileInfo originalFileInfo(originalFilePath);
        kDebug() << originalFilePath;
        document.add_value(OriginalFilePath, originalFilePath.toStdString());
        document.add_value(OriginalMTime, QString::number(originalFileInfo.lastModified().toMSecsSinceEpoch()).toStdString());
        document.add_value(OriginalSize, QString::number(originalFileInfo.size()).toStdString());
    }

    document.add_value(ImportedDateTime, QString::number(importDateTime.toTime_t()).toStdString());
    document.add_value(Event, eventName.toStdString());
    document.add_value(EventCover, "1");
    document.add_value(FilePath, fileInfo.filePath().toStdString());
    document.add_value(LastModified, QString::number(fileInfo.lastModified().toMSecsSinceEpoch()).toStdString());

    QDateTime dt = image.getImageDateTime();
    document.add_value(Year, dt.date().toString(QLatin1String("yyyy")).toStdString());
    document.add_value(Month, dt.date().toString(QLatin1String("M")).toStdString());
    document.add_value(Day, dt.date().toString(QLatin1String("d")).toStdString());
    document.add_value(Rating, image.getXmpTagString("Xmp.xmp.Rating").toStdString());

    return document;
}

bool Database::alreadyImported(const QString &filePath)
{
    // Todo put a try catch here...
    kDebug() << filePath;
    QFileInfo fileInfo(filePath);
    Xapian::Enquire enquire(*m_xapianDB);
    Xapian::Query filePathQuery(Xapian::Query::OP_VALUE_RANGE,
                                OriginalFilePath,
                                filePath.toStdString(),
                                filePath.toStdString());
    Xapian::Query modifedDateQuery(Xapian::Query::OP_VALUE_RANGE,
                                   OriginalMTime,
                                   QString::number(fileInfo.lastModified().toMSecsSinceEpoch()).toStdString(),
                                   QString::number(fileInfo.lastModified().toMSecsSinceEpoch()).toStdString());
    Xapian::Query fileSizeQuery(Xapian::Query::OP_VALUE_RANGE,
                                OriginalSize,
                                QString::number(fileInfo.size()).toStdString(),
                                QString::number(fileInfo.size()).toStdString());
    // Both must be in
    Xapian::Query queryModifieSize(Xapian::Query::OP_AND,
                                   modifedDateQuery,
                                   fileSizeQuery);
    Xapian::Query query(Xapian::Query::OP_AND,
                        filePathQuery,
                        queryModifieSize);
    enquire.set_query(query);
    // Get matches one result is enough
    Xapian::MSet matches = enquire.get_mset(0, 1, m_xapianDB->get_doccount());
    for (Xapian::MSetIterator i = matches.begin(); i != matches.end(); ++i) {
        Xapian::Document doc = i.get_document();
        kDebug() << "FOUND Document ID " << QString::number(*i) << "\t" <<
                QString::number(i.get_percent()) << "% [" <<
                    QString::fromStdString(doc.get_data()) << "]";
    }
    return false;
}

Xapian::Document Database::findDocument(const QString &uniqueId)
{
    // look up the given unique id on the database
    Xapian::Document ret;
    Xapian::PostingIterator p = m_xapianDB->postlist_begin(uniqueId.toStdString());
    if (p == m_xapianDB->postlist_end(uniqueId.toStdString())) {
        kDebug() << "No document with id" << uniqueId;
        return ret;
    } else {
        return m_xapianDB->get_document(*p);
    }
}

QString Database::uniqueId(const QString &filePath) const
{
    // Creates an unique id for the given path
    QCryptographicHash pathHash(QCryptographicHash::Sha1);
    pathHash.addData(filePath.toUtf8());
    return QLatin1Char('Q') + pathHash.result().toHex();
}

Xapian::Database* Database::database() const
{
    return m_xapianDB;
}

QVector<QString> Database::notImported() const
{
    return m_notImported;
}

void Database::created(const QString &path)
{
    QFileInfo fileInfo(path);
    if (fileInfo.isDir()) {
        // Scan the newly created directory
        scanDir(path);
    } else {
        // tries to add the newly created file
        scanFile(fileInfo);
    }
    kDebug() << path;
}

void Database::deleted(const QString &path)
{
    kDebug() << path;

    // Get the unique id generated from the file path
    QString encodedPath = uniqueId(path);

    try {
        m_rwXapianDB->delete_document(encodedPath.toStdString());
        m_commitTimer->start();
    } catch (const Xapian::Error &error) {
        kWarning() << "Exception: " << QString::fromStdString(error.get_msg());
    }
}

void Database::dirty(const QString &path)
{
    kDebug() << path;
    QFileInfo fileInfo(path);
    if (fileInfo.isDir()) {
        // When new files are added the directory changes
        scanDir(path);
    } else {
        // Update the modified file
        scanFile(fileInfo);
    }
}

void Database::commit()
{
    kDebug();
    m_rwXapianDB->commit();

}
