#include "ImageProvider.h"

#include <libkexiv2/kexiv2.h>
#include <libkexiv2/kexiv2data.h>

#include <KDebug>

ImageProvider::ImageProvider() :
    QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
{
}

QImage ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QImage image(id);
    QImage ret;

    if (requestedSize.isEmpty()) {
        ret = image;
    } else {
        // Load the requested image, scalling to the
        // requested size
        QSize scaleSize = requestedSize;
        if (scaleSize.height() == 0) {
            scaleSize.setHeight(scaleSize.width());
        } else if (scaleSize.width() == 0) {
            scaleSize.setWidth(scaleSize.height());
        }

        ret = image.scaled(scaleSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    if (size) {
        *size = QSize(image.width(), image.height());
    }

    KExiv2Iface::KExiv2	exiv(id);
    exiv.rotateExifQImage(ret, exiv.getImageOrientation());

    return ret;
}
