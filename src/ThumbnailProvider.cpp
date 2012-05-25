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

#include "ThumbnailProvider.h"

#include <KDebug>
#include <QThread>
#include <xapian.h>

#include <libkexiv2/kexiv2.h>
#include <libkexiv2/kexiv2previews.h>

ThumbnailProvider::ThumbnailProvider(const QString &path) :
    QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
{
    m_thumb32 = new KImageCache(QLatin1String("PhotoBookThumbs4.cache"), 500000000, 200000);
    m_xapianDB = new Database;
    m_xapianDB->init(path, true);
    kDebug() <<  QThread::currentThreadId();
}

ThumbnailProvider::~ThumbnailProvider()
{
    delete m_thumb32;
    delete m_xapianDB;
}

QImage ThumbnailProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QImage image;

    if (m_thumb32->findImage(id, &image)) {
        return image;
    } else {
        QString filePath;
        if (id.at(0) == QLatin1Char('Q')) {
            Xapian::Document doc = m_xapianDB->findDocument(id);
            if (doc.get_docid() == 0) {
                return image;
            } else {
                filePath = QString::fromStdString(doc.get_value(Database::FilePath));
            }
        } else {
            filePath = id;
        }

        // Load thumbnail
//        KExiv2Iface::KExiv2Previews preview(filePath);
        KExiv2Iface::KExiv2	preview(filePath);
        image = preview.getExifThumbnail(true);
        if (image.isNull()) {
//            image = preview.image();
//        } else {
            // Store thumbnail
            // TODO smooth or fast?
            image = QImage(filePath).scaled(160, 120, Qt::KeepAspectRatio);
//            preview.
            kWarning() << "Could not find preview image for" << filePath << image.isNull();
        }

        // Store the thumbnail into the cache file
        if (m_thumb32->insertImage(id, image)) {
            kWarning() << "Added preview for" << image.byteCount() << filePath << id;
        } else {
            kWarning() << "FAILED to add preview for" << filePath << id;
        }
    }

    return image;
}
