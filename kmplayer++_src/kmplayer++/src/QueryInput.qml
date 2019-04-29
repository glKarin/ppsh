import QtQuick 1.1
import com.nokia.meego 1.0

Dialog {
  id: query
  property string caption
  property string text
  Component.onCompleted: { input.selectAll(); input.forceActiveFocus() }
  title: Item {
    height: query.platformStyle.titleBarHeight
    width: parent.width
    Label {
      anchors.verticalCenter: parent.verticalCenter
      font.capitalization: Font.MixedCase
      color: "white"
      text: query.caption
    }
    Image {
      anchors.verticalCenter: parent.verticalCenter
      anchors.right: parent.right
      source: "image://theme/icon-m-common-dialog-close"
      MouseArea {
        anchors.fill: parent
        onClicked:  { query.reject(); }
      }
    }
  }
  content: Column {
    x: buttons.x + 10
    width: buttons.width - 20
    Item {
      width: parent.width
      height: 20
    }
    TextField {
      id: input
      width: parent.width
      text: query.text
    }
    Item {
      width: parent.width
      height: 20
    }
  }
  buttons: ButtonRow {
    id: buttons
    platformStyle: ButtonStyle { }
    anchors.horizontalCenter: parent.horizontalCenter
    Button { text: qsTr("OK"); onClicked: query.accept()}
    Button { text: qsTr("Cancel"); onClicked: query.reject()}
  }
  onAccepted: { playControl.itemEdit(playlist.currentIndex, input.text); query.destroy(); }
  onRejected: { query.destroy(); }
}

