import QtQuick 1.1
import com.nokia.meego 1.1
import "../../js/main.js" as Script
import "../../js/util.js" as Util

Item{
	id: root;
	property alias model: view.model;
	property alias count: view.count;
	property alias interactive: view.interactive;
	property int iDelegateHeight: constants._iSizeXXXL;
	signal refresh;
	objectName: "idVideoListWidget";

	Text{
		anchors.fill: parent;
		horizontalAlignment: Text.AlignHCenter;
		verticalAlignment: Text.AlignVCenter;
		font.bold: true;
		font.pixelSize: constants._iFontSuper;
		elide: Text.ElideRight;
		clip: true;
		color: constants._cLightColor;
		text: qsTr("No content");
		visible: view.count === 0;
		MouseArea{
			anchors.centerIn: parent;
			width: parent.paintedWidth;
			height: parent.paintedHeight;
			onClicked: root.refresh();
		}
	}

	ListView{
		id: view;
		anchors.fill: parent;
		clip: true;
		z: 1;
		visible: count > 0;
		model: ListModel{}
		header: Component{
			RefreshWidget{
				onRefresh: root.refresh();
			}
		}
		delegate: Component{
			VideoListDelegate{
				width: ListView.view.width;
				height: root.iDelegateHeight;
				onClicked: {
					controller._OpenDetailPage(aid);
				}
				onImageClicked: {
					Script.AddViewHistory(aid, data.title, data.preview, data.up, constants._eVideoType);
					controller._OpenPlayerPage(aid);
				}
			}
		}
	}

	ScrollDecorator{
		flickableItem: view;
	}
}
