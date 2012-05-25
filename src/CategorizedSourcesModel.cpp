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

#include "CategorizedSourcesModel.h"

#include <Solid/DeviceNotifier>
#include <Solid/Device>
#include <Solid/StorageDrive>
#include <Solid/OpticalDrive>
#include <Solid/OpticalDisc>

#include <KLocale>

#include <KDebug>

using namespace Solid;

#define COLLECTION "C"
#define RECENT     "R"
#define DEVICES    "D"
#define ALBUMS     "A"

CategorizedSourcesModel::CategorizedSourcesModel(QObject *parent) :
    QStandardItemModel(parent)
{
    QHash<int, QByteArray> roles = roleNames();
    roles[CategoryId] = "role";
    roles[CategorySection] = "section";
    roles[CategoryUrl] = "url";
    roles[CategoryIconName] = "iconName";
    roles[CategoryIsDevice] = "isDevice";
    roles[CategoryDeviceNotMounted] = "deviceNotMounted";
    roles[CategoryFilePath] = "filePath";
    roles[CategoryBusy] = "busy";
    setRoleNames(roles);

    init();

    connect(DeviceNotifier::instance(), SIGNAL(deviceAdded(QString)),
            this, SLOT(deviceAdded(QString)));
    connect(DeviceNotifier::instance(), SIGNAL(deviceRemoved(QString)),
            this, SLOT(deviceRemoved(QString)));
    foreach (const Device &device, Device::allDevices()) {
        addDevice(device);
    }
}

void CategorizedSourcesModel::init()
{
    QStandardItem *it;

    // Events item
    it = new QStandardItem();
    it->setData(i18n("Events"), Qt::DisplayRole);
    it->setData(i18n("Collection"), CategorySection);
    it->setData(COLLECTION, CategoryId);
    it->setData("events", CategoryUrl);
    it->setData(false, CategoryIsDevice);
    appendRow(it);

    // All Photos Item
    it = new QStandardItem();
    it->setData(i18n("All Photos"), Qt::DisplayRole);
    it->setData(i18n("Collection"), CategorySection);
    it->setData(COLLECTION, CategoryId);
    it->setData("allphotos", CategoryUrl);
    it->setData(false, CategoryIsDevice);
    appendRow(it);

    // Faces Item
    it = new QStandardItem();
    it->setData(i18n("Faces"), Qt::DisplayRole);
    it->setData(i18n("Collection"), CategorySection);
    it->setData(COLLECTION, CategoryId);
    it->setData("faces", CategoryUrl);
    it->setData(false, CategoryIsDevice);
    appendRow(it);

    // Places Item
    it = new QStandardItem();
    it->setData(i18n("Places"), Qt::DisplayRole);
    it->setData(i18n("Collection"), CategorySection);
    it->setData(COLLECTION, CategoryId);
    it->setData("places", CategoryUrl);
    it->setData(false, CategoryIsDevice);
    appendRow(it);
}

void CategorizedSourcesModel::mountDevice(const QString &udi)
{
    Device device(udi);
    Solid::StorageAccess *access = device.as<Solid::StorageAccess>();
    if (access && !access->isAccessible()) {
        access->setup();
    } else {
        kWarning() << "Device has no access" << udi;
    }
}

void CategorizedSourcesModel::ejectDevice(const QString &udi)
{
    Device device(udi);
    if (device.is<Solid::OpticalDisc>()) {
        Solid::OpticalDrive *drive = device.parent().as<Solid::OpticalDrive>();
        if (drive) {
            drive->eject();
        }
    } else if (device.is<Solid::StorageAccess>()) {
        Solid::StorageAccess *access = device.as<Solid::StorageAccess>();
        if (access && access->isAccessible()) {
            access->teardown();
        }
    }
}

void CategorizedSourcesModel::deviceAdded(const QString &udi)
{
//    kDebug() << udi;
    addDevice(Device(udi));
}

void CategorizedSourcesModel::deviceRemoved(const QString &udi)
{
    kDebug() << udi;
    QStandardItem *it = findDevice(udi);
    if (it) {
        removeRow(it->row());
    }
}

