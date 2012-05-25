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

Image {
     id: button
     
     width: buttonText.paintedWidth + 30
     height: buttonText.paintedHeight + 20

     property alias text: buttonText.text
     property alias fontSize: buttonText.font.pixelSize

     signal clicked

     source: 'images/button-light.png'
//      clip: true

     Text {
         id: buttonText
         anchors.centerIn: parent; anchors.verticalCenterOffset: -1
//          style: Text.Sunken
         color: "black"
//          styleColor: "white"
         smooth: true
     }

     MouseArea {
         id: mouseArea
         anchors.fill: parent
         onClicked: {
             button.clicked()
         }
     }

     states: State {
         name: "pressed"
         when: mouseArea.pressed == true
         PropertyChanges { target: button; source: 'images/button-dark.png' }
         PropertyChanges { target: buttonText; color: "white" }
     }
 }
