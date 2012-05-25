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
    id: lineEdit

    property alias text: editor.text
    property alias placeholderText: placeholder.text
    signal accepted()

    BorderImage {
        anchors.fill: parent
        anchors.margins: -2
        source: editor.focus ? 'images/lineedit-focus.sci' : 'images/lineedit.sci'
    }
    
    TextInput {
        id: editor
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: 4
            rightMargin: 4
            verticalCenter: parent.verticalCenter
        }
        selectByMouse: true
        selectionColor: "lightsteelblue"
    }
    Text {
        id: placeholder
        anchors {
            left: parent.left
            right: parent.right
            leftMargin: 4
            rightMargin: 4
            verticalCenter: parent.verticalCenter
        }
        opacity: 0.7
        visible: editor.text == "" && editor.focus == false
    }
    
    Keys.forwardTo: [ (returnKey), (editor)]
    
    Item {
        id: returnKey
        Keys.onReturnPressed: lineEdit.accepted()
        Keys.onEnterPressed: lineEdit.accepted()
        Keys.onEscapePressed: editor.focus = false
    }
}
