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
		property string type;
		property int pageNo: 1;
		property int pageSize: 20;
		property int pageCount: 0;
		property int totalCount: 0;
		property string order: "";

		function _SetType(t, i)
		{
			if(type == t) return;

			type = t;
			var orders = [];
			Util.ModelCopy(orders, Util.ModelGetValue(typeview.model, i, "order"));
			typerow.aOptions = orders;
			typerow.vCurrentValue = Util.ModelGetValue(typerow.aOptions, 0, "value");
			order = typerow.vCurrentValue;

			pageNo = 1;
			pageSize = 20;
			pageCount = 0;
			totalCount = 0;
		}

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
			if(type !== "") d.type = type;
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

	TabListWidget{
		id: typeview;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.top: header.bottom;
		anchors.topMargin: constants._iSpacingMicro;
		height: constants._iSizeXL;
		bTabMode: true;
		onClicked: {
			obj._SetType(value, index);
			obj._SearchResult(obj.keyword);
		}
	}
	TypeRowWidget{
		id: typerow;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.top: typeview.bottom;
		sText: "order";
		onSelected: {
			obj.order = value;
			obj._SearchResult(obj.keyword);
		}
	}

	ResultListWidget{
		id: view;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.top: typerow.bottom;
		anchors.bottom: parent.bottom;
		onRefresh: {
			obj._SearchResult(obj.keyword, constants._sThisPage);
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
			obj._SearchResult(obj.keyword, constants._sPrevPage);
		}
		onNext: {
			obj._SearchResult(obj.keyword, constants._sNextPage);
		}
	}

	Component.onCompleted: {
		var limit = 5;
		typeview.bTabMode = (limit <= 4);
		typeview._LoadModel(Script.idSearch, limit);
		obj._SetType(Util.ModelGetValue(typeview.model, 0, "value"), 0);
	}
}
