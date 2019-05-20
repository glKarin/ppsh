import QtQuick 1.1
import com.nokia.meego 1.1
import "component"
import "../js/main.js" as Script
import "../js/util.js" as Util

BasePage {
	id: root;

	sTitle: qsTr("Live detail");
	objectName: "idLiveDetailPage";

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
		obj.rid = id;
		obj._GetAll();
	}

	QtObject{
		id: obj;
		property string rid;
		property string uname;
		property string preview;
		property string title;
		property string uid;
		property bool living: false;
		property bool danmakuLoaded: false;

		function _PlayOnPage()
		{
			if(rid === "") return;
			if(!living) return;

			loader._DeInit();
			Script.AddViewHistory(rid, title, preview, uname, constants._eLiveType);
			controller._OpenPlayerPage(rid, constants._eLiveType);
		}

		function _Play(index, qn)
		{
			if(rid === "" || Util.ModelSize(partview.qualityModel) === 0) return;
			if(!living) return;

			root.bBusy = true;
			if(loader.item === null) controller._ShowMessage(qsTr("Loading video player..."));
			var r = loader._Load(rid, partview.qualityModel, qn, constants._eLiveType);
			if(r < 0) controller._ShowMessage(qsTr("Load video player fail"));
			else if(r === 0)
			{
				controller._ShowMessage(qsTr("Hold player to switch fullscreen or normal"));
				Script.AddViewHistory(rid, title, preview, uname, constants._eLiveType);
			}
			root.bBusy = false;
		}

		function _GetAll()
		{
			Util.ModelClear(recommendview.model);
			tabgroup.currentTab = danmakuitem;

			_GetDetail();
			//_GetDanmaku();
			//_GetRecommend();
		}

		function _GetDetail()
		{
			if(rid == "") return;

			root.bBusy = true;

			preview = "";
			title = "";
			uname = "";
			uid = "";
			living = false;
			danmakuLoaded = false;

			avatar.source = "";
			desc.text = "";
			sex.text = "";
			official_verify.official = "";
			area.text = "";
			levelinfo.level = 0;
			infolist.model = [];

			var d = {
				rid: rid,
			};

			Util.ModelClear(partview.qualityModel);
			Util.ModelClear(partview.streamModel);

			var s = function(data){
				living = data.live_status != 0;
				obj.preview = data.preview;
				obj.uname = data.up;
				obj.title = data.title;
				obj.uid = data.uid;

				avatar.source = data.avatar;
				desc.text = data.desc;
				official_verify.official = data.official;
				area.text = data.area;
				infolist.model = [
					{ name: qsTr("Online"), value: Util.FormatCount(data.online), },
					{ name: qsTr("Follower"), value: Util.FormatCount(data.follower), },
					{ name: qsTr("Room ID"), value: data.room_id, },
				];

				_GetUserInfo();
				root.bBusy = false;
			};
			var f = function(err){
				root.bBusy = false;
				controller._ShowMessage(err);
			};

			Script.GetLiveDetail(d, s, f);
		}

		function _GetUserInfo()
		{
			if(rid == "") return;

			root.bBusy = true;

			uname = "";
			uid = "";

			avatar.source = "";
			sex.text = "";
			official_verify.official = "";
			levelinfo.level = 0;

			var d = {
				rid: rid,
			};

			var s = function(data){
				obj.uname = data.up;
				obj.uid = data.uid;

				sex.text = data.sex == 2 ? qsTr("Female") : (data.sex == 1 ? qsTr("Male") : ""); 
				avatar.source = data.avatar;
				levelinfo.level = data.level;
				official_verify.official = data.official;
				root.bBusy = false;
			};
			var f = function(err){
				root.bBusy = false;
				controller._ShowMessage(err);
			};

			Script.GetLiveUserDetail(d, s, f);
		}

		function _GetDanmaku(p)
		{
			if(rid == "") return;

			controller._ShowMessage(qsTr("Coming soon..."));
			danmakuLoaded = true;
		}

		function _GetRecommend()
		{
			if(rid == "") return;

			root.bBusy = true;

			var d = {
				rid: rid,
				count: 20,
				model: recommendview.model,
			};

			Util.ModelClear(recommendview.model);

			var s = function(){
				root.bBusy = false;
			};
			var f = function(err){
				root.bBusy = false;
				controller._ShowMessage(err);
			};

			Script.GetLiveRecommend(d, s, f);
		}

		function _GetQuality()
		{
			if(rid == "") return;

			root.bBusy = true;

			var d = {
				quality_model: partview.qualityModel,
				stream_model: partview.streamModel,
				rid: rid,
			};

			Util.ModelClear(partview.qualityModel);
			Util.ModelClear(partview.streamModel);

			var s = function(){
				if(Util.ModelSize(partview.streamModel) > 0)
				partview._SetCurrentQuality(Util.ModelGetValue(partview.streamModel, 0, "quality"));
				root.bBusy = false;
			};
			var f = function(err){
				root.bBusy = false;
				controller._ShowMessage(err);
			};

			Script.GetLiveQualityStreams(d, s, f);
		}

		function _GetStreams(q)
		{
			if(rid == "") return;

			root.bBusy = true;

			var d = {
				stream_model: partview.streamModel,
				rid: rid,
			};
			if(q !== undefined) d.quality = q;

			Util.ModelClear(partview.streamModel);

			var s = function(){
				root.bBusy = false;
			};
			var f = function(err){
				root.bBusy = false;
				controller._ShowMessage(err);
			};

			Script.GetLiveQualityStreams(d, s, f);
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
		LabelWidget{
			anchors.top: parent.top;
			anchors.right: parent.right;
			anchors.topMargin: constants._iSpacingMedium;
			anchors.rightMargin: constants._iSpacingMedium;
			opacity: 0.8;
			sText: obj.living ? qsTr("Living") : qsTr("Not living");
		}
		MouseArea{
			anchors.fill: parent;
			anchors.margins: constants._iSpacingSuper;
			enabled: obj.rid !== "" && obj.living;
			onClicked: {
				if(!obj.living)
				{
					controller._ShowMessage(qsTr("Room is not living"));
					return;
				}
				if(obj.rid !== "") obj._PlayOnPage();
				else controller._ShowMessage(qsTr("room Id is empty"));
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
			id: danmakutab;
			height: parent.height;
			text: qsTr("Danmaku");
			tab: danmakuitem;
			onClicked: {
				if(!obj.danmakuLoaded) obj._GetDanmaku();
			}
		}
		TabButton{
			id: parttab;
			height: parent.height;
			text: qsTr("Streams") + (partview.qualityCount !== 0 ? "\n" + partview.qualityCount : "");
			tab: partview;
			onClicked: {
				if(partview.qualityCount === 0) obj._GetQuality();
			}
		}
		TabButton{
			height: parent.height;
			text: qsTr("Description");
			tab: descview;
		}
		TabButton{
			id: usertab;
			height: parent.height;
			text: qsTr("Recommend") + (recommendview.count !== 0 ? "\n" + recommendview.count : "");
			tab: recommendview;
			onClicked: {
				if(recommendview.count === 0) obj._GetRecommend();
			}
		}
	}

	TabGroup{
		id: tabgroup;
		anchors.bottom: parent.bottom;
		anchors.right: parent.right;
		anchors.left: app.inPortrait ? parent.left : previewimage.right;
		anchors.top: app.inPortrait ? tabrow.bottom : header.bottom;
		currentTab: danmakuitem;

		Item{
			id: danmakuitem;
			anchors.fill: parent;
			Text{
				anchors.fill: parent;
				horizontalAlignment: Text.AlignHCenter;
				verticalAlignment: Text.AlignVCenter;
				font.bold: true;
				font.pixelSize: constants._iFontSuper;
				elide: Text.ElideRight;
				clip: true;
				color: constants._cLightColor;
				text: qsTr("Not support");
			}
		}

		QualityStreamWidget{
			id: partview;
			anchors.fill: parent;
			onRefresh: {
				obj._GetQuality();
			}
			onClicked: {
				obj._Play(index, quality);
			}
			onQuality: {
				obj._GetStreams(value);
			}
		}

		Item{
			id: descview;
			anchors.fill: parent;
			clip: true;
			Column{
				id: infocol;
				width: parent.width;
				anchors.horizontalCenter: parent.horizontalCenter;
				spacing: constants._iSpacingSmall;
				clip: true;
				Row{
					id: up;
					anchors.horizontalCenter: parent.horizontalCenter;
					width: parent.width - constants._iSpacingXXL * 2;
					height: constants._iSizeXL;
					z: 1;
					spacing: constants._iSpacingXXL;
					Image{
						id: avatar;
						anchors.verticalCenter: parent.verticalCenter;
						width: constants._iSizeXL;
						height: width;
						cache: false;
						sourceSize.width: width;
						MouseArea{
							anchors.fill: avatar;
							enabled: obj.uid !== "";
							onClicked: {
								if(obj.uid !== "") controller._OpenUserPage(obj.uid);
							}
						}
					}
					Column{
						width: parent.width - parent.spacing - avatar.width;
						height: parent.height;
						clip: true;
						Row{
							width: parent.width;
							height: parent.height / 2;
							spacing: constants._iSpacingMedium;
							Text{
								id: unametext;
								height: parent.height;
								verticalAlignment: Text.AlignVCenter;
								font.pixelSize: constants._iFontLarge;
								elide: Text.ElideRight;
								clip: true;
								color: constants._cDarkestColor;
								text: obj.uname;
							}
							Text{
								id: sex;
								height: parent.height;
								verticalAlignment: Text.AlignVCenter;
								font.pixelSize: constants._iFontLarge;
								visible: text !== "";
								clip: true;
							}
							LabelWidget{
								id: levelinfo;
								property int level: 0;
								anchors.verticalCenter: parent.verticalCenter;
								sText: "UP" + levelinfo.level;
							}
						}
						Row{
							id: infolayout;
							width: parent.width;
							height: parent.height / 2;
							spacing: constants._iSpacingXXL;
							clip: true;
							Repeater{
								id: infolist;
								delegate: Component{
									Row{
										height: infolayout.height;
										clip: true;
										spacing: constants._iSpacingMedium;
										Text{
											height: parent.height;
											verticalAlignment: Text.AlignVCenter;
											font.pixelSize: constants._iFontLarge;
											elide: Text.ElideRight;
											color: constants._cDarkerColor;
											text: modelData.name;
										}
										Text{
											height: parent.height;
											verticalAlignment: Text.AlignVCenter;
											font.pixelSize: constants._iFontLarge;
											elide: Text.ElideRight;
											color: constants._cDarkestColor;
											text: modelData.value;
										}
									}
								}
							}
						}
					}
				}

				Text{
					id: official_verify;
					property string official;
					anchors.horizontalCenter: parent.horizontalCenter;
					width: parent.width - constants._iSpacingXXL * 2;
					visible: official !== "";
					clip: true;
					font.pixelSize: constants._iFontLarge;
					wrapMode: Text.WordWrap;
					color: constants._cDarkestColor;
					text: qsTr("Bilibili verify") + ": " + official;
				}
			}

			Flickable{
				id: flick;
				anchors.top: infocol.bottom;
				anchors.topMargin: constants._iSpacingXXL;
				anchors.left: parent.left;
				anchors.right: parent.right;
				anchors.bottom: parent.bottom;
				contentWidth: width;
				contentHeight: desclayout.height;
				clip: true;
				Column{
					id: desclayout;
					anchors.horizontalCenter: parent.horizontalCenter;
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
					Text{
						id: area;
						anchors.horizontalCenter: parent.horizontalCenter;
						width: parent.width - constants._iSpacingXXL * 2;
						verticalAlignment: Text.AlignVCenter;
						font.pixelSize: constants._iFontLarge;
						elide: Text.ElideRight;
						color: constants._cDarkerColor;
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
				}
			}
			ScrollDecorator{
				flickableItem: flick;
			}
		}


		LiveListWidget{
			id: recommendview;
			anchors.fill: parent;
			onRefresh: {
				obj._GetRecommend();
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
