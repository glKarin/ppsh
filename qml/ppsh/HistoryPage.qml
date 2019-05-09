import QtQuick 1.1
import com.nokia.meego 1.1
import "component"
import "../js/main.js" as Script
import "../js/util.js" as Util

BasePage {
	id: root;

	sTitle: qsTr("View history") + (obj.editMode ? " - " + qsTr("editing...") : "[" + view.count + "]");
	objectName: "idHistoryPage";

	Header{
		id: header;
		sText: root.sTitle;
		iTextMargin: back.width;
		onClicked: {
			if(!obj.editMode) obj._GetViewHistory();
		}
		ToolIcon{
			id: back;
			anchors.left: parent.left;
			anchors.verticalCenter: parent.verticalCenter;
			iconId: "toolbar-back";
			onClicked: pageStack.pop();
		}
		ToolIcon{
			anchors.right: parent.right;
			anchors.verticalCenter: parent.verticalCenter;
			iconId: obj.editMode ? "toolbar-done" : "toolbar-edit";
			visible: enabled;
			enabled: view.count > 0 || obj.editMode;
			onClicked: {
				obj.editMode = !obj.editMode;
				if(obj.editMode) controller._ShowMessage(qsTr("Hold item to remove it"));
				else obj._GetViewHistory();
			}
		}
	}

	function _Init()
	{
		obj._GetViewHistory();
	}

	QtObject{
		id: obj;
		property bool editMode: false;

		function _GetViewHistory(type)
		{
			root.bBusy = true;
			Util.ModelClear(view.model);
			Script.GetViewHistory(view.model);
			root.bBusy = false;
		}

		function _RemoveViewHistory(index, tid)
		{
			Script.RemoveViewHistory(tid);
			Util.ModelRemove(view.model, index);
		}
	}


	MixedListWidget{
		id: view;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.top: header.bottom;
		anchors.bottom: parent.bottom;
		bEditMode: obj.editMode;
		onRefresh: {
			obj.editMode = false;
			obj._GetViewHistory();
		}
		onLongPressed: {
			obj._RemoveViewHistory(index, data.tid);
		}
	}

	onStatusChanged: {
		if(status === PageStatus.Active)
		{
			obj.editMode = false;
			obj._GetViewHistory();
		}
	}
}
