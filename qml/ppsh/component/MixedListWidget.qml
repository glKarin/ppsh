import QtQuick 1.1
import com.nokia.meego 1.1
import "../../js/main.js" as Script
import "../../js/util.js" as Util

Item{
	id: root;
	property bool bEditMode: false;
	property alias model: view.model;
	property alias count: view.count;
	property alias interactive: view.interactive;
	signal refresh;
	signal longPressed(int index, variant data);
	objectName: "idMixedListWidget";

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
			MixedListDelegate{
				width: ListView.view.width;
				height: model.type == constants._eBangumiType ? constants._iSizeBig : (model.type == constants._eUserType ? constants._iSizeXL : constants._iSizeXXXL);
				editMode: root.bEditMode;
				onClicked: {
					if(!root.bEditMode) __View(aid, data);
				}
				onImageClicked: {
					if(!root.bEditMode)
					{
						__Open(aid, data);
					}
				}
				onLongPressed: {
					if(root.bEditMode) root.longPressed(index, model);
				}
			}
		}
	}

	ScrollDecorator{
		flickableItem: view;
	}

	function __View(aid, data)
	{
		if(data.type == constants._eVideoType) controller._OpenDetailPage(aid);
		else if(data.type == constants._eBangumiType) controller._OpenBangumiDetailPage(aid);
		else if(data.type == constants._eUserType) controller._OpenUserPage(aid);
		else if(data.type == constants._eArticleType)
		{
			Script.AddViewHistory(aid, data.title, data.preview, data.up, data.type);
			controller._OpenArticlePage(aid);
		}
	}

	function __Open(aid, data)
	{
		if(data.type == constants._eVideoType || data.type == constants._eBangumiType)
		{
			Script.AddViewHistory(aid, data.title, data.preview, data.up, data.type);
			controller._OpenPlayerPage(aid, data.type);
		}
		else __View(aid, data);
	}
}
