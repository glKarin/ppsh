import QtQuick 1.1

Rectangle{
	id: root;
	objectName: "idLabelWidget";
	property alias sText: title.text;
	property alias iPixelSize: title.font.pixelSize;

	height: constants._iSizeSmall;
	width: childrenRect.width;
	color: constants._cThemeColor;
	visible: title.text !== "";
	clip: true;
	radius: 4;
	smooth: true;
	Text{
		id: title;
		height: parent.height;
		verticalAlignment: Text.AlignVCenter;
		font.bold: true;
		font.pixelSize: constants._iFontLarge;
		color: "#ffffff";
		clip: true;
	}
}
