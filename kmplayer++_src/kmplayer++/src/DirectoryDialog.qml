import QtQuick 1.1
import com.nokia.meego 1.0

Dialog {
  id: directory
  property int __screenWidth: (screen.rotation === 0 || screen.rotation === 180 ? screen.displayWidth : screen.displayHeight) - 32
  property int __screenHeight: (screen.rotation === 0 || screen.rotation === 180 ? screen.displayHeight : screen.displayWidth) - 32

  title: Item {
    id: titlebar
    x: 16 - platformStyle.leftMargin
    height: directory.platformStyle.titleBarHeight
    width: __screenWidth
    Label {
      anchors.verticalCenter: parent.verticalCenter
      anchors.left: parent.left
      font.capitalization: Font.MixedCase
      color: "white"
      text: qsTr("Choose Directory");
    }
    Image {
      anchors.verticalCenter: parent.verticalCenter
      anchors.right: parent.right
      source: "image://theme/icon-m-common-dialog-close"
      MouseArea {
        anchors.fill: parent
        onClicked:  { directory.reject(); }
      }
    }
  }
  content: [
    Button {
      id: upButton
      iconSource: 'image://theme/icon-m-toolbar-main-view-white'
      x: titlebar.x + 16
      width: 80
      height: 64
      visible: dirModel.canGoUp
      onClicked: dirModel.goUp()
    },
    Column {
      anchors.topMargin: 20
      x: upButton.x + upButton.width + 20
      width: __screenWidth - 140
      height: __screenHeight - 116
      spacing: 20
      Button {
        id: dirButton
        iconSource: 'image://theme/icon-m-toolbar-done-white-selected'
        text: dirModel.directory
        width: parent.width
        height: 64
        onClicked: directory.accept()
      }
      ListView {
        id: entries
        model: dirModel
        clip: true
        width: parent.width
        height: __screenHeight - 200
        delegate: Item {
          id: listItem
          height: 64
          width: parent.width

          Rectangle {
            color: "#000000"
            border.color: "#202000"
            border.width: 2
            radius: 20
            anchors.fill: parent
          }
          BorderImage {
            id: background
            anchors.fill: parent
            anchors.leftMargin: -entries.anchors.leftMargin
            anchors.rightMargin: -entries.anchors.rightMargin
            visible: mouseArea.pressed
            source: "image://theme/meegotouch-list-background-pressed-center"
          }

          Row {
            anchors.fill: parent

            Item {
              width: 8
              height: parent.height
            }
            Image {
              source: "image://theme/icon-m-toolbar-directory-white"
              anchors.verticalCenter: parent.verticalCenter
            }
            Label {
              anchors.verticalCenter: parent.verticalCenter
              id: mainText
              text: caption
              font.weight: Font.Normal
              font.pixelSize: 24
              color: "#D0D0D0"
            }
          }

          MouseArea {
            id: mouseArea
            anchors.fill: parent
            onClicked: { entries.currentIndex = index; dirModel.directory = caption }
          }
        }
      }
    }
  ]
  onAccepted: { playControl.directoryChosen(dirButton.text); directory.destroy() }
  onRejected: { playControl.directoryChosen(""); directory.destroy() }
}

