import QtQuick 1.1
import com.nokia.meego 1.1
import "component"
import "../js/main.js" as Script
import "../js/util.js" as Util

BasePage {
	id: root;

	sTitle: qsTr("Setting");
	objectName: "idSettingPage";

	Header{
		id: header;
		//sText: root.sTitle;
		ToolBarLayout{
			anchors.fill: parent;
			ToolIcon{
				id: back;
				anchors.verticalCenter: parent.verticalCenter;
				iconId: "toolbar-back";
				onClicked: pageStack.pop();
			}
			ButtonRow{
				opacity: 0.8;
				height: parent.height - constants._iSpacingLarge;
				anchors.bottom: parent.bottom;
				TabButton{
					height: parent.height;
					text: qsTr("General");
					tab: generalflick;
				}
				TabButton{
					height: parent.height;
					text: qsTr("Player");
					tab: playerflick;
				}
			}
		}
	}

	function _Init()
	{
	}

	QtObject{
		id: obj;
		property int keywordHistoryCount: Script.KeywordHistoryCount();
		property int viewHistoryCount: Script.ViewHistoryCount();

		function _ClearKeywordHistory()
		{
			controller._Query(
				qsTr("WARNING"),
				qsTr("It will remove all keyword history! Are you sure?"),
				qsTr("Delete"), qsTr("Cancel"),
				function(){
					Script.RemoveKeywordHistory();
					keywordHistoryCount = Script.KeywordHistoryCount();
				}
			);
		}

		function _ClearViewHistory()
		{
			controller._Query(
				qsTr("WARNING"),
				qsTr("It will remove all view history! Are you sure?"),
				qsTr("Delete"), qsTr("Cancel"),
				function(){
					Script.RemoveViewHistory();
					viewHistoryCount = Script.ViewHistoryCount();
				}
			);
		}

		function _ResetSettings()
		{
			controller._Query(
				qsTr("WARNING"),
				qsTr("It will reset all settings! Are you sure?"),
				qsTr("Reset"), qsTr("Cancel"),
				function(){
					settings._ResetSetting();
					_ReadSettings();
				}
			);
		}

		function _ReadSettings()
		{
			orientation.vCurrentValue = settings.iOrientation;
			themeColor.iCurrentIndex = settings.iThemeColor;
			nightMode.checked = settings.bNightMode;
			defaultBrowser.vCurrentValue = settings.iDefaultBrowser;
			touchIconDrag.checked = settings.bTouchIconDrag;
			fullscreen.checked = settings.bFullscreen;

			/*
			browserHelper = settings.bBrowserHelper;
			browserDblZoom = settings.bBrowserDblZoom;
			browserLoadImage = settings.bBrowserLoadImage;
			*/

			videoQuality.vCurrentValue = settings.iVideoQuality;
			danmakuScreenMode.vCurrentValue = settings.eDanmakuScreenMode;
			danmakuOpacity.value = settings.fDanmakuOpacity;
			danmakuSize.value = settings.fDanmakuSize;
			danmakuSpeed.value = settings.fDanmakuSpeed;
			danmakuLimit.sText = settings.iDanmakuLimit;
			openDanmaku.checked = settings.bOpenDanmaku;
		}
	}

	TabGroup{
		anchors.top: header.bottom;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.bottom: resetbtn.top;
		anchors.bottomMargin: constants._iSpacingMedium;
		currentTab: generalflick;

		Flickable{
			id: generalflick;
			anchors.fill: parent;
			contentWidth: width;
			contentHeight: generallayout.height;
			clip: true;
			Column{
				id: generallayout;
				anchors.verticalCenter: parent.verticalCenter;
				width: parent.width;
				spacing: constants._iSpacingXXL;

				SelectWidget{
					id: orientation;
					iMargins: constants._iSpacingLarge;
					sText: qsTr("Orientation");
					aOptions: [
						{
							name: qsTr("Automatic"),
							value: 0,
						},
						{
							name: qsTr("Portrait"),
							value: 1,
						},
						{
							name: qsTr("Landscape"),
							value: 2,
						},
					]
					vCurrentValue: settings.iOrientation;
					onSelected: {
						settings.iOrientation = value;
					}
				}

				SelectWidget{
					id: themeColor;
					iMargins: constants._iSpacingLarge;
					sText: qsTr("Theme");
					aOptions: constants._aThemeColors;
					iCurrentIndex: settings.iThemeColor;
					onSelected: {
						settings.iThemeColor = index;
					}
				}

				SwitcherWidget{
					id: nightMode;
					iMargins: constants._iSpacingLarge;
					sText: qsTr("Night mode");
					checked: settings.bNightMode;
					onCheckedChanged: {
						settings.bNightMode = checked;
					}
				}

				SwitcherWidget{
					id: fullscreen;
					iMargins: constants._iSpacingLarge;
					sText: qsTr("Fullscreen");
					checked: settings.bFullscreen;
					onCheckedChanged: {
						settings.bFullscreen = checked;
					}
				}

				SelectWidget{
					id: defaultBrowser;
					iMargins: constants._iSpacingLarge;
					sText: qsTr("Default browser");
					aOptions: [
						{
							name: qsTr("Internal"),
							value: 0,
						},
						{
							name: qsTr("System"),
							value: 1,
						},
					]
					vCurrentValue: settings.iDefaultBrowser;
					onSelected: {
						settings.iDefaultBrowser = value;
					}
				}

				/*
				SwitcherWidget{
					id: browserHelper;
					iMargins: constants._iSpacingLarge;
					sText: qsTr("Browser helper");
					checked: settings.bBrowserHelper;
					onCheckedChanged: {
						settings.bBrowserHelper = checked;
					}
				}

				SwitcherWidget{
					id: browserLoadImage;
					iMargins: constants._iSpacingLarge;
					sText: qsTr("Browser load image");
					checked: settings.bBrowserLoadImage;
					onCheckedChanged: {
						settings.bBrowserLoadImage = checked;
					}
				}

				SwitcherWidget{
					id: browserDblZoom;
					iMargins: constants._iSpacingLarge;
					sText: qsTr("Browser double-click zoom");
					checked: settings.bBrowserDblZoom;
					onCheckedChanged: {
						settings.bBrowserDblZoom = checked;
					}
				}
				*/

				SwitcherWidget{
					id: touchIconDrag;
					iMargins: constants._iSpacingLarge;
					sText: qsTr("Touch icon drag");
					checked: settings.bTouchIconDrag;
					onCheckedChanged: {
						settings.bTouchIconDrag = checked;
					}
				}

				ButtonWidget{
					iMargins: constants._iSpacingLarge;
					sText: qsTr("Keyword history") + ": " + obj.keywordHistoryCount;
					sButtonText: qsTr("Clear all");
					bEnabled: obj.keywordHistoryCount > 0;
					onClicked: {
						obj._ClearKeywordHistory();
					}
				}

				ButtonWidget{
					iMargins: constants._iSpacingLarge;
					sText: qsTr("View history") + ": " + obj.viewHistoryCount;
					sButtonText: qsTr("Clear all");
					bEnabled: obj.viewHistoryCount > 0;
					onClicked: {
						obj._ClearViewHistory();
					}
				}
			}
		}
		ScrollDecorator{
			flickableItem: generalflick;
		}

		Flickable{
			id: playerflick;
			anchors.fill: parent;
			contentWidth: width;
			contentHeight: playerlayout.height;
			clip: true;
			Column{
				id: playerlayout;
				anchors.verticalCenter: parent.verticalCenter;
				width: parent.width;
				spacing: constants._iSpacingXXL;

				SelectWidget{
					id: videoQuality;
					iMargins: constants._iSpacingLarge;
					sText: qsTr("Video quality");
					aOptions: constants._aVideoQualitys;
					vCurrentValue: settings.iVideoQuality;
					onSelected: {
						settings.iVideoQuality = value;
					}
				}

				SelectWidget{
					id: danmakuScreenMode;
					iMargins: constants._iSpacingLarge;
					sText: qsTr("Danmaku screen mode");
					aOptions: [
						{
							name: qsTr("Full"),
							value: 0,
						},
						{
							name: qsTr("1/4 screen"),
							value: 1,
						},
						{
							name: qsTr("1/2 screen"),
							value: 2,
						},
						{
							name: qsTr("3/4 screen"),
							value: 3,
						},
					]
					vCurrentValue: settings.eDanmakuScreenMode;
					onSelected: {
						settings.eDanmakuScreenMode = value;
					}
				}

				Text{
					width: parent.width - constants._iSpacingLarge * 2;
					anchors.horizontalCenter: parent.horizontalCenter;
					color: "#ff0000";
					horizontalAlignment: Text.AlignHCenter;
					verticalAlignment: Text.AlignVCenter;
					font.pixelSize: 25 * settings.fDanmakuSize;
					opacity: settings.fDanmakuOpacity;
					text: qsTr("This is a danmaku.") + " ABC abc 123 !!!";
					wrapMode: Text.WordWrap;
				}
				SliderWidget{
					id: danmakuOpacity;
					iMargins: constants._iSpacingLarge;
					sText: qsTr("Danmaku opacity");
					iPrecision: 1;
					minimumValue: 0.2;
					maximumValue: 1.0;
					stepSize: 0.1;
					bAutoLabel: false;
					sCurText: parseInt(value * 100) + "%";
					sMinText: parseInt(minimumValue * 100) + "%";
					sMaxText: parseInt(maximumValue * 100) + "%";
					value: settings.fDanmakuOpacity;
					onValueChanged /*onMove*/: {
						settings.fDanmakuOpacity = value;
					}
				}
				SliderWidget{
					id: danmakuSize;
					iMargins: constants._iSpacingLarge;
					sText: qsTr("Danmaku font size");
					iPrecision: 1;
					minimumValue: 0.5;
					maximumValue: 2.0;
					stepSize: 0.1;
					bAutoLabel: false;
					sCurText: value.toFixed(1) + "x";
					sMinText: minimumValue.toFixed(1) + "x";
					sMaxText: maximumValue.toFixed(1) + "x";
					value: settings.fDanmakuSize;
					onValueChanged /*onMove*/: {
						settings.fDanmakuSize = value;
					}
				}
				SliderWidget{
					id: danmakuSpeed;
					iMargins: constants._iSpacingLarge;
					sText: qsTr("Danmaku speed");
					iPrecision: 1;
					minimumValue: 0.1;
					maximumValue: 2.0;
					stepSize: 0.1;
					bAutoLabel: false;
					sCurText: value.toFixed(1) + "x";
					sMinText: minimumValue.toFixed(1) + "x";
					sMaxText: maximumValue.toFixed(1) + "x";
					value: settings.fDanmakuSpeed;
					onValueChanged /*onMove*/: {
						settings.fDanmakuSpeed = value;
					}
				}

				InputWidget{
					id: danmakuLimit;
					sText: qsTr("Danmaku limit");
					sInputText: settings.iDanmakuLimit;
					iLabelWidth: constants._iSizeTooBig;
					eInputMethodHints:Qt.ImhDigitsOnly;
					sActionKeyLabel: qsTr("OK");
					sPlaceholder: qsTr("0 to not limit");
					onSure: {
						settings.iDanmakuLimit = parseInt(text);
					}
				}

				SwitcherWidget{
					id: openDanmaku;
					iMargins: constants._iSpacingMedium;
					sText: qsTr("Open danmaku");
					checked: settings.bOpenDanmaku;
					onCheckedChanged: {
						settings.bOpenDanmaku = checked;
					}
				}
			}
		}
		ScrollDecorator{
			flickableItem: playerflick;
		}
	}

	Button{
		id: resetbtn;
		anchors.bottom: parent.bottom;
		anchors.horizontalCenter: parent.horizontalCenter;
		text: qsTr("Reset settings");
		onClicked: {
			obj._ResetSettings();
		}
	}
}
