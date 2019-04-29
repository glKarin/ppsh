import QtQuick 1.1
import com.nokia.meego 1.1

Item {
	id: root;

	property alias sText: label.text;
	property alias cTextColot: label.color;
	property alias cColor: line.color;
	property int iMargins: constants._iSpacingLarge;
	property string eTextAlign: constants._sAlignLeft;
	signal clicked(variant mouse);

	objectName: "idSectionWidget";
	width: parent.width;
	height: constants._iSizeLarge;
	clip: true;

	Item{
		anchors.top: parent.top;
		anchors.bottom: line.top;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.leftMargin: root.iMargins;
		anchors.rightMargin: root.iMargins;
		z: 1;
		clip: true;
		Text{
			id: label;
			anchors.fill: parent;
			verticalAlignment: Text.AlignVCenter;
			horizontalAlignment: root.eTextAlign === constants._sAlignCenter ? Text.AlignHcenter : (root.eTextAlign === constants._sAlignRight ? Text.AlignRight : Text.AlignLeft);
			font.pixelSize: constants._iFontXL;
			font.bold: true;
			color: constants._cDarkColor;
		}
	}

	Rectangle{
		id: line;
		anchors.horizontalCenter: parent.horizontalCenter;
		anchors.bottom: parent.bottom;
		width: parent.width - root.iMargins * 2;
		height: 1;
		color: constants._cLightColor;
	}

	MouseArea{
		anchors.fill: parent;
		onClicked: root.clicked(mouse);
	}

}
