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

import QtQuick 1.1
import org.kde.photobook 1.0

Item {
    id: delegate

    signal clicked

    width: delegate.ListView.view.width
    height: 20
    
    Image {
        id: deviceIcon
        height: parent.height
        anchors {
            left: parent.left
            leftMargin: 10
            rightMargin: 3
            verticalCenter: parent.verticalCenter
        }
        smooth: true;
        source: "image://kicon/" + iconName;
        sourceSize.width: width
        fillMode: Image.PreserveAspectFit;
        visible: isDevice
    }

    Text {
        id: descriptionText
        text: display
        elide: Text.ElideRight
        anchors {
            left: isDevice ? deviceIcon.right : parent.left
            leftMargin: isDevice ? 3 : 10
            right: isDevice && !deviceNotMounted ? ejectIcon.left : parent.right
            verticalCenter: parent.verticalCenter
        }
    }
    
    MouseArea {
        anchors.fill: parent
        
        onClicked: {
            albumVisualModel.state = "inGrid";
            delegate.ListView.view.currentIndex = index
            // Sets the model's current role
            if (isDevice) {
                mainView.state = "import";
//                 var deviceIndex;
                for (var deviceIndex = 0; deviceIndex < devicesModel.count; ++deviceIndex) {
                    if (devicesModel.get(deviceIndex).url == url) {
                        importVisualModel.model = devicesModel.get(deviceIndex).model;
                        return;
                    }
                }
                var model = Qt.createQmlObject("import org.kde.photobook 1.0; ImageModel{}", devicesModel);
                model.setDevice(url);
                devicesModel.append({"url": url, "model": model });
                importVisualModel.model = model;
//                 importVisualModel.model = devicesModel.get(devicesModel.count - 1).model;
            } else {
                mainView.state = state;
                clicked(url);
//                 model.role = url;
//                 imageModel.setRole(url);
            }
        }
    }
    
    Image {
        id: ejectIcon
        height: parent.height - 10
        anchors {
            right: parent.right
            rightMargin: 5
            verticalCenter: parent.verticalCenter
        }
        asynchronous: true
        smooth: true;
        source: 'images/eject.png'
        fillMode: Image.PreserveAspectFit;
        visible: isDevice && !deviceNotMounted
        MouseArea {
            anchors.fill: parent
            onClicked: {
                // Eject/Unmount the current device
                if (isDevice && !deviceNotMounted) {
                    categorySourceModel.ejectDevice(url);
                }
            }
        }
    }

    BusyIndicator {
        scale: 0.6
        anchors {
            right: parent.right
            rightMargin: 10
            verticalCenter: parent.verticalCenter
        }
    }
}
