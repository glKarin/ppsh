import QtQuick 1.1
import com.nokia.meego 1.1
import "../../js/util.js" as Util

Item{
	id: root;
	property alias model: view.model;
	property alias count: view.count;
	signal refresh;
	objectName: "idVideoRowWidget";

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
	}

	ListView{
		id: view;
		anchors.fill: parent;
		clip: true;
		orientation: ListView.Horizontal;
		visible: count > 0;
		model: ListModel{}
		delegate: Component{
			VideoGridDelegate{
				width: constants._iSizeTooBig;
				height: ListView.view.height;
				onClicked: {
					controller._OpenDetailPage(aid);
				}
				onImageClicked: {
					controller._OpenPlayerPage(aid);
				}
			}
		}
	}

	ScrollDecorator{
		flickableItem: view;
	}
}
