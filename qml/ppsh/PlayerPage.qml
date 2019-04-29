import QtQuick 1.1
import com.nokia.meego 1.1
import QtMultimediaKit 1.1
import "component"
import "../js/main.js" as Script
import "../js/util.js" as Util

BasePage {
	id: root;

	property string aid;
	orientationLock: PageOrientation.LockLandscape;
	bFull: true;
	sTitle: qsTr("Player");
	objectName: "idPlayerPage";

	PlayerLoader{
		id: loader;
		anchors.fill: parent;
		tools: [
			ToolIcon{
				anchors.verticalCenter: parent.verticalCenter;
				iconId: "toolbar-refresh";
				onClicked: {
					obj._GetContents();
				}
			},
			ToolIcon{
				anchors.verticalCenter: parent.verticalCenter;
				iconId: "toolbar-close";
				onClicked: {
					obj._Exit();
				}
			}
		]
		onExit: {
			obj._Exit();
		}
	}

	function _Init(aid)
	{
		obj.aid = aid;
		if(obj.aid) obj._GetContents();
	}

	QtObject{
		id: obj;
		property string aid;
		property variant contents: [];

		function _Exit()
		{
			loader._DeInit();
			pageStack.pop();
		}

		function _GetContents()
		{
			if(aid == "") return;

			root.bBusy = true;

			var model = [];
			var d = {
				model: model,
				aid: aid,
			};

			Util.ModelClear(contents);

			var s = function(data){
				root.bBusy = false;
				obj.contents = d.model; 
				var r = loader._Load(obj.aid, obj.contents);
				if(r < 0) controller._ShowMessage(qsTr("Load video player fail"));
			};
			var f = function(err){
				root.bBusy = false;
				controller._ShowMessage(err);
			};

			Script.GetVideoDetail(d, s, f);
		}
	}

	Component.onDestruction: {
		loader._DeInit();
	}
}