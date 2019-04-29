import QtQuick 1.1
import com.nokia.meego 1.1
import "../../js/util.js" as Util

Item{
	id: root;
	property alias model: view.model;
	property alias count: view.count;
	property alias currentIndex: view.currentIndex;
	signal clicked(int index, variant data);
	objectName: "idStreamGridWidget";

	Text{
		anchors.fill: parent;
		horizontalAlignment: Text.AlignHCenter;
		verticalAlignment: Text.AlignVCenter;
		font.bold: true;
		font.pixelSize: constants._iFontXL;
		elide: Text.ElideRight;
		clip: true;
		color: constants._cLighterColor;
		text: qsTr("No content");
		visible: view.count === 0;
	}

	GridView{
		id: view;
		anchors.fill: parent;
		clip: true;
		visible: count > 0;
		model: [];
		cellWidth: parent.width / 2;
		cellHeight: constants._iSizeXL;
		delegate: Component{
			Item{
				id: viewdelegateroot;
				width: GridView.view.cellWidth;
				height: GridView.view.cellHeight;
				MouseArea{
					anchors.fill: parent;
					onClicked: {
						view.currentIndex = index;
						root.clicked(index, modelData);
					}
				}
				Rectangle{
					anchors.fill: parent;
					anchors.margins: border.width;
					color: parent.GridView.isCurrentItem ? constants._cLightestColor : constants._cTransparent;
					radius: 10;
					smooth: true;
					border.width: 4;
					border.color: parent.GridView.isCurrentItem ? constants._cThemeColor : constants._cTransparent;
					clip: true;

					Text{
						anchors.fill: parent;
						anchors.margins: constants._iSpacingLarge;
						horizontalAlignment: Text.AlignHCenter;
						verticalAlignment: Text.AlignVCenter;
						text: "" + index + ": " + modelData.name;
						font.pixelSize: constants._iFontLarge;
						elide: Text.ElideLeft;
						color: viewdelegateroot.GridView.isCurrentItem ? constants._cDarkestColor : constants._cLightestColor;
					}
				}
			}
		}
	}

	ScrollDecorator{
		flickableItem: view;
	}
}
