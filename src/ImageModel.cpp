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

#include "ImageModel.h"

#include "Importer/Database.h"
#include "ImageRequest.h"

#include <QStringBuilder>

#include <Solid/DeviceNotifier>
#include <Solid/Device>
#include <Solid/StorageAccess>
#include <Solid/StorageDrive>
#include <Solid/OpticalDrive>
#include <Solid/OpticalDisc>

#include <KDebug>

ImageModel::ImageModel(QObject *parent) :
    QStandardItemModel(parent),
    m_imgRequest(0)
{
    QHash<int, QByteArray> roles = roleNames();
    roles[ImageId] = "imageId";
    roles[ThumbImage] = "thumb";
    roles[Selected] = "selected";
    roles[FilePath] = "filePath";
    roles[Url] = "url";
    setRoleNames(roles);
}

QString ImageModel::role() const
{
    return m_role;
}

void ImageModel::setRole(const QString &role)
{
    kDebug() << role << rowCount();
    m_role = role;
    if (rowCount()) {
        // Remove all current rows
        removeRows(0, rowCount());
    }

    kDebug() << "m_imgRequest " << m_imgRequest;
    if (m_imgRequest) {
        m_imgRequest->disconnect(this, SLOT(file(ImageFile)));
        m_imgRequest->quit();
    }

    m_imgRequest = new ImageRequest(this);
    m_imgRequest->setRequest(role);
    connect(m_imgRequest, SIGNAL(file(ImageFile)), this, SLOT(file(ImageFile)), Qt::QueuedConnection);
    connect(m_imgRequest, SIGNAL(finished()), this, SLOT(getFilesFinished()), Qt::QueuedConnection);
    connect(m_imgRequest, SIGNAL(eventItem(Event)), this, SLOT(eventItem(Event)), Qt::QueuedConnection);
    m_imgRequest->start();


//    if (role == QLatin1String("localFiles")) {
//        kDebug() << "localFiles" << m_db->notImported().size();
//        foreach (const QString &file, m_db->notImported()) {
//            QFileInfo fileInfo(file);
//            QStandardItem *it = new QStandardItem();
//            it->setData(file, ImageId);
//            it->setData(fileInfo.fileName(), Qt::DisplayRole);
//            it->setData("thumb2", ThumbImage);
//            it->setData(file, FilePath);
//            appendRow(it);
//        }
//        kDebug() << "localFiles" << m_db->notImported().size();
//    }


}

QString ImageModel::device() const
{
    return m_udi;
}

QString ImageModel::uuid() const
{
    return m_uuid;
}

QStringList ImageModel::selected() const
{
    QStringList ret;
    for (int i = 0; i < rowCount(); ++i) {
        QStandardItem *it = item(i);
        if (it->data(Selected).toBool()) {
            ret << it->data(FilePath).toString();
        }
    }
    return ret;
}

void ImageModel::setDevice(const QString &udi)
{
    kDebug() << udi;
    m_udi = udi;

    Solid::Device device(udi);
    const Solid::StorageAccess *access = device.as<Solid::StorageAccess>();
    if (access) {
        connect(access, SIGNAL(setupDone(Solid::ErrorType,QVariant,QString)),
                this, SLOT(setIdleState(Solid::ErrorType,QVariant,QString)));
        connect(access, SIGNAL(teardownDone(Solid::ErrorType,QVariant,QString)),
                this, SLOT(setIdleState(Solid::ErrorType,QVariant,QString)));

        if (access->isAccessible()) {
            // is mounted
            setRole(access->filePath());
        } else {
            mountDevice(udi);
        }
    }

    // Get the volume uuid
    const Solid::StorageVolume *volume = device.as<Solid::StorageVolume>();
    if (volume) {
        m_uuid = volume->uuid();
    } else {
        m_uuid.clear();
    }
}

void ImageModel::setSelected(int index, bool selected)
{
    QStandardItem *it = item(index);
    kDebug() << index << it->data(Selected).toBool() << selected;
    if (it) {
        // Change the item selection
        it->setData(selected, Selected);
    }
}

void ImageModel::clearSelection()
{
    // Unselect all items
    for (int i = 0; i < rowCount(); ++i) {
        item(i)->setData(false, Selected);
    }
}

void ImageModel::importSelected(const QString &eventName, bool splitEvents)
{
}

void ImageModel::file(const ImageFile &file)
{
    QFileInfo fileInfo(file.filePath);
    QStandardItem *it = new QStandardItem();
    it->setData(static_cast<QString>(QLatin1String("image://image/") % file.filePath),
                ImageId);
    it->setData(fileInfo.fileName(), Qt::DisplayRole);
    it->setData(static_cast<QString>(QLatin1String("image://thumbs/") % file.filePath),
                ThumbImage);
    it->setData(file.filePath, FilePath);
    it->setData(false, Selected);
    appendRow(it);
}

void ImageModel::eventItem(const Event &event)
{
    QStandardItem *it = new QStandardItem();
    it->setData(static_cast<QString>(QLatin1String("image://image/") % event.cover.filePath),
                ImageId);
    it->setData(event.name, Qt::DisplayRole);
    it->setData(static_cast<QString>(QLatin1String("image://thumbs/") % event.cover.filePath),
                ThumbImage);
    it->setData(static_cast<QString>(QLatin1String("events/") % event.name),
                Url);
//    it->setData(file.filePath, FilePath);
    it->setData(false, Selected);
    appendRow(it);
}

void ImageModel::getFilesFinished()
{
    kDebug();
//    m_imgRequest->deleteLater();
    m_imgRequest = 0;
}

void ImageModel::mountDevice(const QString &udi)
{
    Solid::Device device(udi);
    Solid::StorageAccess *access = device.as<Solid::StorageAccess>();
    if (access && !access->isAccessible()) {
        access->setup();
    } else {
        kWarning() << "Device has no access" << udi;
    }
}

void ImageModel::init()
{
    kDebug() << "m_imgRequest " << m_imgRequest;
//    Xapian::Query query(Xapian::Query::OP_VALUE_GE, Database::Rating, "0");
//    Xapian::ValueCountMatchSpy spy0(Database::Rating);
//    Xapian::Enquire enq(*m_db->database());
//    enq.add_matchspy(&spy0);

//    enq.set_query(query);

//    Xapian::MSet mset = enq.get_mset(0, 10, 10000);

//    Xapian::TermIterator i;
//    for (i = spy0.values_begin(); i != spy0.values_end(); ++i) {
//        kDebug() << QString::fromStdString(*i) << ": " << QString::number(i.get_termfreq());
//    }
}

void ImageModel::setIdleState(Solid::ErrorType error, QVariant errorData, const QString &udi)
{
    kDebug() << error << errorData << udi;
    Solid::Device device(udi);
    const Solid::StorageAccess *access = device.as<Solid::StorageAccess>();
    if (access && access->isAccessible()) {
        // The device is now mounted, load it's images
        setRole(access->filePath());
    } else if (rowCount()) {
        // The device was unmount, remove items
        removeRows(0, rowCount());
    }
}
