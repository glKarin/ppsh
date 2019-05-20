import QtQuick 1.1
import com.nokia.meego 1.1
import "../../js/util.js" as Util

Item{
	id: root;
	signal clicked(string rid);
	signal imageClicked(string rid);
	objectName: "idLiveGridDelegate";

	MouseArea{
		anchors.fill: parent;
		onClicked: {
			root.clicked(model.rid);
		}
		onPressAndHold: {
			controller._CopyToClipboard(model.rid, qsTr("room Id"));
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
				height: constants._iSizeSmall;
				clip: true;
				Text{
					width: parent.width / 2;
					height: parent.height;
					text: model.uname;
					verticalAlignment: Text.AlignVCenter;
					font.pixelSize: constants._iFontMedium;
					elide: Text.ElideRight;
					color: "#ffffff";
				}
				Text{
					width: parent.width / 2;
					height: parent.height;
					verticalAlignment: Text.AlignVCenter;
					horizontalAlignment: Text.AlignRight;
					text: qsTr("Online") + " " + (model.online ? Util.FormatCount(model.online) : "-");
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
			Text{
				width: parent.width;
				height: parent.height / 3;
				verticalAlignment: Text.AlignBottom;
				text: model.area;
				font.pixelSize: constants._iFontMedium;
				elide: Text.ElideRight;
				color: constants._cDarkColor;
			}
		}
	}
}
