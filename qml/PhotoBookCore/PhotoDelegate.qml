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

Package {
    Item {
        id: eventItem
        Package.name: 'event'
        width: 160
        height: 150

        Photo {
            id: eventPhoto
            anchors {
                top: parent.top
                right: parent.right
                left: parent.left
            }
//             width:  150
            height: 130
            thumbSource: thumb
            fillMode: Image.PreserveAspectCrop
            clipPhoto: true

            onClicked: {
                console.log("eventUrl-> "  + url);
                imageModel.setRole(url);
                mainView.state = "photos";
            }

        }
        Text {
            anchors.top: eventPhoto.bottom
            anchors.right: parent.right
            anchors.bottom: parent.bottom
            anchors.left: parent.left
            text: display
            color: "white"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignTop
        }
    }

    Item { 
        id: listItem
        Package.name: 'list'
        width: photosListView.width
        height: photosListView.height
    }

    Item {
        id: gridItem
        Package.name: 'grid'
        width: 160
        height: 150
        
        Photo {
            id: photoWrapper
            state: albumVisualModel.state
            width: 160
            height: 150

            thumbSource: thumb
            imageSource: ""
            hqSourceSizeWidth: photosListView.width
            hqSourceSizeHeight: photosListView.height
            
            fillMode: Image.PreserveAspectFit
            clipPhoto: false
            itemSelected: selected

            onClicked: {
                console.log("filePath -> "  + filePath);
                photosGridView.currentIndex = index;
                photosListView.currentIndex = index;
                
                if (albumVisualModel.state == 'inList') {
//                             listItem.ListView.view.currentIndex = index;
                    albumVisualModel.state = 'inGrid';
                } else {
//                             gridItem.GridView.view.currentIndex = index;
                    albumVisualModel.state = 'inList';
                }
                importVisualModel.model.select(index);
            }
            //             width: parent.paintedWidth
            //             height: parent.paintedHeight
            onToggleSelection: {
                console.log("clicked: " + index);
                importVisualModel.model.setSelected(index, !selected);
            }

//                 MouseArea {
//                     anchors.fill: parent
//                     //drag.target: photoWrapper
//                     property int startX
//                     property int startY
//                     property int lastX
//                     property int lastY
//                     
//                     onPressed: {
// //                         photosGridView.interactive = false
//                         startX = photoWrapper.x
//                         startY = photoWrapper.y
//                         var pos = mapToItem(photosGridView, mouse.x, mouse.y)
//                         lastX = pos.x
//                         lastY = pos.y
//                         photoWrapper.z = 900
//                     }
//                     onPositionChanged: {
//                         if (startX < 0) {
//                             return
//                         }
//                         var pos = mapToItem(photosGridView, mouse.x, mouse.y)
//                         photoWrapper.x += (pos.x - lastX)
//                         photoWrapper.y += (pos.y - lastY)
//                         lastX = pos.x
//                         lastY = pos.y
//                     }
//                     onReleased: {
// //                         photosGridView.interactive = true
//                         photoWrapper.z = 0
//                         if (startX < 0) {
//                             return
//                         }
//                         photoWrapper.x = startX
//                         photoWrapper.y = startY
//                         startX = -1
//                         startY = -1
//                     }
//                     onCanceled: {
//                         photoWrapper.z = 0
//                         if (startX < 0) {
//                             return
//                         }
//                         resourceDelegate.x = startX
//                         resourceDelegate.y = startY
//                         startX = -1
//                         startY = -1
//                     }
//                 }

            states: [
                State {
                    name: 'inGrid'
                    PropertyChanges { target: photosListView; interactive: false }
                    PropertyChanges { target: photosGridView; interactive: true }
                    PropertyChanges { target: photosShade; opacity: 0 }
                    ParentChange { target: photoWrapper; parent: gridItem; x: 0; y: 0 }
                    PropertyChanges {
                        target: photoWrapper
                        imageSource: ""
                    }
                },
                State {
                    name: 'inList'
                    PropertyChanges { target: photosListView; interactive: true }
                    PropertyChanges { target: photosGridView; interactive: false }
                    PropertyChanges { target: photosShade; opacity: 1 }
                    ParentChange {
                        target: photoWrapper; parent: listItem; x: 0; y: 0
                        width: photosListView.width; height: photosListView.height
                    }
                    PropertyChanges {
                        target: photoWrapper
                        imageSource: imageId
                    }
                }
            ]

            transitions: [
                Transition {
                    from: 'inGrid'; to: 'inList'
                    NumberAnimation { properties: "y,opacity"; easing.type: Easing.OutQuad; duration: 300 }
                    SequentialAnimation {
                        PauseAnimation { duration: gridItem.GridView.isCurrentItem ? 0 : 600 }
                        ParentAnimation {
                            target: photoWrapper; via: foreground
                            NumberAnimation {
                                targets: [ photoWrapper ]
                                properties: 'x,y,width,height,opacity'
                                duration: gridItem.GridView.isCurrentItem ? 600 : 1; easing.type: 'OutQuart'
                               }
                        }
                    }

                },
                Transition {
                    from: 'inList'; to: 'inGrid'
                    NumberAnimation { properties: 'opacity'; duration: 250 }
                    ParentAnimation {
                        target: photoWrapper; via: foreground
                        NumberAnimation {
                            targets: [ photoWrapper ]
                            properties: 'x,y,width,height,opacity'
                            duration: gridItem.GridView.isCurrentItem ? 600 : 1; easing.type: 'OutQuart'
                        }
                    }
                }
            ]

        }
    }
}
