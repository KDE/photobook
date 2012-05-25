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

Item {
    id: photoWrapper
    
    property alias thumbSource: thumbImage.source
    property alias imageSource: hqImage.source

    property alias hqSourceSizeWidth: hqImage.sourceSize.width
    property alias hqSourceSizeHeight: hqImage.sourceSize.height

    property alias fillMode: thumbImage.fillMode
    // We need image cliping for the events otherwise it won't crop
    property alias clipPhoto: thumbImage.clip

    property bool itemSelected

    signal toggleSelection()
    signal clicked()

    BorderImage {
        anchors {
            fill: border
            leftMargin: -6
            topMargin: -6
            rightMargin: -6
            bottomMargin: -6
        }
        source: itemSelected ? 'images/box-shadow-selected.png' : 'images/box-shadow.png'
        smooth: false
        border {
            left: 10
            right: 10
            top: 10
            bottom: 10
        }
    }
    MouseArea {
        id: mouseArea
        anchors.fill: border
        hoverEnabled: true
    }
    Rectangle {
        id: border
        anchors.centerIn: thumbImage
        smooth: false
        width: (thumbImage.clip ? thumbImage.width : thumbImage.paintedWidth) + 3
        height: (thumbImage.clip ? thumbImage.height : thumbImage.paintedHeight) + 3
        visible: false
    }
    BusyIndicator {
        anchors.centerIn: parent
        on: thumbImage.status != Image.Ready
    }
    Image {
        id: thumbImage
        anchors {
            centerIn: parent
            fill: parent
            margins: 10
        }
        asynchronous: true
        smooth: true
        source: thumb
        clip: true
        visible: thumbImage.status == Image.Ready
    }
    MouseArea {
        id: selectArea
        anchors.fill: selectIcon
        hoverEnabled: true
        z:1
    }
    Component.onCompleted: {
        selectArea.clicked.connect(toggleSelection);
        mouseArea.clicked.connect(clicked)
    }
    Image {
        id: selectIcon
        anchors {
            left: border.left
            top: border.top
            margins: 5
        }
        asynchronous: true
        smooth: true
        source: itemSelected ? 'images/minus.png' : 'images/plus.png'
        opacity: selectArea.containsMouse ? 1 : (itemSelected || mouseArea.containsMouse ? 0.8 : 0)
        Behavior on opacity {
            NumberAnimation { duration: 300 }
        }
        
    }
    Image {
        id: hqImage
        anchors {
            centerIn: parent
            fill: parent
            margins: 10
        }
        asynchronous: true
        fillMode: Image.PreserveAspectFit
    }
}
