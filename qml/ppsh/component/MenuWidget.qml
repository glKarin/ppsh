import QtQuick 1.1
import com.nokia.meego 1.1
import "../../js/util.js" as Util

Item{
	id: root;
	objectName: "idMenuWidget";
	property alias model: view.model;
	property alias sTitle: header.sText;
	property alias sIcon: icon.source;
	property int iMenuListWidth: width - constants._iSizeXXXL;
	property int iAnimInterval: 320;
	property alias tools: toolbarlayout.children;
	signal clicked(variant mouse);

	z: constants._iMenuZ;
	clip: true;
	visible: menu.x !== -iMenuListWidth;

	RectWidget{
		id: mask;
		anchors.fill: parent;
		visible: opacity !== 0;
		color: constants._cDarkestColor;
		state: menu.state;
		iStart: 0;
		iTarget: 0.6;
		sProperty: "opacity";
		iDuration: root.iAnimInterval;
		MouseArea{
			anchors.fill: parent;
			onClicked: {
				root._Toggle(false);
			}
		}
	}

	RectWidget{
		id: menu;
		anchors.top: parent.top;
		anchors.bottom: parent.bottom;
		width: root.iMenuListWidth;
		z: 1;
		color: constants._cLighterColor;
		state: constants._sHideState;
		iStart: -root.iMenuListWidth;
		iTarget: 0;
		sProperty: "x";
		iDuration: root.iAnimInterval;

		Header{
			id: header;
			iTextSize: constants._iFontTooBig;
			iTextMargin: constants._iSizeXXL;
			Image{
				id: icon;
				cache: false;
				anchors.left: parent.left;
				anchors.leftMargin: (header.iTextMargin - width) / 2;
				anchors.verticalCenter: parent.verticalCenter;
				z: 1;
				width: constants._iSizeXL;
				height: width;
				smooth: true;
			}
			onClicked: {
				root.clicked(mouse);
			}
			height: constants._iSizeXXXL;
			z: 1;
		}
		ListView{
			id: view;
			anchors.top: header.bottom;
			anchors.bottom: tb.top;
			anchors.left: parent.left;
			anchors.right: parent.right;
			anchors.topMargin: constants._iSpacingMedium;
			anchors.bottomMargin: constants._iSpacingMedium;
			clip: true;
			model: ListModel{}
			delegate: Component{
				Rectangle{
					id: viewdelegateroot;
					width: ListView.view.width;
					height: constants._iSizeXXL;
					color: controller._IsCurrentPage(model.name) ? constants._cDarkColor : constants._cLighterColor;
					MouseArea{
						anchors.fill: parent;
						onClicked: {
							root._Toggle(false);
							eval(model.func);
						}
					}

					Row{
						anchors.fill: parent;
						anchors.margins: constants._iSpacingLarge;
						spacing: constants._iSpacingXL;
						Image{
							id: icon;
							anchors.verticalCenter: parent.verticalCenter;
							height: Math.min(constants._iSizeLarge, parent.height);
							width: height;
							source: Util.HandleIconSource("toolbar-" + model.icon, constants._bInverted);
							smooth: true;
						}
						Text{
							anchors.verticalCenter: parent.verticalCenter;
							width: parent.width - icon.width - parent.spacing;
							height: parent.height;
							text: model.label;
							verticalAlignment: Text.AlignVCenter;
							font.pixelSize: constants._iFontXL;
							elide: Text.ElideRight;
							color: controller._IsCurrentPage(model.name) ? constants._cThemeColor : constants._cDarkestColor;
							clip: true;
						}
					}

					Rectangle{
						anchors.bottom: parent.bottom;
						anchors.left: parent.left;
						anchors.right: parent.right;
						anchors.leftMargin: constants._iSpacingLarge;
						anchors.rightMargin: constants._iSpacingLarge;
						height: constants._iSpacingMicro;
						color: constants._cLightColor;
						z: 1;
					}
				}
			}
		}

		Item{
			id: tb;
			anchors.bottom: parent.bottom;
			anchors.left: parent.left;
			anchors.right: parent.right;
			height: toolbarlayout.children.length > 0 ? constants._iSizeXL : 0;
			clip: true;
			ToolBarLayout{
				id: toolbarlayout;
				anchors.fill: parent;
			}
			Rectangle{
				anchors.top: parent.top;
				anchors.left: parent.left;
				anchors.right: parent.right;
				anchors.leftMargin: constants._iSpacingLarge;
				anchors.rightMargin: constants._iSpacingLarge;
				height: constants._iSpacingMicro;
				color: constants._cDarkColor;
				z: 1;
			}
		}

		ScrollDecorator{
			flickableItem: view;
		}
	}

	function _Toggle(on)
	{
		if(on === undefined)
		{
			if(menu.state === constants._sHideState) menu.state = constants._sShowState;
			else if(menu.state === constants._sShowState) menu.state = constants._sHideState;
		}
		else
		{
			menu.state = on ? constants._sShowState : constants._sHideState;
		}
	}
}
