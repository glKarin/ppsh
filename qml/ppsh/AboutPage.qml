import QtQuick 1.1
import com.nokia.meego 1.1
import "component"
import "../js/main.js" as Script
import "../js/util.js" as Util

BasePage {
	id: root;

	sTitle: qsTr("About");
	objectName: "idAboutPage";

	Header{
		id: header;
		sText: root.sTitle;
		ToolBarLayout{
			anchors.fill: parent;
			ToolIcon{
				id: back;
				anchors.verticalCenter: parent.verticalCenter;
				iconId: "toolbar-back";
				onClicked: pageStack.pop();
			}
		}
	}

	function _Init()
	{
	}

	QtObject{
		id: obj;
		
		function _GetChangeLog()
		{
			var r = [];
			var c = _UT.Changelog();

			for(var i in c.CHANGES)
			{
				r.push({
					text: c.CHANGES[i],
				});
			}

			return r;
		}
	}

	Flickable{
		id: flick;
		anchors.top: header.bottom;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.bottom: parent.bottom;
		contentWidth: width;
		contentHeight: mainlayout.height;
		clip: true;
		Column{
			id: mainlayout;
			anchors.verticalCenter: parent.verticalCenter;
			width: parent.width;
			spacing: constants._iSpacingXXL;
			TextListWidget{
				width: parent.width;
				sTitle: qsTr("Update");
				iPixelSize: constants._iFontXL;
				vNu: "  * ";
				aTexts: obj._GetChangeLog();
			}
			TextListWidget{
				width: parent.width;
				sTitle: qsTr("System");
				iPixelSize: constants._iFontXL;
				eHorizontalAlignment: Text.AlignHCenter;
				aTexts: [
					{
						text: qsTr("Version") + ": " + _UT.Get("VER"),
					},
					{
						text: qsTr("Release") + ": " + _UT.Get("RELEASE"),
					},
					{
						text: qsTr("Code") + ": " + _UT.Get("CODE"),
					},
					{
						text: qsTr("Platform") + ": " + _UT.Get("PLATFORM"),
					},
					{
						text: qsTr("Qt") + ": " + _UT.Get("QT"),
					},
					{
						text: qsTr("Dev") + ": " + _UT.Get("DEV"),
					},
				]
			}
			TextListWidget{
				width: parent.width;
				sTitle: qsTr("About");
				iPixelSize: constants._iFontXL;
				onLinkClicked: {
					eval(link);
				}
				aTexts: [
					{
						text: _UT.Get("APP"),
						horizontalAlignment: Text.AlignHCenter,
						pixelSize: constants._iFontBig,
					},
					{
						text: _UT.Get("DESC"),
					},
					{
						text: qsTr("Contact") + ": ",
					},
					{
						text: "<a href='controller._OpenUserPage(\"%1\")'>%2</a> <a href='controller._OpenUrl(\"mailto:%3\", 1);'>%3</a> <a href='controller._OpenUrl(\"%4\");'>%5</a>".arg(_UT.Get("BUID")).arg(_UT.Get("DEV")).arg(_UT.Get("EMAIL")).arg(_UT.Get("TMO")).arg("talk.maemo.org"),
					},
					{
						text: qsTr("Download") + ": ",
					},
					{
						text: "<a href='controller._OpenUrl(\"%1\", 1);'>%2</a> <a href='controller._OpenUrl(\"%3\", 1);'>%4(%5)</a>".arg(_UT.Get("OPENREPOS")).arg("OpenRepos").arg(_UT.Get("PAN").split(" ")[0]).arg("Baidu Pan").arg(_UT.Get("PAN").split(" ")[1]),
					},
					{
						text: qsTr("Source") + ": " + "<a href='controller._OpenUrl(\"%1\");'>%1</a>".arg(_UT.Get("GITHUB")),
					},
				]
			}
		}
	}

	ScrollDecorator{
		flickableItem: flick;
	}
}
