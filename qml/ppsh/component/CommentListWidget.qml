import QtQuick 1.1
import com.nokia.meego 1.1
import "../../js/util.js" as Util

Item{
	id: root;
	objectName: "idCommentListWidget";
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
				height: mainlayout.height + line.height + constants._iSpacingBig;

				MouseArea{
					anchors.fill: parent;
					onClicked: {
					}
					onPressAndHold: {
						controller._CopyToClipboard(model.content);
					}
				}

				Row{
					id: mainlayout;
					anchors.top: parent.top;
					anchors.left: parent.left;
					anchors.topMargin: constants._iSpacingBig;
					width: parent.width;
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
}
