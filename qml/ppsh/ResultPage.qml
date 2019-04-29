import QtQuick 1.1
import com.nokia.meego 1.1
import "component"
import "../js/main.js" as Script
import "../js/util.js" as Util

BasePage {
	id: root;

	sTitle: qsTr("Search Result");
	objectName: "idResultPage";

	Header{
		id: header;
		/*
		onClicked: {
			obj._SearchResult(obj.keyword);
		}
		*/
		//sText: root.sTitle;
		Row{
			anchors.fill: parent;
			clip: true;
			spacing: constants._iSpacingSmall;
			ToolIcon{
				id: back;
				anchors.verticalCenter: parent.verticalCenter;
				iconId: "toolbar-back";
				onClicked: pageStack.pop();
			}
			SearchWidget{
				id: input;
				anchors.verticalCenter: parent.verticalCenter;
				width: parent.width - parent.spacing - back.width;
				sPlaceholder: qsTr("Search keyword or avId");
				sActionKeyLabel: qsTr("Search");
				onSearch: {
					Script.AddKeywordHistory(input.sText);
					obj._SearchResult();
				}
			}
		}
	}

	function _Init(kw)
	{
		if(kw)
		obj._SearchResult(kw);
	}

	QtObject{
		id: obj;
		property string keyword;
		property int pageNo: 1;
		property int pageSize: 20;
		property int pageCount: 0;
		property int totalCount: 0;
		property string order: typerow.vCurrentValue;

		function _SearchResult(kw, p)
		{
			if(kw) input.sText = kw;

			if(input.sText !== keyword)
			{
				keyword = input.sText;
				typerow.iCurrentIndex = 0;
				order = typerow.aOptions[0].value;
			}
			if(keyword == "") return;

			root.bBusy = true;

			var pn;
			if(typeof(p) === "number") d.pn = p;
			else if(p === constants._sNextPage) pn = pageNo + 1;
			else if(p === constants._sPrevPage) pn = pageNo - 1;
			else if(p === constants._sThisPage) pn = pageNo;
			else pn = 1;
			var d = {
				keyword: keyword,
				model: view.model,
				pageNo: pn,
			};
			if(order !== "") d.order = order;

			if(p === undefined)
			{
				pageNo = 1;
				pageSize = 20;
				pageCount = 0;
				totalCount = 0;
			}
			Util.ModelClear(view.model);

			var s = function(data){
				obj.pageNo = data.pageNo;
				obj.pageSize = data.pageSize;
				obj.pageCount = data.pageCount;
				obj.totalCount = data.totalCount;
				root.bBusy = false;
			};
			var f = function(err){
				root.bBusy = false;
				controller._ShowMessage(err);
			};

			Script.SearchKeyword(d, s, f);
		}
	}

	TypeRowWidget{
		id: typerow;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.top: header.bottom;
		sText: "order";
		aOptions: [
			{
				name: qsTr("Relation"),
				value: "",
			},
			{
				name: qsTr("Click"),
				value: "click",
			},
			{
				name: qsTr("Date"),
				value: "pubdate",
			},
			{
				name: qsTr("Danmaku"),
				value: "dm",
			},
			{
				name: qsTr("Stow"),
				value: "stow",
			},
		]
		vCurrentValue: "";
		onSelected: {
			obj.order = value;
			obj._SearchResult(obj.keyword);
		}
	}

	VideoListWidget{
		id: view;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.top: typerow.bottom;
		anchors.bottom: parent.bottom;
		onRefresh: {
			obj._SearchResult(obj.keyword, constants._sThisPage);
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
				obj._SearchResult(obj.keyword, constants._sPrevPage);
			}
		}
		IconWidget{
			iconId: "toolbar-next";
			enabled: obj.pageNo < obj.pageCount;
			onClicked: {
				obj._SearchResult(obj.keyword, constants._sNextPage);
			}
		}
	}
}
