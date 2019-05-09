import QtQuick 1.1
import com.nokia.meego 1.1
import "component"
import "../js/main.js" as Script
import "../js/util.js" as Util

BasePage {
	id: root;

	sTitle: flip.bOpen ? qsTr("Category") : obj.categoryName;
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
				iconId: flip.bOpen ? "toolbar-list" : "toolbar-grid";
				onClicked: {
					flip._Toggle();
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
				// local: 1,
			};

			var s = function(data){
				if(Array.isArray(data)) channelsview.model = data;
				else channelsview.model = data.children;
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

			if(!flip.bOpen) flip._Toggle(true);

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

	FlipableWidget{
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

			PagedWidget{
				anchors.bottom: parent.bottom;
				anchors.horizontalCenter: parent.horizontalCenter;
				pageNo: obj.pageNo;
				pageSize: obj.pageSize;
				pageCount: obj.pageCount;
				totalCount: obj.totalCount;
				onPrev: {
					obj._GetCategory(undefined, undefined, constants._sPrevPage);
				}
				onNext: {
					obj._GetCategory(undefined, undefined, constants._sNextPage);
				}
			}
		}
	}
}
