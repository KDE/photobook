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

#ifndef CATEGORIZEDSOURCESMODEL_H
#define CATEGORIZEDSOURCESMODEL_H

#include <QStandardItemModel>

#include <Solid/StorageAccess>

namespace Solid {
    class Device;
}
class CategorizedSourcesModel : public QStandardItemModel
{
    Q_OBJECT
public:
    enum {
        CategoryId = Qt::UserRole + 1,
        CategorySection,
        CategoryUrl,
        CategoryState,
        CategoryIconName,
        CategoryIsDevice,
        CategoryDeviceNotMounted,
        CategoryFilePath,
        CategoryBusy
    } CategoryRoles;

    explicit CategorizedSourcesModel(QObject *parent = 0);
    void init();

    Q_INVOKABLE void mountDevice(const QString &udi);
    Q_INVOKABLE void ejectDevice(const QString &udi);
    
private Q_SLOTS:
    void deviceAdded(const QString &udi);
    void deviceRemoved(const QString &udi);

    void setMountingState(const QString &udi);
    void setUnmountingState(const QString &udi);
    void setIdleState(Solid::ErrorType error, QVariant errorData, const QString &udi);

private:
    void addDevice(const Solid::Device &device);
    QStandardItem* findDevice(const QString &udi);
};

#endif // CATEGORIZEDSOURCESMODEL_H
