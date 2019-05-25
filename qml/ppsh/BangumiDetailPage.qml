import QtQuick 1.1
import com.nokia.meego 1.1
import "component"
import "../js/main.js" as Script
import "../js/util.js" as Util

BasePage {
	id: root;

	sTitle: qsTr("Bangumi detail");
	objectName: "idBangumiDetailPage";

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

	function _Init(id, not_first)
	{
		if(!id)
		return;
		obj.sid = id;
		obj._GetAll(not_first);
	}

	QtObject{
		id: obj;
		property string sid;
		property string order: typerow.vCurrentValue;
		property string aid;
		property int pageNo: 1;
		property int pageSize: 20;
		property int pageCount: 0;
		property int totalCount: 0;
		property string uname;
		property string preview;
		property string title;

		function _PlayOnPage()
		{
			if(sid === "") return;

			loader._DeInit();
			Script.AddViewHistory(sid, title, preview, uname, constants._eBangumiType);
			controller._OpenPlayerPage(sid, constants._eBangumiType);
		}

		function _Play(index, aid, cid, epid)
		{
			if(aid === "" || cid === "" || epid === "" || Util.ModelSize(partview.episodeModel) === 0) return;

			root.bBusy = true;
			if(loader.item === null) controller._ShowMessage(qsTr("Loading video player..."));
			var r = loader._Load(aid, partview.episodeModel, cid, constants._eBangumiType);
			if(r < 0) controller._ShowMessage(qsTr("Load video player fail"));
			else if(r === 0)
			{
				controller._ShowMessage(qsTr("Hold player to switch fullscreen or normal"));
				Script.AddViewHistory(sid, title, preview, uname, constants._eBangumiType);
			}
			root.bBusy = false;
		}

		function _GetAll(not_first)
		{
			pageNo = 1;
			pageSize = 20;
			pageCount = 0;
			totalCount = 0;

			Util.ModelClear(recommendview.model);
			Util.ModelClear(commentview.model);
			if(!not_first) tabgroup.currentTab = descview;

			typerow.iCurrentIndex = 0;
			order = typerow.aOptions[0].value;

			_GetDetail();
			//_GetComment();
			//_GetRecommend();
		}

		function _GetDetail()
		{
			if(sid == "") return;

			root.bBusy = true;

			preview = "";
			title = "";
			uname = "";
			aid = "";

			parttab.num = 0;
			commenttab.num = -1;
			desc.text = "";
			newep.text = "";
			ratingscore.text = "";
			ratingcount.text = "";
			infolist.model = [];
			ranklist.model = [];

			var d = {
				season_model: partview.seasonModel,
				episode_model: partview.episodeModel,
				sid: sid,
			};

			Util.ModelClear(partview.seasonModel);
			Util.ModelClear(partview.episodeModel);

			var s = function(data){
				obj.preview = data.preview;
				obj.uname = data.up;
				obj.title = data.title;
				obj.aid = data.first_aid;
				partview._SetCurrentSeason(obj.sid);

				desc.text = data.desc;
				newep.text = data.newest_ep;
				ratingscore.text = data.rating_score + qsTr("score");
				ratingcount.text = Util.FormatCount(data.rating_count);
				parttab.num = data.episodes;
				//commenttab.num = data.reply;
				infolist.model = [
					{ name: qsTr("Play"), value: Util.FormatCount(data.view_count), },
					{ name: qsTr("Like"), value: Util.FormatCount(data.favorite), },
				];
				ranklist.model = [
					{ name: qsTr("Coin"), value: Util.FormatCount(data.coin), },
					{ name: qsTr("Share"), value: Util.FormatCount(data.share), },
					{ name: qsTr("Danmaku"), value: Util.FormatCount(data.danmu_count), },
				];

				root.bBusy = false;
			};
			var f = function(err){
				root.bBusy = false;
				controller._ShowMessage(err);
			};

			Script.GetBangumiDetail(d, s, f);
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
				order: order,
			};

			if(pn === 1) Util.ModelClear(commentview.model);

			var s = function(data){
				obj.pageNo = data.pageNo;
				obj.pageSize = data.pageSize;
				obj.pageCount = data.pageCount;
				obj.totalCount = data.totalCount;
				commenttab.num = data.totalCount;
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
			if(sid == "") return;

			root.bBusy = true;

			var d = {
				model: recommendview.model,
				sid: sid,
			};

			Util.ModelClear(recommendview.model);

			var s = function(){
				root.bBusy = false;
			};
			var f = function(err){
				root.bBusy = false;
				controller._ShowMessage(err);
			};

			Script.GetBangumiRecommend(d, s, f);
		}

		function _GetEpisodes()
		{
			if(sid == "") return;

			root.bBusy = true;

			var d = {
				episode_model: partview.episodeModel,
				sid: sid,
			};

			Util.ModelClear(partview.episodeModel);

			var s = function(data){
				root.bBusy = false;
			};
			var f = function(err){
				root.bBusy = false;
				controller._ShowMessage(err);
			};

			Script.GetBangumioDetail(d, s, f);
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
			text: qsTr("Episodes") + "\n" + num;
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
			property int num: -1;
			height: parent.height;
			text: qsTr("Comment") + (num >= 0 ? "\n" + num : "");
			tab: commentitem;
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
			Flickable{
				id: flick;
				anchors.topMargin: constants._iSpacingXL;
				anchors.fill: parent;
				contentWidth: width;
				contentHeight: desclayout.height;
				clip: true;
				Column{
					id: desclayout;
					anchors.horizontalCenter: parent.horizontalCenter;
					width: parent.width - constants._iSpacingLarge * 2;
					spacing: constants._iSpacingLarge;
					Row{
						id: descrow;
						width: parent.width;
						height: constants._iSizeXXXL;
						spacing: constants._iSpacingMedium;
						Image{
							id: smallpreview;
							anchors.verticalCenter: parent.verticalCenter;
							width: Util.GetSize(height, 0, "4/3");
							height: parent.height;
							cache: false;
							sourceSize.width: width;
							source: obj.preview;
						}
						Column{
							anchors.verticalCenter: parent.verticalCenter;
							width: parent.width - smallpreview.width - ratingview.width - 2 * parent.spacing;
							height: parent.height;
							clip: true;
							Text{
								id: titletext;
								anchors.horizontalCenter: parent.horizontalCenter;
								width: parent.width;
								height: parent.height / 5;
								verticalAlignment: Text.AlignVCenter;
								font.pixelSize: constants._iFontXL;
								elide: Text.ElideRight;
								clip: true;
								color: constants._cDarkestColor;
								text: obj.title;
							}
							Row{
								id: infolayout;
								anchors.horizontalCenter: parent.horizontalCenter;
								width: parent.width;
								height: parent.height / 5;
								spacing: constants._iSpacingLarge;
								clip: true;
								Repeater{
									id: infolist;
									delegate: Component{
										Row{
											height: infolayout.height;
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
							}
							Text{
								id: newep;
								anchors.horizontalCenter: parent.horizontalCenter;
								width: parent.width;
								height: parent.height / 5;
								verticalAlignment: Text.AlignVCenter;
								font.pixelSize: constants._iFontMedium;
								elide: Text.ElideRight;
								clip: true;
								color: constants._cDarkColor;
							}
							Text{
								id: desc;
								anchors.horizontalCenter: parent.horizontalCenter;
								width: parent.width;
								height: parent.height / 5 * 2;
								verticalAlignment: Text.AlignVCenter;
								font.pixelSize: constants._iFontLarge;
								maximumLineCount: 2;
								elide: Text.ElideRight;
								wrapMode: Text.WrapAnywhere;
								clip: true;
								color: constants._cDarkColor;
							}
						}
						Item{
							id: ratingview;
							anchors.verticalCenter: parent.verticalCenter;
							width: Util.GetSize(height, 0, "16/9");
							height: parent.height;
							clip: true;
							Column{
								anchors.left: parent.left;
								anchors.right: parent.right;
								anchors.bottom: parent.bottom;
								spacing: constants._iSpacingSmall;
								Text{
									id: ratingscore;
									anchors.horizontalCenter: parent.horizontalCenter;
									width: parent.width;
									horizontalAlignment: Text.AlignRight;
									font.pixelSize: constants._iFontLarge;
									elide: Text.ElideRight;
									clip: true;
									color: constants._cGlobalColor;
								}
								Text{
									id: ratingcount;
									anchors.horizontalCenter: parent.horizontalCenter;
									width: parent.width;
									horizontalAlignment: Text.AlignRight;
									font.pixelSize: constants._iFontSmall;
									elide: Text.ElideRight;
									clip: true;
									color: constants._cLightColor;
								}
							}
						}
					}
					Row{
						id: ranklayout;
						anchors.horizontalCenter: parent.horizontalCenter;
						width: parent.width;
						height: constants._iSizeXL;
						clip: true;
						Repeater{
							id: ranklist;
							delegate: Component{
								Column{
									width: ranklayout.width / ranklist.model.length;
									height: ranklayout.height;
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
			}
			ScrollDecorator{
				flickableItem: flick;
			}
		}

		SeasonEpisodeWidget{
			id: partview;
			anchors.fill: parent;
			onRefresh: {
				obj._GetEpisodes();
			}
			onClicked: {
				obj.aid = aid;
				Util.ModelClear(commentview.model);
				commenttab.num = -1;
				obj._Play(index, aid, cid, epid);
			}
			onViewClicked: {
				controller._OpenDetailPage(aid);
			}
			onSeason: {
				root._Init(value, true);
			}
		}

		BangumiListWidget{
			id: recommendview;
			anchors.fill: parent;
			onRefresh: {
				obj._GetRecommend();
			}
		}

		Item{
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
				aid: obj.aid;
				onRefresh: {
					obj._GetComment();
				}
				onMore: {
					obj._GetComment(constants._sNextPage);
				}
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
					bLock: false;
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
					bLock: true;
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
