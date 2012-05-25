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

Rectangle {
     id: toolBar
     color: "white"

     signal clicked
     height: 60

     Button {
         id: button1
         x: 165
         y: 27
         width: 116
         height: 33
         label: "Import All..."
     }

     Text {
         id: text1
         x: 17
         y: 11
         width: 120
         height: 16
         text: qsTr("Device Name")
         font.pixelSize: 12
     }

     TextInput {
         id: eventBaseName
         x: 29
         y: 34
         width: 97
         height: 20
         text: qsTr("Event Name")
         font.pixelSize: 12
     }

//      clip: true

 }
