import QtQuick 1.1
import com.nokia.meego 1.1
import "../../js/util.js" as Util
import "../../js/main.js" as Script

Item{
	id: root;
	objectName: "idCommentListWidget";
	property string aid;
	property string type: "1"; // 12 is article
	property alias model: view.model;
	property alias count: view.count;
	property alias bHasMore: view.hasMore;
	signal refresh;
	signal more;

	Text{
		anchors.fill: parent;
		horizontalAlignment: Text.AlignHCenter;
		verticalAlignment: Text.AlignVCenter;
		font.bold: true;
		font.pixelSize: constants._iFontSuper;
		elide: Text.ElideRight;
		clip: true;
		color: constants._cLightColor;
		text: qsTr("No content");
		visible: view.count === 0;
		MouseArea{
			anchors.centerIn: parent;
			width: parent.paintedWidth;
			height: parent.paintedHeight;
			onClicked: root.refresh();
		}
	}

	ListView{
		id: view;
		property bool hasMore: false;
		anchors.fill: parent;
		clip: true;
		z: 1;
		visible: count > 0;
		model: ListModel{}
		header: Component{
			RefreshWidget{
				onRefresh: root.refresh();
			}
		}
		footer: Component{
			FooterWidget{
				bEnabled: ListView.view.hasMore;
				onClicked: root.more();
			}
		}
		delegate: Component{
			Item{
				id: viewdelegateroot;
				width: ListView.view.width;
				height: mainlayout.height + line.height + constants._iSpacingBig * 2;

				Row{
					id: mainlayout;
					anchors.top: parent.top;
					anchors.left: parent.left;
					anchors.right: parent.right;
					anchors.topMargin: constants._iSpacingBig;
					height: childrenRect.height;
					Item{
						id: user;
						width: constants._iSizeXXL;
						height: parent.height;
						Image{
							id: avatar;
							anchors.top: parent.top;
							anchors.horizontalCenter: parent.horizontalCenter;
							width: constants._iSizeXL;
							height: width;
							source: model.avatar;
							cache: false;
							sourceSize.width: width;
						}
						MouseArea{
							anchors.fill: parent;
							onClicked: {
								controller._OpenUserPage(model.uid);
							}
						}
					}

					Column{
						width: parent.width - user.width;
						Row{
							width: parent.width;
							height: constants._iSizeLarge;
							Row{
								width: parent.width / 2;
								height: parent.height;
								spacing: constants._iSpacingXXL;
								z: 1;
								Text{
									height: parent.height;
									verticalAlignment: Text.AlignVCenter;
									text: model.name;
									font.pixelSize: constants._iFontLarge;
									color: constants._cDarkColor;
								}
								LVWidget{
									anchors.verticalCenter: parent.verticalCenter;
									level: model.level;
								}
							}

							Row{
								width: parent.width / 2;
								height: parent.height;
								spacing: constants._iSpacingXXL;
								layoutDirection: Qt.RightToLeft;
								Text{
									height: parent.height;
									verticalAlignment: Text.AlignVCenter;
									text: Util.FormatTimestamp(model.create_time);
									font.pixelSize: constants._iFontLarge;
									color: constants._cLightColor;
								}
								Text{
									height: parent.height;
									verticalAlignment: Text.AlignVCenter;
									text: model.floor !== undefined ? "#" + model.floor : "";
									font.pixelSize: constants._iFontLarge;
									color: constants._cLightColor;
									visible: model.floor !== undefined;
								}
							}
						}

						Text{
							width: parent.width;
							text: model.content;
							font.pixelSize: constants._iFontLarge;
							//elide: Text.ElideRight;
							color: constants._cDarkestColor;
							wrapMode: Text.WrapAnywhere;
							MouseArea{
								anchors.fill: parent;
								onPressAndHold: {
									controller._CopyToClipboard(model.content);
								}
							}
						}

						Row{
							width: parent.width;
							height: constants._iSizeLarge;
							spacing: constants._iSpacingSuper;
							Text{
								height: parent.height;
								verticalAlignment: Text.AlignVCenter;
								text: qsTr("Like") + " " + Util.FormatCount(model.like);
								font.pixelSize: constants._iFontLarge;
								color: constants._cLightColor;
							}
							Text{
								height: parent.height;
								verticalAlignment: Text.AlignVCenter;
								visible: model.reply_count > 0;
								text: qsTr("Reply") + " " + Util.FormatCount(model.reply_count);
								font.pixelSize: constants._iFontLarge;
								color: constants._cLightColor;
							}
						}
						Rectangle{
							width: parent.width;
							height: visible ? childrenRect.height : 0;
							visible: model.reply_count > 0;
							radius: 2;
							color: constants._cLighterColor;
							Column{
								id: replycol;
								property variant replyModel: model.reply;
								anchors.horizontalCenter: parent.horizontalCenter;
								anchors.top: parent.top;
								width: parent.width - constants._iSpacingLarge * 2;
								spacing: constants._iSpacingTiny;
								Repeater{
									model: replycol.replyModel;
									delegate: Component{
										Text{
											width: replycol.width;
											font.pixelSize: constants._iFontLarge;
											wrapMode: Text.WrapAnywhere;
											color: constants._cDarkerColor;
											text: "<a href='%1'>%2</a>: %3".arg(model.uid).arg(model.name).arg(model.content);
											MouseArea{
												anchors.fill: parent;
												onClicked: {
													controller._OpenUserPage(model.uid);
												}
												onPressAndHold: {
													controller._CopyToClipboard(model.content);
												}
											}
										}
									}
								}
								Text{
									width: replycol.width;
									visible: model.reply_count > 0 && model.reply_count > Util.ModelSize(model.reply);
									font.pixelSize: constants._iFontXL;
									elide: Text.ElideRight;
									color: constants._cDarkerColor;
									text: "<a href='%1'>%2 &gt;</a>".arg(obj.pageNo).arg(qsTr("View total") + " " + model.reply_count);
									onLinkActivated: {
										obj._GetReply(model.rpid, model.content, link);
									}
								}
							}
						}
					}
				}

				Rectangle{
					id: line;
					anchors.bottom: parent.bottom;
					anchors.left: parent.left;
					anchors.right: parent.right;
					anchors.leftMargin: constants._iSpacingLarge;
					anchors.rightMargin: constants._iSpacingLarge;
					height: constants._iSpacingMicro;
					color: constants._cLighterColor;
					z: 1;
				}
			}
		}
	}

	ScrollDecorator{
		flickableItem: view;
	}

	QtObject{
		id: obj;
		property string rid;
		property string content;
		property int pageNo: 1;
		property int pageSize: 20;
		property int pageCount: 0;
		property int totalCount: 0;
		property variant dialog: null;

		function _GetReply(id, name, p)
		{
			if(id !== undefined)
			{
				if(rid != id)
				{
					pageNo = 1;
					pageSize = 20;
					pageCount = 0;
					totalCount = 0;
					rid = id;
					content = name;
				}
			}

			if(rid == "") return;

			var pn;
			if(typeof(p) === "number") d.pn = p;
			else if(p === constants._sNextPage) pn = pageNo + 1;
			else if(p === constants._sPrevPage) pn = pageNo - 1;
			else if(p === constants._sThisPage) pn = pageNo;
			else if(p === constants._sLastPage) pn = pageCount;
			else pn = 1;

			var reply = [];
			var d = {
				aid: root.aid,
				model: reply,
				pageNo: pn,
				rid: rid,
				type: root.type,
			};

			var s = function(data){
				obj.pageNo = data.pageNo;
				obj.pageSize = data.pageSize;
				obj.pageCount = data.pageCount;
				obj.totalCount = data.totalCount;
				if(Util.ModelSize(reply) > 0)
				{
					var r = [];
					var Fmt = "<a href='%1'>%2</a>: %3";
					Util.ModelForeach(reply, function(e, i){
						var item = {
							text: Fmt.arg(e.uid).arg(e.name).arg(e.content),
						};
						r.push(item);
					});
					var title = "%1: %2(%3/%4)".arg(qsTr("Reply")).arg(obj.totalCount).arg(obj.pageNo).arg(obj.pageCount);
					var paged = obj.__MakePaged();
					if(!obj.dialog)
					{
						var diag = controller._Info(title, content, r, paged, function(link){
							if(obj.dialog) obj.dialog.accept();
							controller._OpenUserPage(link);
						},
						function(){
							obj._GetReply(undefined, undefined, constants._sThisPage);
						},
						function(link){
							obj._GetReply(undefined, undefined, link);
						}
					);
					obj.dialog = diag;
					diag.statusChanged.connect(function(){
						if(diag.status == DialogStatus.Closing)
						{
							obj.dialog = null;
							//console.log("Dialog closing");
						}
					});
				}
				else
				{
					obj.dialog._Set(title, content, r, paged);
				}
			}
		};
		var f = function(err){
			controller._ShowMessage(err);
		};

		Script.GetReply(d, s, f);
	}

	function __MakePaged()
	{
		var r = [];
		var Fmt = "<a href='%1'>%2</a>";
		var Fmt_Disable = "%1%2";
		if(obj.pageCount > 1)
		{
			r.push((obj.pageNo > 1 ? Fmt : Fmt_Disable).arg(obj.pageNo > 1 ? constants._sFirstPage : "").arg(qsTr("First")));
			r.push((obj.pageNo > 1 ? Fmt : Fmt_Disable).arg(obj.pageNo > 1 ? constants._sPrevPage : "").arg(qsTr("Previous")));
			r.push((obj.pageNo < obj.pageCount ? Fmt : Fmt_Disable).arg(obj.pageNo < obj.pageCount ? constants._sNextPage : "").arg(qsTr("Next")));
			r.push((obj.pageNo < obj.pageCount ? Fmt : Fmt_Disable).arg(obj.pageNo < obj.pageCount ? constants._sLastPage : "").arg(qsTr("Last")));
		}
		return r.join(" ");
	}
}

}