void CategorizedSourcesModel::setMountingState(const QString &udi)
{
    kDebug() << udi;
    QStandardItem *it = findDevice(udi);
    if (it) {
        it->setData(true, CategoryBusy);
    }
}

void CategorizedSourcesModel::setUnmountingState(const QString &udi)
{
    kDebug() << udi;
    QStandardItem *it = findDevice(udi);
    if (it) {
        it->setData(true, CategoryBusy);
    }
}

void CategorizedSourcesModel::setIdleState(Solid::ErrorType error, QVariant errorData, const QString &udi)
{
    kDebug() << error << errorData << udi;
    QStandardItem *it = findDevice(udi);
    if (it) {
        it->setData(false, CategoryBusy);
        Device device(udi);
        const Solid::StorageAccess *access = device.as<Solid::StorageAccess>();
        if (access) {
            it->setData(!access->isAccessible(), CategoryDeviceNotMounted);
            //if (access->isAccessible()) {
            //    it->setData(access->filePath(), CategoryFilePath);
            //}
        }
    }
}

void CategorizedSourcesModel::addDevice(const Device &device)
{
    if (device.isValid()) {
        bool removable = false;
        bool notMounted = true;
        if (device.isDeviceInterface(DeviceInterface::Camera)) {
            // TODO
            kWarning() << device.udi() << "Wow found a camera! can I just add it?";
        } else if (device.isDeviceInterface(DeviceInterface::OpticalDisc)) {
            // TODO
            removable = true;
        } else if (device.isDeviceInterface(DeviceInterface::SmartCardReader)) {
            // TODO
            kWarning() << device.udi() << "Wow found a SmartCardReader os thuis the storage volume?";
        } else if (device.isDeviceInterface(DeviceInterface::StorageVolume)) {
            // TODO
            kWarning() << device.udi() << "Wow found a StorageVolume os thuis the storage volume?";
            kDebug() << "Parent" << device.parent().udi();
            if (device.parent().is<Solid::StorageDrive>()) {
                const Solid::StorageDrive *storagedrive = device.parent().as<Solid::StorageDrive>();
                if (!storagedrive) {
                    return;
                }

                kWarning() << "REMOVABLE?? " << storagedrive->isRemovable();
                removable = storagedrive->isRemovable();
            }



        }

        // Only removable media, if the user want's to import files
        // from the computer there will be a File -> Import Images
        if (removable) {
            const Solid::StorageAccess *access = device.as<Solid::StorageAccess>();
            if (access) {
                // We should ignore this device
                if (access->isIgnored()) {
                    return;
                }
                connect(access, SIGNAL(setupRequested(QString)),
                        this, SLOT(setMountingState(QString)));
                connect(access, SIGNAL(setupDone(Solid::ErrorType,QVariant,QString)),
                        this, SLOT(setIdleState(Solid::ErrorType,QVariant,QString)));
                connect(access, SIGNAL(teardownRequested(QString)),
                        this, SLOT(setUnmountingState(QString)));
                connect(access, SIGNAL(teardownDone(Solid::ErrorType,QVariant,QString)),
                        this, SLOT(setIdleState(Solid::ErrorType,QVariant,QString)));

                if (access->isAccessible()) {
                    notMounted = false;
                }
            }

            QStandardItem *it = new QStandardItem();
            it->setData(device.description(), Qt::DisplayRole);
            it->setData(device.icon(), Qt::DecorationRole);
            it->setData(i18n("Devices"), CategorySection);
            it->setData(DEVICES, CategoryId);
            it->setData(device.udi(), CategoryUrl);
            it->setData(device.icon(), CategoryIconName);
            it->setData(true, CategoryIsDevice);
            it->setData(notMounted, CategoryDeviceNotMounted);
            if (access->isAccessible()) {
                it->setData(access->filePath(), CategoryFilePath);
            }

            appendRow(it);
            kDebug() << "Added:" << device.udi() << device.vendor();
        }
    }
}

QStandardItem *CategorizedSourcesModel::findDevice(const QString &udi)
{
    for (int i = 0; i < rowCount(); ++i) {
        QStandardItem *it = item(i);
        if (it->data(CategoryIsDevice).toBool() &&
                it->data(CategoryUrl).toString() == udi) {
            return it;
            break;
        }
    }
    return 0;
}
