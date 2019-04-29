import QtQuick 1.1
import com.nokia.meego 1.1
import "../../js/util.js" as Util

Item{
	id: root;
	property alias model: view.model;
	property alias count: view.count;
	signal refresh;
	objectName: "idVideoGridWidget";

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
	}

	GridView{
		id: view;
		anchors.fill: parent;
		clip: true;
		visible: count > 0;
		cellWidth: width / 2;
		cellHeight: cellWidth; //Util.GetSize(cellWidth, 0, "4/3");
		model: ListModel{}
		header: Component{
			RefreshWidget{
				onRefresh: root.refresh();
			}
		}
		delegate: Component{
			VideoGridDelegate{
				width: GridView.view.cellWidth;
				height: GridView.view.cellHeight;
				onClicked: {
					controller._OpenDetailPage(aid);
				}
			}
		}
	}

	ScrollDecorator{
		flickableItem: view;
	}
}
