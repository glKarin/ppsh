import QtQuick 1.1
import com.nokia.meego 1.1

HarmattanCommonDialog {
	id: root;

	objectName: "idInfoDialog";

	property bool bAutoOpen: true;
	property alias sTitle: layout.sTitle;
	property alias aTexts: layout.aTexts;
	property alias vNu: layout.vNu;
	property variant sBottomTitle: "";
	signal linkClicked(string link);
	signal clicked;

	content: Item {
		id: contentField;
		width: root.width;
		height: Math.min(root.__contentHeight, root.__maxContentHeight);
		Flickable{
			id: flickable;
			anchors.fill: parent;
			clip: true;
			contentWidth: width;
			contentHeight: layout.height;
			TextListWidget{
				id: layout;
				width: parent.width;
				onClicked: root.clicked();
				iPixelSize: constants._iFontXL;
				cColor: "#ffffff";
				onLinkClicked: root.linkClicked(link);
			}
		}

		ScrollDecorator{
			flickableItem: flickable;
		}

	}

	__drawFooterLine: sBottomTitle !== "";
	tools: [
		Text{
			width: root.width;
			height: constants._iSizeLarge;
			horizontalAlignment: Text.AlignHCenter;
			verticalAlignment: Text.AlignVCenter;
			text: root.sBottomTitle;
			color: constants._cLightestColor;
			font.bold: true;
			font.pixelSize: constants._iFontXL;
			wrapMode: Text.WordWrap;
			elide: Text.ElideRight;
			maximumLineCount: 2;
			onLinkActivated: {
				if(link !== "") eval(link);
			}
		}
	]

	Component.onCompleted: {
		if(bAutoOpen) open();
	}
}
