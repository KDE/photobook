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

#ifndef IMAGEMODEL_H
#define IMAGEMODEL_H

#include <QStandardItemModel>
#include <QString>

#include <Solid/StorageAccess>

class ImageRequest;
class ImageFile;
class Event;
class ImageModel : public QStandardItemModel
{
    Q_OBJECT
    Q_PROPERTY(QString role READ role WRITE setRole)
    Q_PROPERTY(QString device READ device WRITE setDevice)
    Q_PROPERTY(QString uuid READ uuid)
    Q_PROPERTY(QStringList selected READ selected)
public:
    enum {
        ImageId = Qt::UserRole + 1,
        ThumbImage,
        Selected,
        FilePath,
        Url
    } ImageRoles;
    explicit ImageModel(QObject *parent = 0);

    void init();

    QString role() const;
    QString device() const;
    QString uuid() const;
    QStringList selected() const;

    Q_INVOKABLE void setRole(const QString &role);
    Q_INVOKABLE void setDevice(const QString &udi);
    Q_INVOKABLE void setSelected(int index, bool selected);
    Q_INVOKABLE void clearSelection();
    Q_INVOKABLE void importSelected(const QString &eventName, bool splitEvents);

private Q_SLOTS:
    void setIdleState(Solid::ErrorType error, QVariant errorData, const QString &udi);
    void file(const ImageFile &file);
    void eventItem(const Event &event);
    void getFilesFinished();
    void mountDevice(const QString &udi);

private:
    QString m_udi;
    QString m_uuid;
    QString m_role;
    ImageRequest *m_imgRequest;
};

#endif // IMAGEMODEL_H
