import QtQuick 1.1
import com.nokia.meego 1.1
import com.nokia.extras 1.1
import "component"
import "../js/main.js" as Script
import "../js/main.js" as Util

PageStackWindow {
	id: app;

	property int iStatusBarHeight: __statusBarHeight; // private property
	property int __noneDevMenuCount: 4;

	objectName: "idMainWindow";
	showStatusBar: inPortrait && (pageStack.currentPage && !pageStack.currentPage.bFull);
	showToolBar: false;
	platformStyle: PageStackWindowStyle {
		cornersVisible: app.inPortrait && !settings.bFullscreen;
	}
	initialPage: 
	MainPage
	//PlayerPage
	{
		id: mainpage
	}

	Binding{
		target: theme;
		property: "inverted";
		value: constants._bInverted;
	}

	Constants{
		id: constants;
	}

	SettingsObject{
		id: settings;
	}

	Controller{
		id: controller;
	}

	MenuWidget{
		id: menu;
		anchors.fill: parent;
		anchors.topMargin: iStatusBarHeight;
		sTitle: _UT.Get("NAME") + "\n" + qsTr("Bilibili");
		sIcon: _UT.Get("ICON_PATH");
		tools: [
			ToolIcon{
				iconId: "toolbar-close";
				onClicked: Qt.quit();
			},
			ToolIcon{
				iconId: "toolbar-back";
				visible: pageStack.depth > 1;
				onClicked: {
					menu._Toggle(false);
					if(pageStack.depth > 1)
					{
						var p = pageStack.currentPage;
						if(typeof(p._DeInit() === "function")) p._DeInit();
						pageStack.pop();
					}
				}
			}
		]
		onClicked: {
			if(mouse.x < constants._iSizeXXL)
			{
				_UT.dev ^= 1;
				app.__Dev();
				controller._ShowMessage("Dev " + (_UT.dev !== 0 ? "open" : "close"));
			}
			else controller._ShowMessage(_UT.Get("EGG"));
		}
	}
	Rectangle{
		anchors.horizontalCenter: settings.bTouchIconDrag ? undefined : parent.left;
		anchors.verticalCenter: settings.bTouchIconDrag ? undefined : parent.bottom;
		x: 0 - width / 2;
		y: app.width - height / 2; // do not drag
		width: constants._iSizeXL;
		height: width;
		z: 998;
		color: constants._cThemeColor;
		opacity: 0.6;
		radius: width / 2;
		visible: !menu.visible && (pageStack.currentPage && !pageStack.currentPage.bFull);
		smooth: true;
		clip: true;
		Rectangle{
			anchors.centerIn: parent;
			width: parent.width * 0.6;
			height: width;
			color: constants._cThemeColor;
			//opacity: 0.8;
			smooth: true;
			radius: width / 2;
		}
		MouseArea{
			anchors.fill: parent;
			drag.target: parent;
			drag.axis: Drag.XandYAxis;
			drag.minimumY: app.iStatusBarHeight - parent.height / 2;
			drag.maximumY: app.width - parent.height / 2;
			drag.minimumX: 0 - parent.width / 2;
			drag.maximumX: app.height - parent.width / 2;
			onClicked: {
				menu._Toggle(true);
			}
		}
	}

	InfoBanner{
		id: infobanner;
		topMargin: (app.showStatusBar ? iStatusBarHeight : 0) + constants._iSpacingLarge;
		leftMargin: constants._iSpacingMedium;
		z: constants._iMaxZ;
		function _ShowMessage(text)
		{
			infobanner.text = text;
			infobanner.show();
		}
	}

	Rectangle{
		id: statusbar;
		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.right: parent.right;
		height: iStatusBarHeight;
		z: Number.MAX_VALUE;
		color: constants._cThemeColor;
		opacity: 0.4;
		visible: app.showStatusBar && (_UT.dev !== 0 || settings.bFullscreen);
	}
	function __Dev()
	{
		if(_UT.dev !== 0)
		{
			Util.ModelPush(menu.model, {
				label: "TEST",
				name: "Test",
				icon: "jump-to",
				func: "controller.__Test();",
			});
			Util.ModelPush(menu.model, {
				label: "Test request",
				name: "TestRequest",
				icon: "share",
				func: "controller._OpenTestRequestPage();",
			});
		}
		else
		{
			while(Util.ModelSize(menu.model) > app.__noneDevMenuCount) Util.ModelRemove(menu.model, app.__noneDevMenuCount);
		}
	}

	Component.onCompleted: {
		Util.ModelPush(menu.model, {
			label: qsTr("Browser"),
			name: "Browser",
			icon: "search",
			func: "controller._OpenUrl(undefined, 0);",
		});
		Util.ModelPush(menu.model, {
			label: qsTr("View history"),
			name: "History",
			icon: "directory",
			func: "controller._OpenHistoryPage();",
		});
		Util.ModelPush(menu.model, {
			label: qsTr("Setting"),
			name: "Setting",
			icon: "settings",
			func: "controller._OpenSettingPage();",
		});
		Util.ModelPush(menu.model, {
			label: qsTr("About"),
			name: "About",
			icon: "application",
			func: "controller._OpenAboutPage();",
		});

		__Dev();

		Script.Init(_UT);
	}
}
