import QtQuick 1.1
import com.nokia.meego 1.1
import "../../js/util.js" as Util

Item{
	id: root;
	property bool editMode: false;
	signal clicked(string aid, variant data);
	signal imageClicked(string aid, variant data);
	signal longPressed(int index, variant data);
	objectName: "idMixedListDelegate";

	MouseArea{
		anchors.fill: parent;
		onClicked: {
			root.clicked(model.aid, model);
		}
		onPressAndHold: {
			root.longPressed(index, model);
		}
	}

	Image{
		id: preview;
		anchors.left: parent.left;
		anchors.leftMargin: constants._iSpacingLarge;
		anchors.verticalCenter: parent.verticalCenter;
		height: parent.height - constants._iSpacingMedium * 2;
		width: model.type == constants._eBangumiType ? Util.GetSize(height, 0, "4/3") : (model.type == constants._eUserType ? height : Util.GetSize(0, height, "4/3"));
		fillMode: Image.PreserveAspectCrop;
		clip: true
		source: model.preview;
		cache: false;
		sourceSize: Qt.size(width, height);
		MouseArea{
			anchors.fill: parent;
			onClicked: {
				root.imageClicked(model.aid, model);
			}
		}
	}
	Column{
		anchors.left: preview.right;
		anchors.leftMargin: constants._iSpacingMedium;
		anchors.right: parent.right;
		anchors.rightMargin: constants._iSpacingLarge;
		anchors.verticalCenter: parent.verticalCenter;
		height: preview.height;
		Text{
			width: parent.width;
			height: parent.height / 3 * 2;
			text: model.title;
			font.pixelSize: constants._iFontLarge;
			elide: Text.ElideRight;
			color: constants._cDarkestColor;
			wrapMode: Text.WrapAnywhere;
			maximumLineCount: 2;
			clip: true;
		}
		Column{
			width: parent.width;
			height: parent.height / 3;
			Text{
				width: parent.width;
				height: parent.height / 2;
				clip: true;
				verticalAlignment: Text.AlignVCenter;
				text: model.up;
				font.pixelSize: constants._iFontMedium;
				elide: Text.ElideRight;
				color: constants._cDarkColor;
			}
			Text{
				width: parent.width;
				height: parent.height / 2;
				clip: true;
				verticalAlignment: Text.AlignVCenter;
				//horizontalAlignment: Text.AlignRight;
				text: Util.FormatTimestamp(model.ts / 1000);
				font.pixelSize: constants._iFontSmall;
				elide: Text.ElideRight;
				color: constants._cDarkerColor;
			}
		}
	}

	ToolIcon{
		anchors.top: parent.top;
		anchors.right: parent.right;
		z: 1;
		enabled: root.editMode;
		visible: enabled;
		iconId: "toolbar-close";
		onClicked: {
			root.longPressed(index, model);
		}
	}

	Rectangle{
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
