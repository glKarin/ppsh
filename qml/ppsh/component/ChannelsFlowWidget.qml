import QtQuick 1.1
import com.nokia.meego 1.1
import "../../js/util.js" as Util

Item{
	id: root;
	property alias model: view.model;
	property alias count: view.count;
	signal clicked(string name, string value, string pvalue);
	signal headerClicked(string name, string value, string pvalue);
	signal refresh;
	objectName: "idChannelsFlowWidget";

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
		anchors.fill: parent;
		clip: true;
		z: 1;
		visible: count > 0;
		model: [];
		header: Component{
			RefreshWidget{
				onRefresh: root.refresh();
			}
		}
		delegate: Component{
			Item{
				id: viewdelegateroot;
				width: ListView.view.width;
				height: childrenRect.height;

				Column{
					anchors.top: parent.top;
					anchors.left: parent.left;
					anchors.right: parent.right;
					anchors.leftMargin: constants._iSpacingLarge;
					anchors.rightMargin: constants._iSpacingLarge;
					spacing: constants._iSpacingMedium;
					Item{
						width: parent.width;
						height: childrenRect.height;
						SectionWidget{
							id: cname;
							anchors.top: parent.top;
							anchors.left: parent.left;
							anchors.right: parent.right;
							sText: modelData.name;
							onClicked: root.headerClicked(modelData.name, modelData.rid, modelData.pid);
						}
						Flow{
							id: sublayout;
							anchors.top: cname.bottom;
							anchors.left: parent.left;
							anchors.right: parent.right;
							spacing: constants._iSpacingLarge;
							Repeater{
								model: modelData.children;
								delegate: Component{
									Item{
										width: childrenRect.width;
										height: childrenRect.height;
										clip: true;

										Text{
											text: modelData.name;
											font.pixelSize: constants._iFontXL;
											elide: Text.ElideRight;
											color: constants._cDarkestColor;
										}
										MouseArea{
											anchors.fill: parent;
											onClicked: {
												root.clicked(modelData.name, modelData.rid, modelData.pid);
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}

	ScrollDecorator{
		flickableItem: view;
	}
}
