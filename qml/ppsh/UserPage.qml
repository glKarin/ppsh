import QtQuick 1.1
import com.nokia.meego 1.1
import "component"
import "../js/main.js" as Script
import "../js/util.js" as Util

BasePage {
	id: root;

	property int __duration: 180;
	sTitle: qsTr("User");
	objectName: "idUserPage";

	function _Init(id)
	{
		if(!id) return;
		obj.uid = id;
		obj._GetAll();
	}

	QtObject{
		id: obj;
		property string uid;
		property string tid;
		property string keyword;
		property int pageNo: 1;
		property int pageSize: 20;
		property int pageCount: 0;
		property int totalCount: 0;
		property string order: typerow.vCurrentValue;
		property bool viewList: false;

		function _GetUserVideos(id, p, t)
		{
			var nid = id ? id : "0";

			if(tid !== nid)
			{
				tid = nid;
				typerow.iCurrentIndex = 0;
				order = typerow.aOptions[0].value;
			}

			root.bBusy = true;

			var pn;
			if(typeof(p) === "number") d.pn = p;
			else if(p === constants._sNextPage) pn = pageNo + 1;
			else if(p === constants._sPrevPage) pn = pageNo - 1;
			else if(p === constants._sThisPage) pn = pageNo;
			else pn = 1;
			var d = {
				uid: uid,
				tid: tid,
				model: view.model,
				pageNo: pn,
				pageSize: pageSize,
				order: order,
			};
			if(keyword !== "") d.keyword = keyword;

			if(p === undefined)
			{
				pageNo = 1;
				pageSize = 20;
				pageCount = 0;
				totalCount = 0;
			}
			Util.ModelClear(view.model);
			descview.contentY = 0;

			if(t)
			{
				pageNo = 1;
				pageSize = 20;
				pageCount = 0;
				totalCount = 0;
				typerow.iCurrentIndex = 0;
				order = typerow.aOptions[0].value;
				d.tmodel = tview.model;
				Util.ModelClear(tview.model);
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

			Script.GetUserVideos(d, s, f);
		}

		function _GetAll()
		{
			viewList = false;
			pageNo = 1;
			pageSize = 20;
			pageCount = 0;
			totalCount = 0;
			typerow.iCurrentIndex = 0;
			order = typerow.aOptions[0].value;
			Util.ModelClear(tview.model);
			Util.ModelClear(view.model);

			_GetUserDetail();
		}

		function _GetUserDetail()
		{
			if(uid == "") return;

			root.bBusy = true;

			uname.text = "";
			uidtext.text = "";
			preview.source = "";
			avatar.source = "";
			sex.text = "";
			sign.text = "";
			levelinfo.level = 0;
			official_verify.official = "";
			infolist.model = [];
			vipinfo.vip = 0;
			Util.ModelClear(tabbar.model);

			var d = {
				uid: uid,
			};

			var s = function(data){
				if(data)
				{
					uname.text = data.up;
					uidtext.text = data.uid !== "" ? "UID: " + data.uid : "";
					preview.source = data.preview;
					avatar.source = data.avatar;
					sex.text = data.sex;
					sign.text = data.sign;
					levelinfo.level = data.current_level;
					official_verify.official = data.official;
					vipinfo.vip = data.vipType;
					infolist.model = [
						{ name: qsTr("following"), value: Util.FormatCount(data.following), },
						{ name: qsTr("follower"), value: Util.FormatCount(data.follower), },
					];
					Util.ModelPush(tabbar.model, {
						name: qsTr("Archive") + "[" + data.archive_count + "]",
						value: "archive",
					});
					Util.ModelPush(tabbar.model, {
						name: qsTr("Article") + "[" + data.article_count + "]",
						value: "article",
					});
				}

				root.bBusy = false;
			};
			var f = function(err){
				root.bBusy = false;
				controller._ShowMessage(err);
			};

			Script.GetUserDetail(d, s, f);
		}

	}

	Image{
		id: preview;
		property int initHeight: constants._iSizeTooBig;
		property int theight: constants._iSizeXXXL;
		anchors.top: parent.top;
		anchors.horizontalCenter: parent.horizontalCenter;
		height: Math.max(initHeight, initHeight - descview.contentY);
		width: parent.width * (height / initHeight);
		fillMode: Image.PreserveAspectCrop;
		clip: true;
		cache: false;
		z: 1;
		state: obj.viewList ? constants._sHideState : constants._sShowState;
		states: [
			State{
				name: constants._sShowState;
				PropertyChanges{
					target: preview;
					initHeight: constants._iSizeTooBig;
				}
			},
			State{
				name: constants._sHideState;
				PropertyChanges{
					target: preview;
					initHeight: theight;
				}
			}
		]

		transitions: [
			Transition{
				from: constants._sHideState;
				to: constants._sShowState;
				NumberAnimation{
					target: preview;
					property: "initHeight";
					duration: root.__duration;
					easing.type: Easing.OutExpo;
				}
			},
			Transition{
				from: constants._sShowState;
				to: constants._sHideState;
				NumberAnimation{
					target: preview;
					property: "initHeight";
					duration: root.__duration;
					easing.type: Easing.InExpo;
				}
			}
		]
		MouseArea{
			anchors.fill: parent;
			onClicked: {
				obj._GetAll();
			}
		}
	}
	Rectangle{
		id: avatarrect;
		anchors.verticalCenter: preview.bottom;
		anchors.left: parent.left;
		anchors.leftMargin: constants._iSpacingBig;
		width: constants._iSizeXXL;
		height: width;
		border.width: 4;
		border.color: constants._cThemeColor;
		color: constants._cTransparent;
		z: 2;
		Image{
			id: avatar;
			anchors.centerIn: parent;
			height: parent.height - parent.border.width;
			width: height;
			cache: false;
		}
	}

	Text{
		id: uname;
		anchors.top: preview.bottom;
		anchors.bottom: avatarrect.bottom;
		anchors.left: avatarrect.right;
		anchors.right: parent.right;
		anchors.leftMargin: constants._iSpacingBig;
		verticalAlignment: Text.AlignVCenter;
		font.pixelSize: constants._iFontXXL;
		elide: Text.ElideRight;
		clip: true;
		font.bold: true;
		color: constants._cThemeColor;
	}
	MouseArea{
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.top: preview.bottom;
		anchors.bottom: avatarrect.bottom;
		onClicked: {
			obj.viewList = !obj.viewList; // false
		}
	}

	Flickable{
		id: descview;
		anchors.top: preview.top;
		anchors.topMargin: preview.initHeight + avatarrect.height / 2;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.bottom: parent.bottom;
		contentWidth: width;
		contentHeight: desclayout.height;
		clip: true;
		interactive: !obj.viewList;

		Column{
			id: desclayout;
			width: parent.width;
			Column{
				id: infocol;
				property int theight: childrenRect.height;
				width: parent.width;
				anchors.horizontalCenter: parent.horizontalCenter;
				state: obj.viewList ? constants._sHideState : constants._sShowState;
				spacing: constants._iSpacingSmall;
				clip: true;
				states: [
					State{
						name: constants._sShowState;
						PropertyChanges{
							target: infocol;
							height: theight;
						}
					},
					State{
						name: constants._sHideState;
						PropertyChanges{
							target: infocol;
							height: up.height;
						}
					}
				]

				transitions: [
					Transition{
						from: constants._sHideState;
						to: constants._sShowState;
						NumberAnimation{
							target: infocol;
							property: "height";
							duration: root.__duration;
							easing.type: Easing.OutExpo;
						}
					},
					Transition{
						from: constants._sShowState;
						to: constants._sHideState;
						NumberAnimation{
							target: infocol;
							property: "height";
							duration: root.__duration;
							easing.type: Easing.InExpo;
						}
					}
				]
				Row{
					id: up;
					anchors.horizontalCenter: parent.horizontalCenter;
					width: parent.width - constants._iSpacingXXL * 2;
					height: constants._iSizeXL;
					z: 1;
					spacing: constants._iSpacingXXL;
					Text{
						id: sex;
						height: parent.height;
						verticalAlignment: Text.AlignVCenter;
						font.pixelSize: constants._iFontLarge;
						//elide: Text.ElideRight;
						clip: true;
					}
					LVWidget{
						id: levelinfo;
						anchors.verticalCenter: parent.verticalCenter;
					}
					Text{
						id: uidtext;
						height: parent.height;
						verticalAlignment: Text.AlignVCenter;
						font.pixelSize: constants._iFontLarge;
						//elide: Text.ElideRight;
						color: constants._cDarkestColor;
						clip: true;
					}
					Rectangle{
						id: vipinfo;
						property int vip: 0;
						anchors.verticalCenter: parent.verticalCenter;
						height: constants._iSizeSmall;
						width: childrenRect.width;
						color: constants._cThemeColor;
						visible: vip > 0;
						clip: true;
						radius: 4;
						smooth: true;
						Text{
							height: parent.height;
							verticalAlignment: Text.AlignVCenter;
							font.bold: true;
							font.pixelSize: constants._iFontLarge;
							color: "#ffffff";
							clip: true;
							text: constants._GetVipName(vipinfo.vip);
						}
					}
				}

				ListView{
					id: infolist;
					anchors.horizontalCenter: parent.horizontalCenter;
					width: parent.width - constants._iSpacingXXL * 2;
					height: constants._iSizeLarge;
					interactive: false;
					spacing: constants._iSpacingXXL;
					clip: true;
					orientation: ListView.Horizontal;
					delegate: Component{
						Row{
							height: ListView.view.height;
							clip: true;
							spacing: constants._iSpacingMedium;
							Text{
								height: parent.height;
								verticalAlignment: Text.AlignVCenter;
								font.pixelSize: constants._iFontLarge;
								elide: Text.ElideRight;
								color: constants._cDarkestColor;
								text: modelData.value;
							}
							Text{
								height: parent.height;
								verticalAlignment: Text.AlignVCenter;
								font.pixelSize: constants._iFontLarge;
								elide: Text.ElideRight;
								color: constants._cDarkerColor;
								text: modelData.name;
							}
						}
					}
				}

				Text{
					id: official_verify;
					property string official;
					anchors.horizontalCenter: parent.horizontalCenter;
					width: parent.width - constants._iSpacingXXL * 2;
					height: visible ? constants._iSizeMedium : 0;
					visible: official !== "";
					clip: true;
					font.pixelSize: constants._iFontLarge;
					wrapMode: Text.WordWrap;
					color: constants._cDarkestColor;
					text: qsTr("Bilibili verify") + ": " + official;
				}

				Text{
					id: sign;
					anchors.horizontalCenter: parent.horizontalCenter;
					width: parent.width - constants._iSpacingXXL * 2;
					verticalAlignment: Text.AlignVCenter;
					font.pixelSize: constants._iFontLarge;
					//elide: Text.ElideRight;
					wrapMode: Text.WrapAnywhere;
					clip: true;
					color: constants._cDarkerColor;
				}
			}

			TabListWidget{
				id: tabbar;
				width: parent.width
				height: visible ? constants._iSizeXL : 0;
				cLineColor: constants._cThemeColor;
				bTabMode: true;
				bInteractive: false;
				visible: view.count === 0 && !obj.viewList;
				MouseArea{
					anchors.fill: parent;
					onClicked: {
						obj.viewList = true;
						obj._GetUserVideos("0", undefined, true);
					}
				}
			}

			Column{
				id: videoview;
				width: parent.width;
				anchors.horizontalCenter: parent.horizontalCenter;
				height: obj.viewList ? descview.height - infocol.height : childrenRect.height;
				visible: view.count > 0 || obj.viewList;
				TabListWidget{
					id: tview;
					width: parent.width;
					anchors.horizontalCenter: parent.horizontalCenter;
					height: constants._iSizeLarge;
					cLineColor: constants._cThemeColor;
					onClicked: {
						obj._GetUserVideos(value);
					}
				}
				TypeRowWidget{
					id: typerow;
					width: parent.width;
					anchors.horizontalCenter: parent.horizontalCenter;
					sText: "order";
					aOptions: [
						{
							name: qsTr("Update"),
							value: "update",
						},
						{
							name: qsTr("Click"),
							value: "click",
						},
						{
							name: qsTr("Stow"),
							value: "stow",
						},
					]
					vCurrentValue: "update";
					onSelected: {
						obj.order = value;
						obj._GetUserVideos();
					}
				}

				VideoListWidget{
					id: view;
					width: parent.width;
					anchors.horizontalCenter: parent.horizontalCenter;
					height: obj.viewList ? parent.height - tview.height - typerow.height : count * iDelegateHeight;
					interactive: obj.viewList;
					onRefresh: {
						obj._GetUserVideos(undefined, constants._sThisPage);
					}
				}
			}
		}
	}

	ScrollDecorator{
		flickableItem: descview;
	}

	ToolBarLayout{
		visible: videoview.visible;
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
				obj._GetUserVideos(undefined, constants._sPrevPage);
			}
		}
		IconWidget{
			iconId: "toolbar-next";
			enabled: obj.pageNo < obj.pageCount;
			onClicked: {
				obj._GetUserVideos(undefined, constants._sNextPage);
			}
		}
	}

	ToolIcon{
		anchors.left: parent.leftt;
		anchors.top: parent.top;
		opacity: 0.6;
		iconId: "toolbar-back";
		z: 10;
		onClicked: {
			pageStack.pop();
		}
	}

}
