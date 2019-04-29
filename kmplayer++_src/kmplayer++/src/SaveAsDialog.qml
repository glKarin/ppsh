import QtQuick 1.1
import com.nokia.meego 1.0

Dialog {
  id: saveAs
  property string caption
  property string file

  Component.onCompleted: { input.selectAll(); input.forceActiveFocus() }

  title: Item {
    height: saveAs.platformStyle.titleBarHeight
    width: parent.width
    Label {
      anchors.verticalCenter: parent.verticalCenter
      anchors.left: parent.left
      font.capitalization: Font.MixedCase
      color: "white"
      text: qsTr("Save As");
    }
    Image {
      anchors.verticalCenter: parent.verticalCenter
      anchors.right: parent.right
      source: "image://theme/icon-m-common-dialog-close"
      MouseArea {
        anchors.fill: parent
        onClicked:  { saveAs.reject(); }
      }
    }
  }
  content: Column {
    spacing: 20
    x: buttons.x + 10
    width: buttons.width - 20
    Item {
      width: parent.width
      height: 20
    }
    Button {
      id: dirButton
      iconSource: 'image://theme/icon-m-toolbar-directory-white'
      text: dirModel.directory
      width: parent.width
      onClicked: { input.focus = false;Qt.createComponent("DirectoryDialog.qml").createObject(listPage).open() }
    }
    TextField {
      id: input
      width: parent.width
      text: saveAs.file
    }
    Item {
      width: parent.width
      height: 20
    }
    CheckBox {
      id: remove
      text: qsTr("Auto remove from Phone")
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
    Button {id: b1; text: qsTr("OK"); onClicked: saveAs.accept()}
    Button {id: b2; text: qsTr("Cancel"); onClicked: saveAs.reject()}
  }
  onAccepted: { playControl.saveFileAs(caption, input.text, remove.checked); saveAs.destroy() }
  onRejected: { playControl.saveFileAs("", ""); saveAs.destroy() }
}

