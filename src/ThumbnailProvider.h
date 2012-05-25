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

#ifndef THUMBNAILPROVIDER_H
#define THUMBNAILPROVIDER_H

#include <QDeclarativeImageProvider>
#include <KImageCache>

#include "Importer/Database.h"

class ThumbnailProvider : public QDeclarativeImageProvider
{
public:
    explicit ThumbnailProvider(const QString &path);
    ~ThumbnailProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

private:
    KImageCache *m_thumb32;
    Database *m_xapianDB;
};

#endif // THUMBNAILPROVIDER_H
