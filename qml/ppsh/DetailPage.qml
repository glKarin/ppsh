import QtQuick 1.1
import com.nokia.meego 1.1
import "component"
import "../js/main.js" as Script
import "../js/util.js" as Util

BasePage {
	id: root;

	sTitle: qsTr("Detail");
	objectName: "idDetailPage";

	Header{
		id: header;
		sText: obj.title !== "" ? obj.title : root.sTitle;
		iTextMargin: back.width;
		onClicked: {
			obj._GetAll();
		}
		ToolBarLayout{
			anchors.fill: parent;
			ToolIcon{
				id: back;
				anchors.verticalCenter: parent.verticalCenter;
				/*
				height: parent.height;
				width: height;
				*/
				iconId: "toolbar-back";
				onClicked: {
					loader._DeInit();
					pageStack.pop();
				}
			}
		}
	}

	function _Init(id)
	{
		if(!id)
		return;
		obj.aid = id;
		obj._GetAll();
	}

	QtObject{
		id: obj;
		property string aid;
		property int pageNo: 1;
		property int pageSize: 20;
		property int pageCount: 0;
		property int totalCount: 0;
		property string uname;
		property string preview;
		property string title;
		property string uid;

		function _PlayOnPage()
		{
			if(aid === "") return;

			loader._DeInit();
			Script.AddViewHistory(aid, title, preview, uname, constants._eVideoType);
			controller._OpenPlayerPage(aid);
		}

		function _Play(index, cid)
		{
			if(aid === "" || Util.ModelSize(partview.model) === 0) return;

			root.bBusy = true;
			if(loader.item === null) controller._ShowMessage(qsTr("Loading video player..."));
			var r = loader._Load(aid, partview.model, cid);
			if(r < 0) controller._ShowMessage(qsTr("Load video player fail"));
			else if(r === 0)
			{
				controller._ShowMessage(qsTr("Hold player to switch fullscreen or normal"));
				Script.AddViewHistory(aid, title, preview, uname, constants._eVideoType);
			}
			root.bBusy = false;
		}

		function _GetAll()
		{
			pageNo = 1;
			pageSize = 20;
			pageCount = 0;
			totalCount = 0;

			Util.ModelClear(recommendview.model);
			Util.ModelClear(commentview.model);
			tabgroup.currentTab = descview;

			_GetDetail();
			//_GetComment();
			//_GetRecommend();
		}

		function _GetDetail()
		{
			if(aid == "") return;

			root.bBusy = true;

			preview = "";
			title = "";
			uname = "";
			uid = "";

			parttab.num = 0;
			commenttab.num = 0;
			avatar.source = "";
			desc.text = "";
			infolist.model = [];
			ranklist.model = [];

			var d = {
				model: partview.model,
				aid: aid,
			};

			Util.ModelClear(partview.model);

			var s = function(data){
				obj.preview = data.preview;
				obj.uname = data.up;
				obj.title = data.title;
				obj.uid = data.uid;

				avatar.source = data.avatar;
				desc.text = data.desc;
				parttab.num = data.videos;
				commenttab.num = data.reply;
				infolist.model = [
					{ name: "", value: Util.FormatCount(data.view_count), },
					{ name: "", value: Util.FormatCount(data.danmu_count), },
					{ name: "", value: Util.FormatTimestamp(data.create_time), },
					{ name: "", value: "AV" + data.aid, },
				];
				ranklist.model = [
					{ name: qsTr("Like"), value: Util.FormatCount(data.like), },
					{ name: qsTr("Dislike"), value: Util.FormatCount(data.dislike), },
					{ name: qsTr("Coin"), value: Util.FormatCount(data.coin), },
					{ name: qsTr("Favorite"), value: Util.FormatCount(data.favorite), },
					{ name: qsTr("Share"), value: Util.FormatCount(data.share), },
				];

				root.bBusy = false;
			};
			var f = function(err){
				root.bBusy = false;
				controller._ShowMessage(err);
			};

			Script.GetVideoDetail(d, s, f);
		}

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
			};

			if(pn === 1) Util.ModelClear(commentview.model);

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

			Script.GetComment(d, s, f);
		}

		function _GetRecommend()
		{
			if(aid == "") return;

			root.bBusy = true;

			var d = {
				model: recommendview.model,
				aid: aid,
			};

			Util.ModelClear(recommendview.model);

			var s = function(){
				root.bBusy = false;
			};
			var f = function(err){
				root.bBusy = false;
				controller._ShowMessage(err);
			};

			Script.GetRecommend(d, s, f);
		}

		function _GetParts()
		{
			if(aid == "") return;

			root.bBusy = true;

			var d = {
				model: partview.model,
				aid: aid,
			};

			Util.ModelClear(partview.model);

			var s = function(data){
				root.bBusy = false;
			};
			var f = function(err){
				root.bBusy = false;
				controller._ShowMessage(err);
			};

			Script.GetVideoDetail(d, s, f);
		}

	}

	Image{
		id: previewimage;
		anchors.top: header.bottom;
		anchors.left: parent.left;
		width: app.inPortrait ? parent.width : constants._iMaxWidth;
		height: Util.GetSize(width, 0, "16/9");
		fillMode: Image.PreserveAspectCrop;
		clip: true;
		cache: false;
		sourceSize.width: width;
		source: obj.preview;
		MouseArea{
			anchors.fill: parent;
			anchors.margins: constants._iSpacingSuper;
			enabled: obj.aid !== "";
			onClicked: {
				if(obj.aid !== "") obj._PlayOnPage();
				else controller._ShowMessage(qsTr("avId is empty"));
			}
		}
	}

	ButtonRow{
		id: tabrow;
		anchors.top: previewimage.bottom;
		anchors.left: parent.left;
		anchors.right: previewimage.right;
		height: constants._iSizeXL;
		TabButton{
			height: parent.height;
			text: qsTr("Description");
			tab: descview;
		}
		TabButton{
			id: parttab;
			property int num: 0;
			height: parent.height;
			text: qsTr("Videos") + "\n" + num;
			tab: partview;
		}
		TabButton{
			id: recommendtab;
			height: parent.height;
			text: qsTr("Recommend") + (recommendview.count !== 0 ? "\n" + recommendview.count : "");
			tab: recommendview;
			onClicked: {
				if(recommendview.count === 0) obj._GetRecommend();
			}
		}
		TabButton{
			id: commenttab;
			property int num: 0;
			height: parent.height;
			text: qsTr("Comment") + "\n" + num;
			tab: commentview;
			onClicked: {
				if(commentview.count === 0) obj._GetComment();
			}
		}
	}

	TabGroup{
		id: tabgroup;
		anchors.bottom: parent.bottom;
		anchors.right: parent.right;
		anchors.left: app.inPortrait ? parent.left : previewimage.right;
		anchors.top: app.inPortrait ? tabrow.bottom : header.bottom;
		currentTab: descview;

		Item{
			id: descview;
			anchors.fill: parent;
			clip: true;
			Row{
				id: up;
				anchors.top: parent.top;
				anchors.left: parent.left;
				anchors.right: parent.right;
				anchors.leftMargin: constants._iSpacingXXL;
				anchors.rightMargin: constants._iSpacingXXL;
				height: constants._iSizeXXL;
				z: 1;
				spacing: constants._iSpacingXXL;
				Image{
					id: avatar;
					anchors.verticalCenter: parent.verticalCenter;
					width: constants._iSizeXL;
					height: width;
					cache: false;
					sourceSize.width: width;
				}
				Column{
					anchors.verticalCenter: parent.verticalCenter;
					width: parent.width - avatar.width - parent.spacing;
					height: avatar.height;
					Text{
						id: unametext;
						width: parent.width;
						height: parent.height / 2;
						verticalAlignment: Text.AlignVCenter;
						font.pixelSize: constants._iFontLarge;
						elide: Text.ElideRight;
						clip: true;
						color: constants._cDarkestColor;
						text: obj.uname;
					}
					Text{
						id: uidtext;
						width: parent.width;
						height: parent.height / 2;
						verticalAlignment: Text.AlignVCenter;
						font.pixelSize: constants._iFontMedium;
						elide: Text.ElideRight;
						clip: true;
						color: constants._cDarkColor;
						text: obj.uid !== "" ? "UID: " + obj.uid : "";
					}
				}
			}
			MouseArea{
				anchors.fill: up;
				enabled: obj.uid !== "";
				onClicked: {
					if(obj.uid !== "") controller._OpenUserPage(obj.uid);
				}
			}

			Flickable{
				id: flick;
				anchors.top: up.bottom;
				anchors.topMargin: constants._iSpacingXXL;
				anchors.left: parent.left;
				anchors.right: parent.right;
				anchors.bottom: parent.bottom;
				contentWidth: width;
				contentHeight: desclayout.height;
				clip: true;
				Column{
					id: desclayout;
					anchors.verticalCenter: parent.verticalCenter;
					width: parent.width;
					spacing: constants._iSpacingSmall;
					Text{
						id: titletext;
						anchors.horizontalCenter: parent.horizontalCenter;
						width: parent.width - constants._iSpacingXXL * 2;
						font.pixelSize: constants._iFontXL;
						//elide: Text.ElideRight;
						wrapMode: Text.WrapAnywhere;
						clip: true;
						color: constants._cDarkestColor;
						text: obj.title;
					}
					ListView{
						id: infolist;
						anchors.horizontalCenter: parent.horizontalCenter;
						width: parent.width - constants._iSpacingXXL * 2;
						height: constants._iSizeLarge;
						interactive: false;
						spacing: constants._iSpacingLarge;
						clip: true;
						orientation: ListView.Horizontal;
						delegate: Component{
							Row{
								height: ListView.view.height;
								clip: true;
								Text{
									height: parent.height;
									verticalAlignment: Text.AlignVCenter;
									font.pixelSize: constants._iFontLarge;
									elide: Text.ElideRight;
									font.bold: true;
									color: constants._cDarkerColor;
									text: modelData.name;
								}
								Text{
									height: parent.height;
									verticalAlignment: Text.AlignVCenter;
									font.pixelSize: constants._iFontMedium;
									elide: Text.ElideRight;
									color: constants._cDarkColor;
									text: modelData.value;
								}
							}
						}
					}
					Text{
						id: desc;
						anchors.horizontalCenter: parent.horizontalCenter;
						width: parent.width - constants._iSpacingXXL * 2;
						verticalAlignment: Text.AlignVCenter;
						font.pixelSize: constants._iFontLarge;
						//elide: Text.ElideRight;
						wrapMode: Text.WrapAnywhere;
						clip: true;
						color: constants._cDarkColor;
					}
					ListView{
						id: ranklist;
						anchors.horizontalCenter: parent.horizontalCenter;
						width: parent.width - constants._iSpacingXXL * 2;
						height: constants._iSizeXL;
						interactive: false;
						clip: true;
						orientation: ListView.Horizontal;
						delegate: Component{
							Column{
								width: ListView.view.width / 5;
								height: ListView.view.height;
								clip: true;
								Text{
									width: parent.width;
									height: parent.height / 2;
									verticalAlignment: Text.AlignVCenter;
									horizontalAlignment: Text.AlignHCenter;
									font.pixelSize: constants._iFontLarge;
									elide: Text.ElideRight;
									font.bold: true;
									color: constants._cDarkerColor;
									text: modelData.name;
								}
								Text{
									width: parent.width;
									height: parent.height / 2;
									verticalAlignment: Text.AlignVCenter;
									horizontalAlignment: Text.AlignHCenter;
									font.pixelSize: constants._iFontMedium;
									elide: Text.ElideRight;
									color: constants._cDarkColor;
									text: modelData.value;
								}
							}
						}
					}
				}
			}
			ScrollDecorator{
				flickableItem: flick;
			}
		}

		PartListWidget{
			id: partview;
			anchors.fill: parent;
			onRefresh: {
				obj._GetParts();
			}
			onClicked: {
				obj._Play(index, cid);
			}
		}
		VideoListWidget{
			id: recommendview;
			anchors.fill: parent;
			onRefresh: {
				obj._GetRecommend();
			}
		}
		CommentListWidget{
			id: commentview;
			anchors.fill: parent;
			bHasMore: obj.pageNo < obj.pageCount;
			onRefresh: {
				obj._GetComment();
			}
			onMore: {
				obj._GetComment(constants._sNextPage);
			}
		}
	}

	PlayerLoader{
		id: loader;
		property int __duration: 250;
		state: constants._sHideState;
		transform: [
			Rotation {
				id: rot;
				origin: Qt.vector3d(app.width / 2, app.height / 2, 0);
				axis: Qt.vector3d(0, 0, 1);
				angle: 0;
			}
		]
		states: [
			State{
				name: constants._sHideState;
				PropertyChanges{
					target: root;
					bFull: false;
				}
				PropertyChanges{
					target: loader;
					width: previewimage.width;
					height: previewimage.height;
					z: 10;
				}
				AnchorChanges{
					target: loader;
					anchors.verticalCenter: previewimage.verticalCenter;
					anchors.horizontalCenter: previewimage.horizontalCenter;
				}
				PropertyChanges{
					target: rot;
					angle: 0;
				}
			},
			State{
				name: constants._sShowState;
				PropertyChanges{
					target: root;
					bFull: true;
				}
				PropertyChanges{
					target: loader;
					width: app.width;
					height: app.height;
					z: header.z + 100;
				}
				AnchorChanges{
					target: loader;
					anchors.verticalCenter: root.verticalCenter;
					anchors.horizontalCenter: root.horizontalCenter;
				}
				PropertyChanges{
					target: rot;
					angle: app.inPortrait ? 90 : 0;
				}
			}
		]

		transitions: [
			Transition{
				ParallelAnimation{
					NumberAnimation{
						target: loader;
						properties: "width,height,z";
						duration: loader.__duration;
					}
					AnchorAnimation{
						duration: loader.__duration;
					}
					RotationAnimation{
						duration: loader.__duration;
					}
				}
			}
		]
		onExit: {
			__Toggle(false);
		}
		onMenu: {
			__Toggle();
		}
		function __Toggle(on)
		{
			if(on === undefined)
			{
				state = state === constants._sShowState ? constants._sHideState : constants._sShowState;
			}
			else
			{
				if(on) state = constants._sShowState;
				else state = constants._sHideState;
			}
		}
	}
	onStatusChanged: {
		if(status === PageStatus.Deactivating)
		{
			loader._DeInit();
		}
	}

	Component.onDestruction: {
		loader._DeInit();
	}
}
