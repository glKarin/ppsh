import QtQuick 1.1
import com.nokia.meego 1.1
import "../../js/main.js" as Script
import "../../js/util.js" as Util

Item{
	id: root;
	property alias model: view.model;
	property alias count: view.count;
	property alias interactive: view.interactive;
	signal refresh;
	objectName: "idResultListWidget";

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
			ResultListDelegate{
				width: ListView.view.width;
				height: _GetCellHeight(model.type);
				onClicked: {
					__View(mid, data);
				}
				onImageClicked: {
					__Open(mid, data);
				}
				onLongPressed: {
					__Copy(mid, data);
				}
			}
		}
	}

	ScrollDecorator{
		flickableItem: view;
	}

	function __Copy(mid, data)
	{
		controller._CopyToClipboard(mid, data.type == constants._eVideoType ? qsTr("avId") : (data.type == constants._eBangumiType ? qsTr("Bangumi Id") : (data.type == constants._eUserType ? qsTr("UID") : undefined)));
	}

	function __Open(mid, data)
	{
		if(data.type == constants._eVideoType)
		{
			Script.AddViewHistory(mid, data.title, data.preview, data.subtitle, data.type);
			controller._OpenPlayerPage(mid, data.type);
		}
		else __View(mid, data);
	}

	function __View(mid, data)
	{
		if(data.type == constants._eVideoType) controller._OpenDetailPage(mid);
		else if(data.type == constants._eBangumiType) controller._OpenBangumiDetailPage(mid);
		else if(data.type == constants._eUserType) controller._OpenUserPage(mid);
		else if(data.type == constants._eArticleType)
		{
			controller._OpenArticlePage(mid);
			Script.AddViewHistory(mid, data.title, data.preview, "", data.type);
		}
		else if(data.type == constants._eLiveType) controller._OpenLiveDetailPage(mid);
	}

	function _GetCellHeight(type)
	{
		return type == constants._eBangumiType ? constants._iSizeBig : (type == constants._eUserType ? constants._iSizeXXL : constants._iSizeXXXL);
	}
}
