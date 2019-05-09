import QtQuick 1.1
import com.nokia.meego 1.1
import "../component"
import "../../js/main.js" as Script

BasePage {
	id: root;

	sTitle: "Test Request";
	objectName: "idTestRequestPage";

	Header{
		id: header;
		//sText: root.sTitle;
		Row{
			anchors.fill: parent;
			clip: true;
			spacing: constants._iSpacingSmall;
			ToolIcon{
				id: menuicon;
				anchors.verticalCenter: parent.verticalCenter;
				iconId: "toolbar-view-menu";
				onClicked: mainmenu.open();
			}
			SearchWidget{
				id: input;
				anchors.verticalCenter: parent.verticalCenter;
				width: parent.width - parent.spacing - menuicon.width;
				sPlaceholder: "Input request url";
				sActionKeyLabel: "Request";
				eInputMethodHints: Qt.ImhNoPredictiveText | Qt.ImhNoAutoUppercase;
				onSearch: {
					obj._Request();
				}
			}
		}
	}

	function _Init(url)
	{
		obj._Request(url);
	}


	QtObject{
		id: obj;
		function _Request(url)
		{
			if(url) input.sText = url;
			var u = input.sText;
			if(u == "") return;
			var u = _UT.FormatUrl(u);
			if(!u) return;

			root.bBusy = true;
			var params = {
			};
			var method = "GET";
			var s = function(json, header){
				root.bBusy = false;
				resp.text = typeof(json) === "object" ? JSON.stringify(json) : json.toString();
			};
			var f = function(json){
				root.bBusy = false;
				controller._ShowMessage("Error");
				resp.text = json;
			};

			var headers = {
			};

			//_UT.SetRequestHeaders(headers);
			Script.Request(u, method, params, s, f, "TEXT");
		}
	}

	Flickable{
		id: flick;
		anchors.right: parent.right;
		anchors.top: header.bottom;
		anchors.bottom: parent.bottom;
		width: parent.width;
		contentWidth: width;
		contentHeight: resp.height;
		clip: true;

		Text{
			id: resp;
			width: parent.width;
			textFormat: Text.PlainText;
			wrapMode: Text.WrapAnywhere;
			font.pixelSize: constants._iFontLarge;
			color: constants._cDarkestColor;
		}
	}

	ScrollDecorator{
		flickableItem: flick;
	}

	ContextMenu{
		id: mainmenu;
		MenuLayout {
			MenuItem {
				text: "Open externally";
				enabled: input.sText != "";
				onClicked: {
					if(input.sText != "") controller._OpenUrl(input.sText, 1);
				}
			}
			MenuItem{
				text: "Copy url";
				enabled: input.sText != "";
				onClicked: {
					if(input.sText != "") controller._CopyToClipboard(input.sText);
				}
			}
			MenuItem{
				text: "Copy response";
				enabled: resp.text !== "";
				onClicked: {
					controller._CopyToClipboard(resp.text);
				}
			}
			MenuItem{
				text: "Clear response";
				enabled: resp.text !== "";
				onClicked: {
					resp.text = "";
				}
			}
			MenuItem {
				text: "Back";
				onClicked: {
					pageStack.pop();
				}
			}
		}
	}
}
