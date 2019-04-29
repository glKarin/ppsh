import QtQuick 1.1
import com.nokia.meego 1.1
import "component"
import "../js/main.js" as Script
import "../js/util.js" as Util

BasePage {
	id: root;

	sTitle: qsTr("Search");
	objectName: "idSearchPage";

	Header{
		id: header;
		//sText: root.sTitle;
		/*
		onClicked: {
			obj._GetHotKeyword();
		}
		*/
		Row{
			anchors.fill: parent;
			clip: true;
			spacing: constants._iSpacingSmall;
			ToolIcon{
				id: back;
				anchors.verticalCenter: parent.verticalCenter;
				iconId: "toolbar-back";
				onClicked: pageStack.pop();
			}
			SearchWidget{
				id: input;
				anchors.verticalCenter: parent.verticalCenter;
				width: parent.width - parent.spacing - back.width;
				sPlaceholder: qsTr("Search keyword or avId");
				sActionKeyLabel: qsTr("Search");
				onSearch: {
					obj._Search();
				}
			}
		}
	}

	function _Init()
	{
		obj._GetHotKeyword();
		obj._GetKeywordHistory();
	}

	QtObject{
		id: obj;

		function _GetKeywordHistory()
		{
			Util.ModelClear(view.model);
			Script.GetKeywordHistory(view.model);
		}

		function _RemoveHistory(kw)
		{
			Script.RemoveKeywordHistory(kw);
			_GetKeywordHistory();
		}

		function _GetHotKeyword()
		{
			root.bBusy = true;
			Util.ModelClear(hotrepeater.model);

			var d = {
				model: hotrepeater.model,
//				limit: 10,
			};

			var s = function(){
				root.bBusy = false;
			};
			var f = function(err){
				root.bBusy = false;
				controller._ShowMessage(err);
			};

			Script.GetHotKeyword(d, s, f);
		}

		function _Search(kw)
		{
			if(kw) input.sText = kw;

			Script.AddKeywordHistory(input.sText);
			Util.ModelClear(view.model);
			Script.GetKeywordHistory(view.model);
			controller._OpenResultPage(input.sText, true);
		}
	}

	SectionWidget{
		id: hotlabel;
		anchors.top: header.bottom;
		anchors.left: parent.left;
		width: parent.width;
		sText: qsTr("Hot keyword");
		onClicked: {
			obj._GetHotKeyword();
		}
	}

	Flow{
		id: hot;
		anchors.top: hotlabel.bottom;
		anchors.left: parent.left;
		width: parent.width;
		Repeater{
			id: hotrepeater;
			model: ListModel{}
			delegate: Component{
				Item{
					width: hotkeyword.width + constants._iSpacingMedium * 2;
					height: constants._iSizeLarge;
					clip: true;
					Text{
						id: hotkeyword;
						anchors.horizontalCenter: parent.horizontalCenter;
						height: parent.height;
						verticalAlignment: Text.AlignVCenter;
						font.pixelSize:constants._iFontXL;
						text: model.keyword;
						color: constants._cDarkerColor;
					}
					MouseArea{
						anchors.fill: parent;
						onClicked: {
							obj._Search(model.keyword);
						}
					}
				}
			}
		}
	}

	SectionWidget{
		id: historylabel;
		anchors.top: hot.bottom;
		anchors.left: parent.left;
		width: parent.width;
		sText: qsTr("History") + "[" + view.count + "]";
		onClicked: {
			obj._GetKeywordHistory();
		}
	}

	GridView{
		id: view;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.top: historylabel.bottom;
		anchors.bottom: parent.bottom;
		clip: true;
		model: ListModel{}
		cellWidth: width / 2;
		cellHeight: constants._iSizeLarge;
		delegate: Component{
			Item{
				width: GridView.view.cellWidth;
				height: GridView.view.cellHeight;
				clip: true;
				Text{
					anchors.fill: parent;
					//horizontalAlignment: Text.AlignHCenter;
					verticalAlignment: Text.AlignVCenter;
					font.pixelSize:constants._iFontXL;
					text: model.keyword;
					elide: Text.ElideRight;
					color: constants._cDarkerColor;
				}
				MouseArea{
					anchors.fill: parent;
					onClicked: {
						obj._Search(model.keyword);
					}
					onPressAndHold: {
						obj._RemoveHistory(model.keyword);
					}
				}
			}
		}
	}

	ScrollDecorator{
		flickableItem: view;
	}

	onStatusChanged: {
		if(status === PageStatus.Active)
		{
			obj._GetKeywordHistory();
		}
	}
}

