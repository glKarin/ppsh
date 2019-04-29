import QtQuick 1.1
import com.nokia.meego 1.1
import "component"
import "../js/main.js" as Script
import "../js/util.js" as Util

BasePage {
	id: root;

	sTitle: flip.side === Flipable.Front ? qsTr("Category") : obj.categoryName;
	objectName: "idCategoryPage";

	Header{
		id: header;
		sText: root.sTitle;
		ToolBarLayout{
			anchors.fill: parent;
			ToolIcon{
				id: back;
				anchors.verticalCenter: parent.verticalCenter;
				iconId: "toolbar-back";
				onClicked: pageStack.pop();
			}
			ToolIcon {
				id: flipicon;
				anchors.verticalCenter: parent.verticalCenter;
				visible: false;
				iconId: flip.side === Flipable.Front ? "toolbar-list" : "toolbar-grid";
				onClicked: {
					flip.state = flip.state === "" ? constants._sShowState : "";
				}
			}
		}
	}

	function _Init()
	{
		obj._GetChannels();
	}

	QtObject{
		id: obj;
		property string categoryName;
		property string rid;
		property int pageNo: 1;
		property int pageSize: 20;
		property int pageCount: 0;
		property int totalCount: 0;
		property string order: typerow.vCurrentValue;

		function _GetChannels()
		{
			root.bBusy = true;
			//Util.ModelClear(channelsview.model);

			var d = {
				model: channelsview.model,
			};

			var s = function(data){
				channelsview.model = data.children;
				root.bBusy = false;
			};
			var f = function(err){
				root.bBusy = false;
				controller._ShowMessage(err);
			};

			Script.GetFullChannels(d, s, f);
		}

		function _GetCategory(name, id, p)
		{
			if(id !== undefined && rid !== id)
			{
				rid = id;
				typerow.iCurrentIndex = 0;
				order = typerow.aOptions[0].value;
			}
			if(rid == "") return;
			flipicon.visible = true;
			if(name !== undefined) categoryName = name;

			if(flip.state !== constants._sShowState) flip.state = constants._sShowState;

			root.bBusy = true;

			var pn;
			if(typeof(p) === "number") d.pn = p;
			else if(p === constants._sNextPage) pn = pageNo + 1;
			else if(p === constants._sPrevPage) pn = pageNo - 1;
			else if(p === constants._sThisPage) pn = pageNo;
			else pn = 1;
			var d = {
				rid: rid,
				model: videoview.model,
				pageNo: pn,
				paveSize: 20,
			};

			if(p === undefined)
			{
				pageNo = 1;
				pageSize = 20;
				pageCount = 0;
				totalCount = 0;
			}
			Util.ModelClear(videoview.model);

			var s = function(data){
				if(data)
				{
					obj.pageNo = data.pageNo;
					obj.pageSize = data.pageSize;
					obj.pageCount = data.pageCount;
					obj.totalCount = data.totalCount;
				}
				root.bBusy = false;
			};
			var f = function(err){
				root.bBusy = false;
				controller._ShowMessage(err);
			};

			(order === "RANKING" ? Script.GetCategory : Script.GetCategoryNewlist)(d, s, f);
		}
	}

	Flipable{
		id: flip;
		anchors.top: header.bottom;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.bottom: parent.bottom;
		front: ChannelsGridWidget{
			id: channelsview;
			anchors.fill: parent;
			onRefresh: {
				obj._GetChannels();
			}
			onClicked: {
				obj._GetCategory(name, value);
			}
		}
		back: Item{
			anchors.fill: parent;
			TypeRowWidget{
				id: typerow;
				anchors.left: parent.left;
				anchors.right: parent.right;
				anchors.top: parent.top;
				sText: "order";
				aOptions: [
					{
						name: qsTr("Ranking"),
						value: "RANKING",
					},
					{
						name: qsTr("New"),
						value: "NEWLIST",
					},
				]
				vCurrentValue: "RANKING";
				onSelected: {
					obj.order = value;
					obj._GetCategory();
				}
			}

			VideoListWidget{
				id: videoview;
				anchors.top: typerow.bottom;
				anchors.left: parent.left;
				anchors.right: parent.right;
				anchors.bottom: parent.bottom;
				onRefresh: {
					obj._GetCategory(undefined, undefined, constants._sThisPage);
				}
			}

			ToolBarLayout{
				anchors.bottom: parent.bottom;
				anchors.horizontalCenter: parent.horizontalCenter;
				z: 1;
				height: constants._iSizeXL;
				width: constants._iSizeBig;
				opacity: 0.6;
				IconWidget{
					iconId: "toolbar-previous";
					enabled: obj.pageNo > 1;
					onClicked: {
						obj._GetCategory(undefined, undefined, constants._sPrevPage);
					}
				}
				IconWidget{
					iconId: "toolbar-next";
					enabled: obj.pageNo < obj.pageCount;
					onClicked: {
						obj._GetCategory(undefined, undefined, constants._sNextPage);
					}
				}
			}
		}

		transform: Rotation{
			id: rotation;
			origin: Qt.vector3d(flip.width / 2, flip.height / 2, 0);
			axis: Qt.vector3d(0, 1, 0);
			angle: 0;
		}
		states: State{
			name: constants._sShowState;
			PropertyChanges{
				target: rotation;
				angle: 180;
			}
		}
		transitions: Transition{
			RotationAnimation{
				direction: RotationAnimation.Clockwise;
			}
		}
	}
}
