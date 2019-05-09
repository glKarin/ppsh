import QtQuick 1.1
import com.nokia.meego 1.1
import "../../js/util.js" as Util

Item{
	id: root;
	signal clicked(string sid);
	objectName: "iBangumiGridDelegate";

	MouseArea{
		anchors.fill: parent;
		onClicked: {
			root.clicked(model.sid);
		}
		onPressAndHold: {
			controller._CopyToClipboard(model.bangumi_id, "Bangumi Id");
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
			height: Util.GetSize(0, width, "4/3");
			source: model.preview;
			fillMode: Image.PreserveAspectCrop;
			clip: true;
			cache: false;
			sourceSize.width: width;
			LabelWidget{
				anchors.right: parent.right;
				anchors.top: parent.top;
				anchors.topMargin: constants._iSpacingSmall;
				anchors.rightMargin: constants._iSpacingSmall;
				sText: model.badge;
				iPixelSize: constants._iFontMedium;
			}
			Text{
				anchors.left: parent.left;
				anchors.bottom: parent.bottom;
				anchors.bottomMargin: constants._iSpacingSmall;
				anchors.leftMargin: constants._iSpacingSmall;
				width: parent.width;
				text: model.follow;
				font.pixelSize: constants._iFontMedium;
				elide: Text.ElideRight;
				color: "#ffffff";
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
				height: parent.height / 4 * 3;
				text: model.title;
				font.pixelSize: constants._iFontMedium;
				elide: Text.ElideRight;
				color: constants._cDarkestColor;
				wrapMode: Text.WrapAnywhere;
				maximumLineCount: 2;
			}
			Text{
				width: parent.width;
				height: parent.height / 4;
				verticalAlignment: Text.AlignBottom;
				text: model.index_show;
				font.pixelSize: constants._iFontSmall;
				elide: Text.ElideRight;
				color: constants._cLightColor;
			}
		}
	}
}
