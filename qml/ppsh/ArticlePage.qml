import QtQuick 1.1
import com.nokia.meego 1.1
import "component"
import "../js/main.js" as Script
import "../js/util.js" as Util

BasePage {
	id: root;

	sTitle: webpage.title || qsTr("Article");
	objectName: "idArticlePage";

	function _Init(id)
	{
		obj.aid = id;
		if(obj.aid) obj._View();
	}

	QtObject{
		id: obj;
		property string aid;
		property string order: typerow.vCurrentValue;
		property int pageNo: 1;
		property int pageSize: 20;
		property int pageCount: 0;
		property int totalCount: 0;
		property int num: -1;
		property bool __loadImage: true;
		property bool __helper: false;

		function _GetComment(p)
		{
			if(aid == "") return;

			root.bBusy = true;

			var pn;
			if(typeof(p) === "number") d.pn = p;
			else if(p === constants._sNextPage) pn = pageNo + 1;
			else if(p === constants._sPrevPage) pn = pageNo - 1;
			else if(p === constants._sThisPage) pn = pageNo;
			else pn = 1;
			var d = {
				aid: aid,
				model: commentview.model,
				pageNo: pn,
				order: order,
				type: 12,
			};

			if(pn === 1) Util.ModelClear(commentview.model);

			var s = function(data){
				obj.pageNo = data.pageNo;
				obj.pageSize = data.pageSize;
				obj.pageCount = data.pageCount;
				obj.totalCount = data.totalCount;
				num = obj.totalCount;
				root.bBusy = false;
			};
			var f = function(err){
				root.bBusy = false;
				controller._ShowMessage(err);
			};

			Script.GetComment(d, s, f);
		}

		function _View()
		{
			if(aid == "") return;

			webpage.url = "";
			pageNo = 1;
			pageSize = 20;
			pageCount = 0;
			totalCount = 0;
			num = -1;
			Util.ModelClear(commentview.model);
			flip._Toggle(false);
			typerow.iCurrentIndex = 0;
			order = typerow.aOptions[0].value;

			var d = {
				aid: aid,
			};
			var s = function(data){
				webpage.url = data;
			};
			Script.GetArticleDetail(d, s);
		}
	}

	Header{
		id: header;
		sText: root.sTitle;
		iTextMargin: back.width;
		onClicked: {
			obj._View();
		}
		ToolBarLayout{
			anchors.fill: parent;
			ToolIcon{
				id: back;
				iconId: "toolbar-back";
				onClicked: {
					pageStack.pop();
				}
			}
			ToolIcon{
				iconId: "toolbar-view-menu";
				onClicked: {
					mainmenu.open();
				}
			}
		}
	}

	FlipableWidget{
		id: flip;
		anchors.top: header.bottom;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.bottom: parent.bottom;
		front: WebPage{
			id: webpage;
			anchors.fill: parent;
			dontDelegateLinks: false;
			bLoadImage: obj.__loadImage;
			bBrowserHelper: obj.__helper;
			bAllowDblZoom: false;
			onAlert: {
				console.log(message);
			}
			onProgressChanged: {
				if(!flip.bOpen) root.bBusy = webpage.progress !== 1;
			}
		}
		back: Item{
			id: commentitem;
			anchors.fill: parent;
			TypeRowWidget{
				id: typerow;
				anchors.left: parent.left;
				anchors.right: parent.right;
				anchors.top: parent.top;
				sText: "sort";
				aOptions: [
					{
						name: qsTr("New"),
						value: "0",
					},
					{
						name: qsTr("Hot"),
						value: "2",
					},
				]
				vCurrentValue: "";
				onSelected: {
					obj.order = value;
					obj._GetComment();
				}
			}
			CommentListWidget{
				id: commentview;
				anchors.left: parent.left;
				anchors.right: parent.right;
				anchors.top: typerow.bottom;
				anchors.bottom: parent.bottom;
				bHasMore: obj.pageNo < obj.pageCount;
				onRefresh: {
					obj._GetComment();
				}
				onMore: {
					obj._GetComment(constants._sNextPage);
				}
			}
		}
	}

	ContextMenu{
		id: mainmenu;
		MenuLayout {
			MenuItem{
				text: qsTr("Helper");
				clip: true;
				CheckBox{
					anchors.verticalCenter: parent.verticalCenter;
					anchors.right: parent.right;
					checked: obj.__helper;
					onClicked: {
						obj.__helper = checked;
					}
				}
			}
			MenuItem{
				text: qsTr("Load image");
				clip: true;
				CheckBox{
					anchors.verticalCenter: parent.verticalCenter;
					anchors.right: parent.right;
					checked: obj.__loadImage;
					onClicked: {
						obj.__loadImage = checked;
					}
				}
			}
			MenuItem{
				text: qsTr("Refresh");
				onClicked: {
					obj._View();
				}
			}
		}
	}

	IconWidget{
		anchors.right: parent.right;
		anchors.bottom: parent.bottom;
		z: 1;
		opacity: 0.6;
		iconId: flip.bOpen ? "toolbar-gallery" : "toolbar-new-chat";
		enabled: obj.aid != "";
		visible: enabled;
		Text{
			anchors.fill: parent;
			color: constants._cDarkestColor;
			elide: Text.ElideRight;
			horizontalAlignment: Text.AlignHCenter;
			verticalAlignment: Text.AlignBottom;
			font.pixelSize: constants._iFontLarge;
			font.bold: true;
			visible: obj.num >= 0;
			text: Util.FormatCount(obj.num);
		}
		onClicked: {
			flip._Toggle();
			if(flip.bOpen)
			{
				if(obj.num < 0) obj._GetComment();
				else root.bBusy = false;
			}
		}
	}

}
