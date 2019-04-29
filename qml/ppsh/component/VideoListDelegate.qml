import QtQuick 1.1
import com.nokia.meego 1.1
import "../../js/util.js" as Util

Item{
	id: root;
	signal clicked(string aid, variant data);
	signal imageClicked(string aid, variant data);
	signal longPressed(int index, variant data);
	objectName: "idVideoListDelegate";

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
		height: constants._iSizeXXXL - constants._iSpacingLarge * 2;
		width: Util.GetSize(0, height, "4/3");
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

		Rectangle{
			anchors.right: parent.right;
			anchors.bottom: parent.bottom;
			anchors.rightMargin: constants._iSpacingMedium;
			anchors.bottomMargin: constants._iSpacingSmall;
			width: constants._iSizeLarge;
			height: constants._iSizeSmall;
			radius: 2;
			color: constants._cDarkestColor;
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
				color: constants._cLightestColor;
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
			height: parent.height / 5 * 2.5;
			text: model.title;
			font.pixelSize: constants._iFontLarge;
			elide: Text.ElideRight;
			color: constants._cDarkestColor;
			wrapMode: Text.WrapAnywhere;
			maximumLineCount: 2;
			clip: true;
		}
		Item{
			width: parent.width;
			height: parent.height / 5 * 0.5;
		}
		Text{
			width: parent.width;
			height: parent.height / 5;
			verticalAlignment: Text.AlignBottom;
			text: model.up;
			font.pixelSize: constants._iFontMedium;
			elide: Text.ElideRight;
			color: constants._cDarkColor;
		}
		Row{
			width: parent.width;
			height: parent.height / 5;
			clip: true;
			spacing: constants._iSpacingXXL;
			Text{
				height: parent.height;
				verticalAlignment: Text.AlignBottom;
				visible: model.view_count !== undefined;
				text: qsTr("Play") + " " + (model.view_count ? Util.FormatCount(model.view_count) : "-");
				font.pixelSize: constants._iFontSmall;
				elide: Text.ElideRight;
				color: constants._cDarkerColor;
			}
			Text{
				height: parent.height;
				verticalAlignment: Text.AlignBottom;
				visible: model.danmu_count !== undefined;
				text: qsTr("Danmaku") + " " + (model.danmu_count ? Util.FormatCount(model.danmu_count) : "-");
				font.pixelSize: constants._iFontSmall;
				elide: Text.ElideRight;
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
