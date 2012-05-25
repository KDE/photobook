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
import "PhotoBookCore"

Item {
    id: mainWindow

    width: 800
    height: 480

    Row {
        anchors.fill: parent
        
        Rectangle {
            id: categoryView
            width: 170
            height: parent.height

            BorderImage {
                z: -1
                anchors {
                    fill: categories
                    leftMargin: -6
                    topMargin: -6
                    rightMargin: -8
                    bottomMargin: -6
                }
                source: 'PhotoBookCore/images/box-shadow.png'
                smooth: false
                border {
                    left: 10
                    right: 15
                    top: 10
                    bottom: 10
                }
            }

            ListModel {
                id: devicesModel
            }
            // Left view
            ListView {
                id: categories
                anchors.fill: parent
                focus: true
                section.property : "section"
                section.criteria : ViewSection.FullString
                section.delegate : Rectangle {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: category.paintedHeight
                    color: "transparent"
                    Text {
                        id: category
                        text: section
                        color: "black"
                        font.bold: true
                        anchors {
                            left: parent.left
                            leftMargin: 5
                            topMargin: 15
                            verticalCenter: parent.verticalCenter
                        }
                    }
                }
                
                model: categorySourceModel
                delegate: CategoriesDelegate {}
                highlight : Rectangle { gradient: Gradient {
                        GradientStop { position: 0.0; color: "#4F6ACD" }
                        GradientStop { position: 0.07; color: "#ABABCD" }
                        GradientStop { position: 1.0; color: "#4F6ACD" }
                    }
                }

                onCurrentIndexChanged: {
                    imageModel.setRole(currentIndex);
                }
            }

            ScrollBar {
                scrollArea: categories; height: categories.height; width: 8
                anchors.right: categories.right
            }
        }

        VisualDataModel {
            id: albumVisualModel
            property string state: 'inGrid'
            model: imageModel
            delegate: PhotoDelegate {}
        }

        VisualDataModel {
            id: importVisualModel
            property string state: 'inGrid'
            delegate: PhotoDelegate {}
        }
        
        Column {
            z: -1
            width: mainWindow.width - categories.width
            height: mainWindow.height
            ImportBar {
                id: toolBar
                z: 1
                width: parent.width
//                 height:60
            }

            Rectangle {
                id: mainView
                color: "#525252"
                width: parent.width
                height: parent.height - toolBar.height
                
                GridView {
                    id: eventsGridView
                    visible: false
                    anchors {
                        fill: parent
                        leftMargin: 10
                    }
                    cellWidth: 160
                    cellHeight: 150
                    focus:  true
                    model: albumVisualModel.parts.event
                    highlight: Rectangle { color: "lightsteelblue"; radius: 5 ; z: -1 }
                    onCurrentIndexChanged: {
                        photosListView.positionViewAtIndex(currentIndex, ListView.Contain);
                        photosListView.currentIndex = currentIndex;
                    }
                    ScrollBar {
                        scrollArea: parent; height: parent.height; width: 10
                        anchors.right: parent.right
                    }
                    ListModel {
                        id: selectionModel
                        signal modelCleared
                    }
                }

                GridView {
                    id: photosGridView
                    anchors {
                        fill: parent
                        leftMargin: 10
                    }
                    cellWidth: 160
                    cellHeight: 150
                    focus:  true
                    model: albumVisualModel.parts.grid
                    highlight: Rectangle { color: "lightsteelblue"; radius: 5 ; z: -1 }
                    onCurrentIndexChanged: {
                        photosListView.positionViewAtIndex(currentIndex, ListView.Contain);
                        photosListView.currentIndex = currentIndex;
                    }
                    ScrollBar {
                        scrollArea: parent; height: parent.height; width: 10
                        anchors.right: parent.right
                    }
                }

                Rectangle {
                    id: photosShade
                    color: '#525252'
                    anchors.fill: parent
                    opacity: 0
                }

                ListView {
                    id: photosListView
                    anchors.fill: parent
                    orientation: ListView.Horizontal
                    snapMode: ListView.SnapOneItem
                    model: albumVisualModel.parts.list
                    interactive: false
                    onCurrentIndexChanged: {
                        photosGridView.positionViewAtIndex(currentIndex, GridView.Contain);
                        photosGridView.currentIndex = currentIndex;
                    }
                    preferredHighlightBegin: parent.top.y
                    preferredHighlightEnd: parent.bottom.y
                    highlightFollowsCurrentItem: false
                }

                GridView {
                    id: importGridView
                    anchors {
                        fill: parent
                        leftMargin: 10
                    }
                    cellWidth: 160
                    cellHeight: 150
                    focus:  true
                    model: importVisualModel.parts.grid
                    onCurrentIndexChanged: {
                        photosListView.positionViewAtIndex(currentIndex, ListView.Contain);
                        photosListView.currentIndex = currentIndex;
                    }
                    ScrollBar {
                        scrollArea: parent; height: parent.height; width: 10
                        anchors.right: parent.right
                    }
                }
                
                states: [
                    State {
                        name: 'events'
                        PropertyChanges {
                            target: eventsGridView
                            visible: true
                        }
                        PropertyChanges {
                            target: photosGridView
                            visible: false
                        }
                        PropertyChanges {
                            target: importGridView
                            visible: false
                        }
                    },
                    State {
                        name: 'photos'
                        PropertyChanges {
                            target: eventsGridView
                            visible: false
                        }
                        PropertyChanges {
                            target: photosGridView
                            visible: true
                        }
                        PropertyChanges {
                            target: importGridView
                            visible: false
                        }
                    },
                    State {
                        name: 'import'
                        PropertyChanges {
                            target: eventsGridView
                            visible: false
                        }
                        PropertyChanges {
                            target: photosGridView
                            visible: false
                        }
                        PropertyChanges {
                            target: importGridView
                            visible: true
                        }
                    }
                ]

                Item { id: foreground; anchors.fill: parent }

            }
        }
    }
}
