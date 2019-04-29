import QtQuick 1.1
import com.nokia.meego 1.1
import "component"
import "../js/main.js" as Script
import "../js/util.js" as Util

BasePage {
	id: root;

	sTitle: obj.categoryName || qsTr("Ranking");
	objectName: "idRankingPage";

	Header{
		id: header;
		sText: channelsview.count > 0 ? "" : root.sTitle;
		onClicked: {
			obj._GetChannels();
		}
		Row{
			anchors.fill: parent;
			ToolIcon{
				id: back;
				anchors.verticalCenter: parent.verticalCenter;
				iconId: "toolbar-back";
				onClicked: pageStack.pop();
			}
			TabListWidget{
				id: channelsview;
				anchors.verticalCenter: parent.verticalCenter;
				width: parent.width - back.width;
				height: parent.height;
				visible: count > 0;
				iTopMargin: constants._iSpacingSmall;
				onClicked: {
					obj._GetCategory(name, value);
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
		property string order: typerow.vCurrentValue;
		property int limit: 20;

		function _GetChannels()
		{
			root.bBusy = true;
			Util.ModelClear(channelsview.model);

			var d = {
				model: channelsview.model,
			};

			var s = function(){
				root.bBusy = false;
				Util.ModelForeach(channelsview.model, function(e){
					e.name = Script.GetChannelNameById(e.value) || e.value;
				});
				if(Util.ModelSize(channelsview.model) > 0)
				{
					var r = Util.ModelGet(channelsview.model);
					obj._GetCategory(r.name, r.value);
				}
			};
			var f = function(err){
				root.bBusy = false;
				controller._ShowMessage(err);
			};

			Script.GetChannels(d, s, f);
		}

		function _GetCategory(name, id)
		{
			if(id !== undefined && rid !== id)
			{
				rid = id;
				typerow.iCurrentIndex = 0;
				order = typerow.aOptions[0].value;
			}
			if(rid == "") return;
			if(name !== undefined) categoryName = name;

			_GetDynamic();
			_GetRanking();
		}

		function _GetRanking()
		{
			if(rid == "") return;

			root.bBusy = true;

			var d = {
				rid: rid,
				model: videoview.model,
				pageSize: limit,
			};

			Util.ModelClear(videoview.model);

			var s = function(){
				root.bBusy = false;
			};
			var f = function(err){
				root.bBusy = false;
				controller._ShowMessage(err);
			};

			(order === "NEWLIST" ? Script.GetCategoryNewlist : Script.GetCategoryRanking)(d, s, f);
		}

		function _GetDynamic()
		{
			if(rid == "") return;

			root.bBusy = true;

			var d = {
				rid: rid,
				model: dynamicview.model,
				//pageSize: limit,
				day: 1,
			};

			Util.ModelClear(dynamicview.model);

			var s = function(){
				root.bBusy = false;
			};
			var f = function(err){
				root.bBusy = false;
				controller._ShowMessage(err);
			};

			Script.GetCategoryDynamic(d, s, f);
		}
	}

	SectionWidget{
		id: dynamic;
		anchors.top: header.bottom;
		anchors.left: parent.left;
		width: app.inPortrait ? parent.width : parent.width - constants._iMaxWidth;
		sText: qsTr("Dynamic(Click to refresh)");
		onClicked: {
			obj._GetDynamic();
		}
	}
	VideoRowWidget{
		id: dynamicview;
		anchors.left: parent.left;
		anchors.right: dynamic.right;
		anchors.top: dynamic.bottom;
		height: constants._iSizeTooBig;
	}

	TypeRowWidget{
		id: typerow;
		anchors.left: app.inPortrait ? parent.left : dynamic.right;
		anchors.right: parent.right;
		anchors.top: app.inPortrait ? dynamicview.bottom : header.bottom;
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
			obj._GetRanking();
		}
	}
	VideoListWidget{
		id: videoview;
		anchors.top: typerow.bottom;
		anchors.left: typerow.left;
		anchors.right: parent.right;
		anchors.bottom: parent.bottom;
		onRefresh: {
			obj._GetRanking();
		}
	}
}

