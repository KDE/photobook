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

Rectangle {
    id: toolBar
    color: "white"

    property int photosCount: 0

    signal clicked
    height: 40

    LineEdit {
        id: lineEdit
        anchors {
            left: parent.left
            right: checkBox.left
            verticalCenter: parent.verticalCenter
            leftMargin: 15
        }
        height: 16
        placeholderText: "Event name"
    }
    
    CheckBox {
        id: checkBox
        anchors {
            right: onlySelectedBt.left
            verticalCenter: parent.verticalCenter
            leftMargin: 15
        }
        height: 16
        text: "Split events"
    }

    Button {
        id: onlySelectedBt
        anchors {
            right: importAllBt.left
            verticalCenter: parent.verticalCenter
        }
        text: "Import Selected"
        onClicked: {
            var model = importVisualModel.model;
            importer.importFromFileList(model.uuid, model.selected, lineEdit.text, true);
        }
    }

    Button {
        id: importAllBt
        anchors {
            right: parent.right
            verticalCenter: parent.verticalCenter
        }
        text: "Import " + photosCount + " Photos"
    }

    Importer {
        id: importer
    }
}
