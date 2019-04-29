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
	property bool bHandleMouseEvent: true;
	signal refresh;
	signal longPressed(int index, variant data);
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
	}

	ListView{
		id: view;
		anchors.fill: parent;
		clip: true;
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
					if(root.bHandleMouseEvent) controller._OpenDetailPage(aid);
				}
				onImageClicked: {
					if(root.bHandleMouseEvent)
					{
						Script.AddViewHistory(aid, data.title, data.preview, data.up, constants._eVideoType);
						controller._OpenPlayerPage(aid);
					}
				}
				onLongPressed: {
					if(root.bHandleMouseEvent) controller._CopyToClipboard(data.aid, "avId");
					else root.longPressed(index, data);
				}
			}
		}
	}

	ScrollDecorator{
		flickableItem: view;
	}
}
