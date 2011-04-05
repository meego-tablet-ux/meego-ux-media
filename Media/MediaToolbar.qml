/*
 * Copyright 2011 Intel Corporation.
 *
 * This program is licensed under the terms and conditions of the
 * Apache License, version 2.0.  The full text of the Apache License is at 	
 * http://www.apache.org/licenses/LICENSE-2.0
 */

import Qt 4.7
import MeeGo.Labs.Components 0.1
import MeeGo.Media 0.1

Item {
    id: container
    width: background.width
    height: background.height

    property bool landscape: false

    property bool showprev: false
    signal prevPressed()

    property bool showrecord: false
    property bool disablerecord: false
    property bool ispauserecord: false
    signal recordPressed()
    signal pauseRecordPressed()

    property bool showstop: false
    property bool disablestop: false
    signal stopPressed()

    property bool showplay: false
    property bool disableplay: false
    property bool ispause: true
    signal playPressed()
    signal pausePressed()

    property bool shownext: false
    signal nextPressed()

    property bool showprogressbar: false
    property real sliderPosition: 0
    property alias elapsedTimeText: elapsedTime.text
    property alias remainingTimeText: remainingTime.text
    signal sliderMoved(real pos)

    property bool showvolume: false
    property variant volumeParent: container.parent

    property bool showfavorite: false
    property bool isfavorite: false
    signal favoritePressed()

    property bool showshuffle: false
    property bool isshuffle: false
    signal shufflePressed()

    property bool showrepeat: false
    property bool isrepeat: false
    signal repeatPressed()

    property int buttoncount: showprev + showrecord + showstop + showplay + shownext + showvolume + showfavorite + showshuffle + showrepeat
    property int buttonwidth: (buttoncount > 0)?(background.width/buttoncount):background.width

    VolumeControl {
        id: volumeControl
    }

    Loader {
        id: volumeLoader
    }

    Component {
        id: volumeControlComponent
        VolumeSlider{
            onClose: {
                volumeLoader.sourceComponent = undefined;
            }
        }
    }

    Image {
        id: background
        anchors.fill: parent
        source: "image://meegotheme/widgets/common/action-bar/action-bar-background"
        MediaToolbarButton {
            id: btRewind
            show: showprev
            anchors.left: parent.left
            anchors.top: parent.top
            height: parent.height
            width: (show)?((showprogressbar)?iwidth:buttonwidth):0
            bgSourceUp: "image://meegotheme/icons/actionbar/media-backward"
            bgSourceDn: "image://meegotheme/icons/actionbar/media-backward-active"
            onClicked: container.prevPressed()
        }
        MediaToolbarButton {
            id: btRecord
            show: showrecord
            anchors.left: btRewind.right
            anchors.top:parent.top
            height: parent.height
            width: (show)?((showprogressbar)?iwidth:buttonwidth):0
            bgSourceUp: (disablerecord)?"image://meegotheme/icons/actionbar/media-record-disabled":
                    ((ispauserecord)?"image://meegotheme/icons/actionbar/media-pause":"image://meegotheme/icons/actionbar/media-record")
            bgSourceDn: (disablerecord)?"image://meegotheme/icons/actionbar/media-record-disabled":
                    ((ispauserecord)?"image://meegotheme/icons/actionbar/media-pause-active":"image://meegotheme/icons/actionbar/media-record-active")
            onClicked: {
                if(!disablerecord)
                {
                    if (ispauserecord)
                        container.pauseRecordPressed();
                    else
                        container.recordPressed();
                }
            }
        }
        MediaToolbarButton {
            id: btStop
            show: showstop
            anchors.left: btRecord.right
            anchors.top:parent.top
            height: parent.height
            width: (show)?((showprogressbar)?iwidth:buttonwidth):0
            bgSourceUp: (disablestop)?"image://meegotheme/icons/actionbar/media-stop-disabled":"image://meegotheme/icons/actionbar/media-stop"
            bgSourceDn: (disablestop)?"image://meegotheme/icons/actionbar/media-stop-disabled":"image://meegotheme/icons/actionbar/media-stop-active"
            onClicked: {
                if(!disablestop)
                    container.stopPressed();
            }
        }
        MediaToolbarButton {
            id: btPlay
            show: showplay
            anchors.left: btStop.right
            anchors.top:parent.top
            height: parent.height
            width: (show)?((showprogressbar)?iwidth:buttonwidth):0
            bgSourceUp: (disableplay)?"image://meegotheme/icons/actionbar/media-play":
                    ((ispause)?"image://meegotheme/icons/actionbar/media-pause":"image://meegotheme/icons/actionbar/media-play")
            bgSourceDn: (disableplay)?"image://meegotheme/icons/actionbar/media-play":
                    ((ispause)?"image://meegotheme/icons/actionbar/media-pause-active":"image://meegotheme/icons/actionbar/media-play-active")
            onClicked: {
                if(!disableplay)
                {
                    if (ispause)
                        container.pausePressed();
                    else
                        container.playPressed();
                }
            }
        }
        MediaToolbarButton {
            id: btForward
            show: shownext
            anchors.left: btPlay.right
            anchors.top:parent.top
            height: parent.height
            width: (show)?((showprogressbar)?iwidth:buttonwidth):0
            bgSourceUp: "image://meegotheme/icons/actionbar/media-forward"
            bgSourceDn: "image://meegotheme/icons/actionbar/media-forward-active"
            onClicked: container.nextPressed()
        }
        Item {
            id: progressBarItem
            visible: showprogressbar
            anchors.left: btForward.right
            anchors.leftMargin: 20
            anchors.right: btVolume.left
            anchors.top:parent.top
            height: parent.height

            Item {
                id: elapsedTimeRect
                anchors.left: parent.left
                anchors.top: parent.top
                width: elapsedTime.font.pixelSize * 2
                height: parent.height
                Text {
                    id: elapsedTime
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment:Text.AlignVCenter
                    color: theme_fontColorMediaHighlight
                    text:"0:00"
                }
            }

            Item {
                id: progressBarRect
                anchors.left :elapsedTimeRect.right
                anchors.leftMargin: 20
                anchors.rightMargin: 20
                anchors.right:remainingTimeRect.left
                anchors.bottom: parent.bottom
                height:parent.height
                Image {
                    id: progressBar
                    anchors.right: parent.right
                    anchors.left: parent.left
                    anchors.verticalCenter:parent.verticalCenter
                    source: "image://theme/playhead_bg"
                }
                Image {
                    id: progressBarSlider
                    anchors.verticalCenter:progressBar.verticalCenter
                    source:"image://theme/scrub_head_lrg"
                    x: -width/2
                    z:10
                }
                Image {
                    id: elapsedHead
                    source: "image://theme/media/progress_fill_1"
                    anchors.left: progressBar.left
                    anchors.verticalCenter:progressBar.verticalCenter
                    z:1
                }
                BorderImage {
                    id: elapsedBody
                    source: "image://theme/media/progress_fill_2"
                    anchors.left: elapsedHead.right
                    anchors.right: elapsedTail.left
                    anchors.verticalCenter:progressBar.verticalCenter
                   // width: 100
                    border.left: 1; border.top: 1
                    border.right: 1; border.bottom: 1
                    z:1
                }
                Image {
                    id: elapsedTail
                    source: "image://theme/media/progress_fill_3"
                    anchors.right: progressBarSlider.right
                    anchors.rightMargin: progressBarSlider.width/2
                    anchors.verticalCenter:progressBar.verticalCenter
                    z:1
                }

                Connections {
                    id: progressBarConnection
                    target: container
                    onSliderPositionChanged: {
                        progressBarSlider.x = (sliderPosition * progressBar.width) - progressBarSlider.width/2;
                    }
                }
                MouseArea {
                    anchors.fill: parent
                    onPositionChanged: {
                        progressBarConnection.target = null
                        var pos = mouseX;
                        pos = (pos < 0)?0:pos;
                        pos = (pos > progressBar.width)?progressBar.width:pos;
                        progressBarSlider.x = pos - (progressBarSlider.width/2);
                    }
                    onReleased: {
                        sliderPosition = (progressBarSlider.x + (progressBarSlider.width/2))/(progressBar.width);
                        container.sliderMoved(sliderPosition);
                        progressBarConnection.target = container
                    }
                }
            }
            Item {
                id: remainingTimeRect
                anchors.right: divider.left
                anchors.rightMargin: 20
                anchors.top: parent.top
                width: remainingTime.font.pixelSize * 2
                height: parent.height

                Text {
                    id: remainingTime
                    anchors.fill: parent
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment:Text.AlignVCenter
                    color: theme_fontColorMediaHighlight
                    text: "0:00"
                }
            }
            Image{
                id: divider
                anchors.right: parent.right
                source: "image://meegotheme/widgets/common/action-bar/action-bar-separator"
                height: parent.height
            }
        }
        MediaToolbarButton {
            id: btVolume
            show: showvolume
            anchors.right: favoriteButton.left
            anchors.top: parent.top
            height: parent.height
            width: (show)?((showprogressbar)?iwidth:buttonwidth):0
            bgSourceUp: (volumeControl.volume < 2)?"image://meegotheme/icons/actionbar/media-volume-mute":
                        (volumeControl.volume < 33)?"image://meegotheme/icons/actionbar/media-volume-1":
                        (volumeControl.volume < 67)?"image://meegotheme/icons/actionbar/media-volume-2":
                        "image://meegotheme/icons/actionbar/media-volume-max"
            bgSourceDn: (volumeControl.volume < 2)?"image://meegotheme/icons/actionbar/media-volume-mute-active":
                        (volumeControl.volume < 33)?"image://meegotheme/icons/actionbar/media-volume-1-active":
                        (volumeControl.volume < 67)?"image://meegotheme/icons/actionbar/media-volume-2-active":
                        "image://meegotheme/icons/actionbar/media-volume-max-active"
            onClicked: {
                if (volumeLoader.sourceComponent != null ) {
                    volumeLoader.sourceComponent = null;
                } else {
                    volumeLoader.sourceComponent = volumeControlComponent;
                    volumeLoader.item.parent = volumeParent;
                    volumeLoader.item.z = 1000;
                    volumeLoader.item.volumeControl = volumeControl;
                    volumeLoader.item.volumeControlX = btVolume.x + (btVolume.width - volumeLoader.item.volumeWidth)/2
                    volumeLoader.item.volumeControlY = volumeParent.height - container.height - volumeLoader.item.volumeHeight;
                    volumeLoader.item.closeTimer.interval = 3000;
                    volumeLoader.item.closeTimer.restart();
                }
            }
        }
        MediaToolbarButton {
            id: favoriteButton
            show: showfavorite
            anchors.right: shuffleBt.left
            anchors.top: parent.top
            height: parent.height
            width: (show)?((showprogressbar)?iwidth:buttonwidth):0
            bgSourceUp: (isfavorite)?"image://meegotheme/icons/toolbar/contact-favorite-selected":"image://meegotheme/icons/toolbar/contact-favorite"
            bgSourceDn: (isfavorite)?"image://meegotheme/icons/toolbar/contact-favorite-selected-active":"image://meegotheme/icons/toolbar/contact-favorite-active"
            onClicked: {
                isfavorite = !isfavorite;
                container.favoritePressed();
            }
        }
        MediaToolbarButton {
            id: shuffleBt
            show: showshuffle
            anchors.right: repeatBt.left
            anchors.top: parent.top
            height: parent.height
            width: (show)?((showprogressbar)?iwidth:buttonwidth):0
            bgSourceUp: (isshuffle)?"image://meegotheme/icons/actionbar/media-shuffle-selected":"image://meegotheme/icons/actionbar/media-shuffle"
            bgSourceDn: "image://meegotheme/icons/actionbar/media-shuffle-active"
            onClicked: {
                isshuffle = !isshuffle;
                container.shufflePressed();
            }
        }
        MediaToolbarButton {
            id: repeatBt
            show: showrepeat
            anchors.right: parent.right
            anchors.top: parent.top
            height: parent.height
            width: (show)?((showprogressbar)?iwidth:buttonwidth):0
            bgSourceUp: (isrepeat)?"image://meegotheme/icons/actionbar/media-repeat-active-selected":"image://meegotheme/icons/actionbar/media-repeat"
            bgSourceDn: "image://meegotheme/icons/actionbar/media-repeat-active"
            onClicked: {
                isrepeat = !isrepeat;
                container.repeatPressed();
            }
        }
    }
}
