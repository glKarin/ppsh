import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.0

Page {
  id: listPage
  anchors.margins: 0
  tools: listTools
  orientationLock: playControl.videoVisible ? PageOrientation.LockLandscape : PageOrientation.Automatic

  Component.onCompleted: {
    playControl.uiLoaded();
  }

  function openFile(file) {
    var component = Qt.createComponent(file)

    if (component.status == Component.Ready)
      pageStack.push(component);
    else
      console.log("Error loading component:", component.errorString());
  }

  ListView {
    id: playlist
    anchors.fill: parent
    model: playModel
    highlightMoveDuration: 0
    highlightMoveSpeed: -1

    delegate:  Item {
      id: listItem
      height: 64
      width: parent.width

      Rectangle {
        color: playlist.currentIndex == index ? '#3D3D3D' : playControl.videoVisible ? "#080810" : "#000000"
        border.color: playlist.currentIndex == index ? 'black' : "#202000"
        border.width: 2
        radius: 20
        anchors.fill: parent
     }
      BorderImage {
        id: background
        anchors.fill: parent
        // Fill page borders
        anchors.leftMargin: -playlist.anchors.leftMargin
        anchors.rightMargin: -playlist.anchors.rightMargin
        visible: mouseArea.pressed
        source: "image://theme/meegotouch-list-background-pressed-center"
      }

      Row {
        anchors.fill: parent

        Item {
          width: 8 * depth
          height: parent.height
        }
        Image {
          source: icon
          anchors.verticalCenter: parent.verticalCenter
        }
        Label {
          anchors.verticalCenter: parent.verticalCenter
          id: mainText
          text: title
          font.weight: depth > 1 ? Font.Normal: Font.DemiBold
          font.pixelSize: 24
          color: "#D0D0D0"
        }
      }

      MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: {
          var cur = playlist.currentIndex;
          playlist.currentIndex = index;
          playControl.itemClicked(index, cur);
        }
        onPressAndHold: {
          playlist.currentIndex = index;
          playControl.itemContext(index);
        }
      }
    }
    Connections {
      target: playModel
      onSelectionChanged: {
        playlist.positionViewAtIndex(scroll, ListView.Contain);
        playlist.positionViewAtIndex(index, ListView.Contain);
        playlist.currentIndex = index;
      }
    }
  }

  ScrollDecorator {
    flickableItem: playlist
  }

  InfoBanner {
    id: banner
    timerEnabled: true
    timerShowTime: 3000
    topMargin: 10
    leftMargin: 20
  }

  ContextMenu {
    id: contextMenu
    visualParent: listPage
    MenuLayout {
      id: menuLayout
      Repeater {
        id: menu
        model: []
        MenuItem {
          parent: menuLayout
          text:menu.model[index].substring(1)
          onClicked:playControl.contextSelected(menu.model[index].charCodeAt(0))
        }
      }
    }
  }

  ToolBarLayout {
    id: listTools
    visible: false
    ToolIcon {
      iconId: "toolbar-mediacontrol-backwards"
      visible: !rootWindow.inPortrait && pslider.visible
      onClicked: { playControl.Seek(-200, false) }
    }
    Slider {
      id: pslider
      orientation: Qt.Horizontal
      minimumValue: 0; maximumValue: 1000
      valueIndicatorVisible: false
      value: 0
      visible: value > .1
    }
    ToolIcon {
      iconId: "toolbar-mediacontrol-forward"
      visible: !rootWindow.inPortrait && pslider.visible
      onClicked: { playControl.Seek(200, false) }
    }
    ToolIcon {
      iconId: "toolbar-down"
      visible: playControl.playStatus < 1
      onClicked: {
        playControl.downloadClicked(playlist.currentIndex);
      }
    }
    ToolIcon {
      iconId: "toolbar-mediacontrol-stop"
      visible: playControl.playStatus > 0
      onClicked: { playControl.Stop() }
    }
    ToolIcon {
      iconId: "toolbar-view-menu";
      visible: !rootWindow.inPortrait || !pslider.visible
      onClicked: {
        contextMenu.status == DialogStatus.Closed ? playControl.menuClicked() : contextMenu.close()
      }
    }
    ToolIcon {
      iconId: "toolbar-next";
      visible: playControl.videoVisible
      onClicked: { playControl.raiseVideo() }
    }
    BusyIndicator {
      id: indicator1
      visible: playControl.loadStatus < 100
      platformStyle: BusyIndicatorStyle { size: "small" }
      running: playControl.loadStatus < 100
    }
  }
  Timer {
    interval: 100
    running: pslider.pressed
    triggeredOnStart: true
    repeat: true
    onTriggered: { playControl.Seek(pslider.value, true); }
  }
  ListModel { id: selectModel }
  Connections {
    target: playControl
    onShowErrorMsg: { banner.text = text; banner.show(); }
    onShowSaveFileAs: { Qt.createComponent("SaveAsDialog.qml").createObject(listPage, {"caption":title,"file":file}).open(); }
    onPositionChanged: { if (!pslider.pressed) pslider.value = position; }
    onSelectItem: {
      selectModel.clear();
      for (var i = 0; i < items.length; ++i )
        selectModel.append({name:items[i]});
      Qt.createComponent("SingleSelectionDialog.qml").createObject(listPage, {"context":context,"titleText":title,"model":selectModel}).open()
    }
    onSelectContextItem: {
      menu.model = items
      contextMenu.open();
    }
    onShowEdit: { Qt.createComponent("QueryInput.qml").createObject(listPage, {"caption":title,"text":text }).open() }
    onShowFileOpen: { Qt.createComponent("FileDialog.qml").createObject(listPage).open() }
  }
}
