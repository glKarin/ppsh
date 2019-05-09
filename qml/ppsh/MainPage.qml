import QtQuick 1.1
import com.nokia.meego 1.1
import "component"
import "../js/main.js" as Script
import "../js/util.js" as Util

BasePage {
	id: root;

	sTitle: _UT.Get("NAME");
	objectName: "idMainPage";

	Header{
		id: header;
		//sText: root.sTitle;
		/*
		onClicked: {
			obj._GetRanking();
		}
		*/
	 height: constants._iSizeXXL;
		Row{
			anchors.fill: parent;
			clip: true;
			spacing: constants._iSpacingSmall;
			ToolIcon{
				id: menuicon;
				anchors.verticalCenter: parent.verticalCenter;
				iconId: "toolbar-view-menu";
				onClicked: {
					menu._Toggle(true);
				}
			}
			Item{
				width: parent.width - btnrow.width - menuicon.width;
				height: parent.height;
				clip: true;
				TextField{
					anchors.verticalCenter: parent.verticalCenter;
					anchors.left: parent.left;
					anchors.right: parent.right;
					readOnly: true;
					placeholderText: qsTr("Search");
				}
				MouseArea{
					anchors.fill: parent;
					onClicked: {
						controller._OpenSearchPage(true);
					}
				}
			}
			Row{
				id: btnrow;
				height: parent.height;
				clip: true;
				ToolIcon{
					anchors.verticalCenter: parent.verticalCenter;
					width: height;
					iconId: "toolbar-gallery";
					onClicked: {
						controller._OpenBangumiPage();
					}
				}
				ToolIcon{
					anchors.verticalCenter: parent.verticalCenter;
					width: height;
					iconId: "toolbar-favorite-mark";
					onClicked: {
						controller._OpenRankingPage();
					}
				}
				ToolIcon{
					anchors.verticalCenter: parent.verticalCenter;
					width: height;
					iconId: "toolbar-grid";
					onClicked: {
						controller._OpenCategoryPage();
					}
				}
			}
		}
	}

	function _Init()
	{
		obj._GetRanking();
	}

	QtObject{
		id: obj;

		function _GetRanking()
		{
			root.bBusy = true;
			Util.ModelClear(view.model);

			var d = {
				model: view.model,
				limit: 20,
			};

			var s = function(){
				root.bBusy = false;
			};
			var f = function(err){
				root.bBusy = false;
				controller._ShowMessage(err);
			};

			Script.GetRanking(d, s, f);
		}
	}

	VideoGridWidget{
		id: view;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.top: header.bottom;
		anchors.bottom: parent.bottom;
		onRefresh: {
			obj._GetRanking();
		}
	}

	Component.onCompleted: {
		_Init();
	}
}
