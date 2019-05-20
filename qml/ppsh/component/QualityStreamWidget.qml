import QtQuick 1.1
import com.nokia.meego 1.1
import "../../js/util.js" as Util

Item{
	id: root;
	objectName: "idQualityStreamWidget";
	property alias qualityModel: qualityview.model;
	property alias qualityCount: qualityview.count;
	property alias qualityCurrentIndex: qualityview.currentIndex;
	property alias streamModel: view.model;
	property alias streamCount: view.count;
	property string rid;

	signal quality(string value);
	signal refresh;
	signal clicked(int index, string quality);

	clip: true;

	function _SetCurrentQuality(qn)
	{
		for(var i = 0; i < Util.ModelSize(qualityview.model); i++)
		{
			if(Util.ModelGet(qualityview.model, i).value == qn)
			{
				qualityview.currentIndex = i;
				return;
			}
		}
		qualityview.currentIndex = 0;
	}

	TabListWidget{
		id: qualityview;
		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.right: parent.right;
		height: constants._iSizeXL;
		onClicked: {
			root.quality(value);
		}
	}

	Rectangle{
		id: line;
		anchors.top: qualityview.bottom;
		anchors.topMargin: constants._iSpacingSmall;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.leftMargin: constants._iSpacingLarge;
		anchors.rightMargin: constants._iSpacingLarge;
		height: constants._iSpacingMicro;
		color: constants._cLighterColor;
		z: 1;
	}
	Item{
		id: episodeview;
		anchors.top: line.bottom;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.bottom: parent.bottom;
		anchors.topMargin: constants._iSpacingSmall;

		Text{
			anchors.fill: parent;
			horizontalAlignment: Text.AlignHCenter;
			verticalAlignment: Text.AlignVCenter;
			font.bold: true;
			font.pixelSize: constants._iFontSuper;
			elide: Text.ElideRight;
			clip: true;
			color: constants._cLightColor;
			text: qsTr("No content");
			visible: view.count === 0;
			MouseArea{
				anchors.centerIn: parent;
				width: parent.paintedWidth;
				height: parent.paintedHeight;
				onClicked: root.refresh();
			}
		}

		ListView{
			id: view;
			anchors.fill: parent;
			clip: true;
			z: 1;
			visible: count > 0;
			model: ListModel{}
			header: Component{
				RefreshWidget{
					onRefresh: root.refresh();
				}
			}
			delegate: Component{
				Item{
					id: viewdelegateroot;
					width: ListView.view.width;
					height: constants._iSizeXXL;
					MouseArea{
						anchors.fill: parent;
						onClicked: {
							view.currentIndex = index;
							root.clicked(index, model.quality);
						}
					}
					Rectangle{
						anchors.fill: parent;
						anchors.margins: border.width;
						color: constants._cTransparent;
						radius: 10;
						smooth: true;
						border.width: 4;
						border.color: parent.ListView.isCurrentItem ? constants._cGlobalColor : constants._cTransparent;

						Row{
							anchors.fill: parent;
							anchors.margins: constants._iSpacingLarge;
							clip: true;
							Item{
								id: pages;
								width: constants._iSizeMedium;
								height: parent.height;
								clip: true;
								Text{
									anchors.fill: parent;
									horizontalAlignment: Text.AlignHCenter;
									verticalAlignment: Text.AlignVCenter;
									font.pixelSize: constants._iFontXL;
									text: model.page;
								}
							}
							Text{
								width: parent.width - pages.width;
								height: parent.height;
								//horizontalAlignment: Text.AlignHCenter;
								verticalAlignment: Text.AlignVCenter;
								text: model.name;
								font.pixelSize: constants._iFontLarge;
								elide: Text.ElideRight;
								color: constants._cDarkestColor;
								wrapMode: Text.WrapAnywhere;
								maximumLineCount: 2;
							}
						}
					}
				}
			}
		}

		ScrollDecorator{
			flickableItem: view;
		}
	}
}
