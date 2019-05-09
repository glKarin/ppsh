import QtQuick 1.1
import com.nokia.meego 1.1
import "../../js/util.js" as Util

Item{
	id: root;
	property bool editMode: false;
	signal clicked(string mid, variant data);
	signal imageClicked(string mid, variant data);
	signal longPressed(int index, variant data);
	objectName: "idResultListDelegate";

	MouseArea{
		anchors.fill: parent;
		onClicked: {
			root.clicked(model.mid, model);
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
				root.imageClicked(model.mid, model);
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
			visible: model.sign ? true : false;
			Text{
				anchors.fill: parent;
				text: model.sign || "";
				verticalAlignment: Text.AlignVCenter;
				horizontalAlignment: Text.AlignHCenter;
				font.pixelSize: constants._iFontSmall;
				elide: Text.ElideRight;
				color: "#ffffff";
			}
		}
	}
	Item{
		anchors.left: preview.right;
		anchors.leftMargin: constants._iSpacingMedium;
		anchors.right: parent.right;
		anchors.rightMargin: constants._iSpacingLarge;
		anchors.verticalCenter: parent.verticalCenter;
		height: preview.height;
		Text{
			anchors.top: parent.top;
			anchors.left: parent.left;
			anchors.right: parent.right;
			anchors.bottom: extracol.top;
			text: model.title;
			font.pixelSize: constants._iFontLarge;
			elide: Text.ElideRight;
			color: constants._cDarkestColor;
			wrapMode: Text.WrapAnywhere;
			maximumLineCount: height > parent.height / 2 ? 2 : 1;
			clip: true;
			z: 1;
		}
		Column{
			id: extracol;
			anchors.bottom: parent.bottom;
			anchors.left: parent.left;
			anchors.right: parent.right;
			clip: true;
			Text{
				width: parent.width;
				text: model.subtitle;
				font.pixelSize: constants._iFontMedium;
				elide: Text.ElideRight;
				color: constants._cDarkColor;
			}
			Flow{
				id: extras;
				property variant __model: model.extras;
				width: parent.width;
				//height: childrenRect.height;
				spacing: constants._iSpacingXXL;
				Repeater{
					model: extras.__model;
					delegate: Component{
						Text{
							visible: model.value !== undefined;
							text: (model.name ? model.name + " " : "") + model.value + (model.unit ? " " + model.unit : "");
							font.pixelSize: constants._iFontSmall;
							color: constants._cDarkerColor;
						}
					}
				}
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
