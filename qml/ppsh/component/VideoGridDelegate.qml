import QtQuick 1.1
import com.nokia.meego 1.1
import "../../js/util.js" as Util

Item{
	id: root;
	signal clicked(string aid);
	signal imageClicked(string aid);
	objectName: "idVideoGridDelegate";

	MouseArea{
		anchors.fill: parent;
		onClicked: {
			root.clicked(model.aid);
		}
		onPressAndHold: {
			controller._CopyToClipboard(model.aid, "avId");
		}
	}

	Rectangle{
		anchors.fill: parent;
		anchors.margins: constants._iSpacingMedium;
		smooth: true;
		radius: 4;
		color: constants._cLightestColor;
		Image{
			id: preview;
			anchors.left: parent.left;
			anchors.right: parent.right;
			anchors.top: parent.top;
			height: Util.GetSize(width, 0, "16/9");
			source: model.preview;
			fillMode: Image.PreserveAspectCrop;
			clip: true;
			cache: false;
			sourceSize.width: width;
			Row{
				anchors.left: parent.left;
				anchors.right: parent.right;
				anchors.leftMargin: constants._iSpacingMedium;
				anchors.rightMargin: constants._iSpacingMedium;
				anchors.bottom: parent.bottom;
				height: constants._iSizeMedium;
				clip: true;
				spacing: constants._iSpacingLarge;
				Text{
					id: view_count;
					height: parent.height;
					verticalAlignment: Text.AlignVCenter;
					text: model.view_count ? Util.FormatCount(model.view_count) : "-";
					font.pixelSize: constants._iFontMedium;
					elide: Text.ElideRight;
					color: "#ffffff";
				}
				Text{
					id: danmu_count;
					height: parent.height;
					verticalAlignment: Text.AlignVCenter;
					text: model.danmu_count ? Util.FormatCount(model.danmu_count) : "-";
					font.pixelSize: constants._iFontMedium;
					elide: Text.ElideRight;
					color: "#ffffff";
				}
				Text{
					width: parent.width - view_count.width - danmu_count.width - parent.spacing * 2;
					height: parent.height;
					text: model.duration || "";
					verticalAlignment: Text.AlignVCenter;
					horizontalAlignment: Text.AlignRight;
					font.pixelSize: constants._iFontMedium;
					elide: Text.ElideRight;
					color: "#ffffff";
				}
			}
		}

		Column{
			anchors.left: parent.left;
			anchors.right: parent.right;
			anchors.top: preview.bottom;
			anchors.bottom: parent.bottom;
			anchors.margins: constants._iSpacingMedium;
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
			Row{
				width: parent.width;
				height: parent.height / 3;
				clip: true;
				spacing: constants._iSpacingMedium;
				Text{
					height: parent.height;
					verticalAlignment: Text.AlignBottom;
					text: qsTr("UP") + " · ";
					font.pixelSize: constants._iFontMedium;
					elide: Text.ElideRight;
					font.bold: true;
					color: constants._cDarkColor;
				}
				Text{
					height: parent.height;
					verticalAlignment: Text.AlignBottom;
					text: model.up;
					font.pixelSize: constants._iFontMedium;
					elide: Text.ElideRight;
					color: constants._cDarkerColor;
				}
			}
		}
	}
}

