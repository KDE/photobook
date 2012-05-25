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

import QtQuick 1.0

Item {
    id: checkBox
     
    width: buttonText.paintedWidth + 30
    height: buttonText.paintedHeight
    

    property alias text: buttonText.text
    property alias fontSize: buttonText.font.pixelSize

    signal clicked
     
    Image {
        id: checkBoxImage
        width: 24
        anchors {
            left: parent.left
            verticalCenter: parent.verticalCenter
            leftMargin: 10
        }
        source: 'images/checkbox-nofocus.png'
    }
    MouseArea {
        id: mouseArea
        anchors.fill: checkBoxImage
        onClicked: {
            checkBox.clicked()
        }
    }

    Text {
        id: buttonText
        anchors {
            left: checkBoxImage.right
        }
     }

//      states: State {
//          name: "pressed"
//          when: mouseArea.pressed == true
//          PropertyChanges { target: button; source: 'images/button-dark.png' }
//          PropertyChanges { target: buttonText; color: "white" }
//      }
 }
