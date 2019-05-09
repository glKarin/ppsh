import QtQuick 1.1
import com.nokia.meego 1.1
import "component"
import "../js/main.js" as Script
import "../js/util.js" as Util

BasePage {
	id: root;

	sTitle: qsTr("Bangumi");
	objectName: "idBangumiPage";

	Header{
		id: header;
		sText: root.sTitle;
		onClicked: {
			obj._GetBangumi();
		}
		ToolBarLayout{
			anchors.fill: parent;
			ToolIcon{
				id: back;
				anchors.verticalCenter: parent.verticalCenter;
				iconId: "toolbar-back";
				onClicked: pageStack.pop();
			}
			Row{
				height: parent.height;
				ToolIcon{
					anchors.verticalCenter: parent.verticalCenter;
					iconId: obj.sort == "1" ? "toolbar-up" : "toolbar-down";
					onClicked: {
						obj.sort = obj.sort == "0" ? "1" : "0";
						obj._GetBangumi();
					}
				}
				ToolIcon{
					anchors.verticalCenter: parent.verticalCenter;
					iconId: "toolbar-view-menu";
					onClicked: {
						filtermenu.open();
					}
				}
			}
		}
	}

	function _Init()
	{
		obj._GetBangumi();
	}

	QtObject{
		id: obj;
		property int pageNo: 1;
		property int pageSize: 18;
		property int pageCount: 0;
		property int totalCount: 0;

		property string season_version: "-1";
		property string area: "-1";
		property string is_finish: "-1";
		property string season_status: "-1";
		property string season_month: "-1";
		property string pub_date: "-1";
		property string style_id: "-1";

		property string order: typerow.vCurrentValue;
		property string sort: "0";

		function _OpenSelection(m)
		{
			for(var i in Script.idBangumi.filter.options)
			{
				if(Script.idBangumi.filter.options[i].value === m.fieldValue)
				{
					var e = Script.idBangumi.filter.options[i];
					var nm = [];
					var cur = 0;
					Util.ModelForeach(e.options, function(e, i){
						if(e.value === m.value) cur = i;
						Util.ModelPush(nm, e.name);
					});
					controller._Select(e.name, nm, function(index){
						var data = e.options[index];
						obj[m.fieldValue] = m.value = data.value;
						m.name = data.name;
						obj._GetBangumi();
					}, e.fieldValue, cur);
					break;
				}
			}
		}

		function _GetBangumi(p)
		{
			root.bBusy = true;
			Util.ModelClear(view.model);

			var pn;
			if(typeof(p) === "number") d.pn = p;
			else if(p === constants._sNextPage) pn = pageNo + 1;
			else if(p === constants._sPrevPage) pn = pageNo - 1;
			else if(p === constants._sThisPage) pn = pageNo;
			else pn = 1;

			var d = {
				model: view.model,
				season_version: season_version,
				area: area,
				is_finish: is_finish,
				season_status: season_status,
				season_month: season_month,
				pub_date: pub_date,
				style_id: style_id,

				order: order,
				sort: sort,
				pageNo: pn,
				pageSize: pageSize,
			};

			if(p === undefined)
			{
				pageNo = 1;
				pageSize = 18;
				pageCount = 0;
				totalCount = 0;
			}

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

			Script.GetBangumi(d, s, f);
		}
	}

	TypeRowWidget{
		id: typerow;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.top: header.bottom;
		sText: "order";
		aOptions: Script.idBangumi.order.options;
		vCurrentValue: Script.idBangumi.order.options[0].value;
		onSelected: {
			obj.order = value;
			obj._GetBangumi();
		}
	}

	BangumiGridWidget{
		id: view;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.top: typerow.bottom;
		anchors.bottom: parent.bottom;
		onRefresh: {
			obj._GetBangumi(constants._sThisPage);
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
			obj._GetBangumi(constants._sPrevPage);
		}
		onNext: {
			obj._GetBangumi(constants._sNextPage);
		}
	}

	ContextMenu{
		id: filtermenu;
		MenuLayout {
			id: menulayout;
		}
	}

	Component{
		id: menuitemcomp;
		MenuItem{
			id: menuitem;
			property string fieldName;
			property string fieldValue;
			property string name;
			property string value;
			text: fieldName + ": " + name;
			onClicked: {
				obj._OpenSelection(menuitem);
			}
		}
	}

	Component.onCompleted: {
		var menucol = menulayout.children[0];
		for(var i in Script.idBangumi.filter.options)
		{
			var e = Script.idBangumi.filter.options[i];
			var prop = {
				fieldName: e.name,
				fieldValue: e.value,
				name: e.options[0].name,
				value: e.options[0].value,
			};
			var m = menuitemcomp.createObject(menucol, prop);
			obj[e.value] = e.options[0].value;
		}
	}
}
