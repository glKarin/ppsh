import QtQuick 1.1
import com.nokia.meego 1.1
import "../../js/util.js" as Util

Item{
	id: root;
	property alias model: view.model;
	property alias count: view.count;
	property color cLineColor: constants._cDarkerColor;
	property bool bInteractive: true;
	property bool bTabMode: false;
	property int iTopMargin: 0;
	property int iBottomMargin: 0;
	signal clicked(string name, string value);
	objectName: "idTabListWidget";

	ListView{
		id: view;
		anchors.fill: parent;
		anchors.topMargin: root.iTopMargin;
		anchors.bottomMargin: root.iBottomMargin;
		clip: true;
		orientation: ListView.Horizontal;
		interactive: !root.bTabMode;
		model: ListModel{}
		delegate: Component{
			Item{
				id: delegateroot;
				property int iLineWidth: constants._iSpacingMedium;
				width: root.bTabMode ? (ListView.view.count > 0 ? ListView.view.width / ListView.view.count : ListView.view.width) : constants._iSizeXXXL;
				height: ListView.view.height;
				clip: true;
				//color: constants._cLightestColor;
				MouseArea{
					anchors.fill: parent;
					onClicked: {
						if(root.bInteractive)
						{
							view.currentIndex = index;
							root.clicked(model.name, model.value);
						}
					}
				}
				Rectangle{
					anchors.top: parent.top;
					anchors.left: parent.left;
					anchors.right: parent.right;
					anchors.topMargin: border.width / 2;
					anchors.leftMargin: border.width / 2;
					anchors.rightMargin: border.width / 2;
					height: parent.height + border.width;
					border.width: iLineWidth;
					visible: delegateroot.ListView.isCurrentItem;
					border.color: root.cLineColor;
					radius: border.width;
					smooth: true;
					color: constants._cTransparent;
				}
				Text{
					anchors.fill: parent;
					anchors.margins: iLineWidth;
					horizontalAlignment: Text.AlignHCenter;
					verticalAlignment: Text.AlignVCenter;
					elide: Text.ElideLeft;
					color: delegateroot.ListView.isCurrentItem ? constants._cDarkestColor : constants._cDarkerColor;
					font.pixelSize: constants._iFontXL;
					font.bold: delegateroot.ListView.isCurrentItem;
					text: model.name;
				}
				Rectangle{
					anchors.bottom: parent.bottom;
					anchors.left: parent.left;
					anchors.right: parent.right;
					height: iLineWidth;
					color: root.cLineColor;
					visible: !delegateroot.ListView.isCurrentItem;
				}
			}
		}
	}
}
