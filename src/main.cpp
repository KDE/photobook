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

#include <QtOpenGL/QGLWidget>
#include <QtGui/QFileSystemModel>
#include <QtDeclarative/QDeclarativeContext>
#include <QtDeclarative/QDeclarativeEngine>
#include <QtDeclarative>

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KGlobalSettings>
#include <KConfig>
#include <KConfigGroup>
#include <KStandardDirs>

#include "qmlapplicationviewer.h"
#include "ImageModel.h"
#include "ThumbnailProvider.h"
#include "KIconProvider.h"
#include "ImageProvider.h"
#include "Importer.h"
#include "Importer/Database.h"
#include "CategorizedSourcesModel.h"

int main(int argc, char **argv)
{
    KAboutData about("photobook",
                     "photobook", // DO NOT change this catalog unless you know it will not break translations!
                     ki18n("PhotoBook"),
                     "0.1",
                     ki18n("PhotoBook"),
                     KAboutData::License_GPL,
                     ki18n("(C) 2012 Daniel Nicoletti"));
    KCmdLineArgs::init(argc, argv, &about);
    KApplication app;

    QmlApplicationViewer view;
    view.setAttribute(Qt::WA_OpaquePaintEvent);
    view.setAttribute(Qt::WA_NoSystemBackground);
    view.viewport()->setAttribute(Qt::WA_OpaquePaintEvent);
    view.viewport()->setAttribute(Qt::WA_NoSystemBackground);

    // TODO we need an option for this, since on old machines OpenGL sucks a bit
    QGLWidget *glWidget = new QGLWidget;
    glWidget->format().setSwapInterval(1);
    view.setViewport(glWidget);

    KConfig config;
    KConfigGroup generalConfig(&config, "General");

    QString picturesPath;
    picturesPath = generalConfig.readEntry("PicturesPath",
                                           KGlobalSettings::picturesPath() + QLatin1String("/PhotoBook Library/"));
    // TODO make sure '/' is always present at the end
    app.setProperty("PicturesPath", picturesPath);

    Database::global()->init(picturesPath, false);

    foreach (const QString &importPath, KGlobal::dirs()->findDirs("module", "imports")) {
        view.engine()->addImportPath(importPath);
    }

    // Image Model
//    ImageModel *model = new ImageModel;
//    model->init();
//    view.rootContext()->setContextProperty("imageModel", model);

    // Categories Model
    CategorizedSourcesModel *categoryModel = new CategorizedSourcesModel(0);
    view.rootContext()->setContextProperty("categorySourceModel", categoryModel);

    // Thumbs provider
    view.engine()->addImageProvider(QLatin1String("thumbs"),
                                    new ThumbnailProvider(picturesPath));

    // Icons provider
    view.engine()->addImageProvider(QLatin1String("kicon"),
                                    new KIconProvider());

    // Image provider
    view.engine()->addImageProvider(QLatin1String("image"),
                                    new ImageProvider());

    qmlRegisterType<ImageModel>("org.kde.photobook", 1, 0, "ImageModel");
    qmlRegisterType<Importer>("org.kde.photobook", 1, 0, "Importer");

    view.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);

    QString file = KGlobal::dirs()->findResource("module", "imports/org/kde/photobook/main.qml" );

    view.setMainQmlFile(file);
    view.showExpanded();

    return app.exec();
}
