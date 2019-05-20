import QtQuick 1.1
import com.nokia.meego 1.1
import "../../js/util.js" as Util

Item{
	id: root;
	signal clicked(string rid, variant data);
	signal imageClicked(string rid, variant data);
	objectName: "idLiveListDelegate";

	MouseArea{
		anchors.fill: parent;
		onClicked: {
			root.clicked(model.rid, model);
		}
	}

	Image{
		id: preview;
		anchors.left: parent.left;
		anchors.leftMargin: constants._iSpacingLarge;
		anchors.verticalCenter: parent.verticalCenter;
		height: parent.height - constants._iSpacingMedium * 2;
		width: Util.GetSize(0, height, "4/3");
		fillMode: Image.PreserveAspectCrop;
		clip: true
		source: model.preview;
		cache: false;
		sourceSize: Qt.size(width, height);
		MouseArea{
			anchors.fill: parent;
			onClicked: {
				root.imageClicked(model.rid, model);
			}
		}

		Rectangle{
			anchors.right: parent.right;
			anchors.bottom: parent.bottom;
			anchors.rightMargin: constants._iSpacingMedium;
			anchors.bottomMargin: constants._iSpacingSmall;
			width: constants._iSizeLarge;
			height: constants._iSizeSmall;
			radius: 2;
			color: "#000000";
			clip: true;
			smooth: true;
			opacity: 0.8;
			visible: model.duration ? true : false;
			Text{
				anchors.fill: parent;
				text: model.duration || "";
				verticalAlignment: Text.AlignVCenter;
				horizontalAlignment: Text.AlignHCenter;
				font.pixelSize: constants._iFontSmall;
				elide: Text.ElideRight;
				color: "#ffffff";
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
				verticalAlignment: Text.AlignVCenter;
				elide: Text.ElideRight;
				text: qsTr("Online") + " " + (model.online ? Util.FormatCount(model.online) : "-");
				font.pixelSize: constants._iFontSmall;
				color: constants._cDarkerColor;
			}
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
