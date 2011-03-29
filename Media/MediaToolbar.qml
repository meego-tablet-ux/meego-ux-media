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

    BorderImage {
        id: background
        anchors.fill: parent
        source: (landscape)?"image://theme/navigationBar_l":"image://theme/navigationBar_p"
        MediaToolbarButton {
            id: btRewind
            show: showprev
            anchors.left: parent.left
            anchors.top: parent.top
            height: parent.height
            width: (show)?((showprogressbar)?iwidth:buttonwidth):0
            bgSourceUp: "image://theme/media/icn_back_up"
            bgSourceDn: "image://theme/media/icn_back_dn"
            onClicked: container.prevPressed()
        }
        MediaToolbarButton {
            id: btRecord
            show: showrecord
            anchors.left: btRewind.right
            anchors.top:parent.top
            height: parent.height
            width: (show)?((showprogressbar)?iwidth:buttonwidth):0
            bgSourceUp: (disablerecord)?"image://theme/icn_record_disabled":((ispauserecord)?"image://theme/icn_pause_up":"image://theme/icn_record_up")
            bgSourceDn: (disablerecord)?"image://theme/icn_record_disabled":((ispauserecord)?"image://theme/icn_pause_dn":"image://theme/icn_record_dn")
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
            bgSourceUp: (disablestop)?"image://theme/icn_stop_disabled":"image://theme/icn_stop_up"
            bgSourceDn: (disablestop)?"image://theme/icn_stop_disabled":"image://theme/icn_stop_dn"
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
            bgSourceUp: (disableplay)?"image://theme/icn_play_disabled":((ispause)?"image://theme/icn_pause_up":"image://theme/icn_play_up")
            bgSourceDn: (disableplay)?"image://theme/icn_play_disabled":((ispause)?"image://theme/icn_pause_dn":"image://theme/icn_play_dn")
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
            bgSourceUp: "image://theme/media/icn_forward_up"
            bgSourceDn: "image://theme/media/icn_forward_dn"
            onClicked: container.nextPressed()
        }
        Item {
            id: progressBarItem
            visible: showprogressbar
            anchors.left: btForward.right
            anchors.right: btVolume.left
            anchors.rightMargin: 20
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
                anchors.right: parent.right
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
        }
        MediaToolbarButton {
            id: btVolume
            show: showvolume
            anchors.right: favoriteButton.left
            anchors.top: parent.top
            height: parent.height
            width: (show)?((showprogressbar)?iwidth:buttonwidth):0
            bgSourceUp: (volumeControl.volume < 50)?"image://theme/icn_volume_min":"image://theme/icn_volume_up"
            bgSourceDn: "image://theme/icn_volume_dn"
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
            bgSourceUp: (isfavorite)?"image://theme/media/icn_favourite_active":"image://theme/media/icn_favourite_up"
            bgSourceDn: "image://theme/media/icn_favourite_dn"
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
            bgSourceUp: (isshuffle)?"image://theme/icn_shuffle_dn":"image://theme/icn_shuffle_up"
            bgSourceDn: "image://theme/icn_shuffle_dn"
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
            bgSourceUp: (isrepeat)?"image://theme/icn_repeat_dn":"image://theme/icn_repeat_up"
            bgSourceDn: "image://theme/icn_repeat_dn"
            onClicked: {
                isrepeat = !isrepeat;
                container.repeatPressed();
            }
        }
    }
}
