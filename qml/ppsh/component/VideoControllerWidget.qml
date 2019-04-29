import QtQuick 1.1
import com.nokia.meego 1.1
import karin.ppsh 1.0
import "../../js/util.js" as Util

Item{
	id:root;

	property Item video;
	property int iBarHeight: constants._iSizeXL;
	property real fBarOpacity: 0.8;
	property int iAnimInterval: 400;
	property int iStateDuration: 8000;

	property alias sTitle: titletext.text;
	property bool bCanPrev: false;
	property bool bCanNext: false;
	property int iBaseTime: 0;
	property int iTotalTime: 0;
	property bool bShowDanmaku: settings.bOpenDanmaku;

	property int __totalTime: iTotalTime === 0 ? video.duration : iTotalTime;
	property int iTotalPlayedTime: iBaseTime + video.position;

	signal playExternally(string source);
	signal seek(real value);
	signal seekForTotal(real value);
	signal exit;
	signal stop;
	signal playPart(string where);
	signal showPartsWidget;
	signal showContentsWidget;

	objectName: "idVideoControllerWidget";

	function _ToggleState(on)
	{
		if(on === undefined)
		{
			if(toolbar.state === constants._sShowState) toolbar.state = constants._sHideState;
			else if(toolbar.state === constants._sHideState) toolbar.state = constants._sShowState;
		}
		else
		{
			if(on) toolbar.state = constants._sShowState;
			else toolbar.state = constants._sHideState;
		}
	}

	function _Reset()
	{
		fillmodellist.vCurrentValue = PPSHVideo.PreserveAspectFit;
		timer.stop();
		_ToggleState();
	}

	function _Load()
	{
		_Reset();
	}

	RectWidget{
		id: headbar;
		property int theight: root.iBarHeight;
		anchors.top: parent.top;
		anchors.left: parent.left;
		anchors.right: parent.right;
		color: "#000000";
		z: 2;
		opacity: root.fBarOpacity;
		visible: height !== 0;
		clip: true;
		iStart: 0;
		iTarget: theight;
		sProperty: "height";
		iDuration: root.iAnimInterval;
		state: toolbar.state;

		Text{
			id:titletext;
			anchors.left: parent.left;
			anchors.right: contents.left;
			anchors.top: parent.top;
			anchors.bottom: parent.bottom;
			verticalAlignment: Text.AlignVCenter;
			color: "#ffffff";
			font.pixelSize: constants._iFontXL;
			visible: parent.height === parent.theight;
			elide: Text.ElideRight;
			clip: true;
		}

		ToolIcon{
			id: contents;
			width: height;
			iconId: "toolbar-pages-all";
			anchors.right: parts.left;
			anchors.verticalCenter: parent.verticalCenter;
			visible: parent.height === parent.theight && enabled;
			onClicked: {
				timer.restart();
				root.showContentsWidget();
			}
		}
		ToolIcon{
			id: parts;
			width: height;
			iconId: "toolbar-list";
			anchors.right: setting.left;
			anchors.verticalCenter: parent.verticalCenter;
			visible: parent.height === parent.theight && enabled;
			onClicked: {
				timer.restart();
				root.showPartsWidget();
			}
		}
		ToolIcon{
			id: setting;
			width: height;
			iconId: "toolbar-settings";
			anchors.right: close.left;
			anchors.verticalCenter: parent.verticalCenter;
			visible: parent.height === parent.theight;
			onClicked: {
				timer.restart();
				if(settingbar.state === constants._sHideState){
					settingbar.state = constants._sShowState;
				}else if(settingbar.state === constants._sShowState){
					settingbar.state = constants._sHideState;
				}
			}
		}

		ToolIcon{
			id: close;
			width: height;
			iconId: "toolbar-close";
			anchors.right: parent.right;
			anchors.verticalCenter: parent.verticalCenter;
			visible: parent.height === parent.theight;
			onClicked: {
				root.exit();
			}
		}
	}

	RectWidget{
		id: settingbar;
		property int twidth: constants._iSettingBarWidth;
		anchors{
			top: parent.top;
			left: parent.left;
			topMargin: root.iBarHeight;
			bottom: parent.bottom;
			bottomMargin: root.iBarHeight;
		}
		color: constants._cLighterColor;
		z: 1;
		opacity: root.fBarOpacity;
		visible: width !== 0;
		state: constants._sHideState;
		iStart: 0;
		iTarget: twidth;
		sProperty: "width";
		iDuration: root.iAnimInterval;
		Item{
			id: settingrect;
			anchors.fill: parent;
			visible: parent.width === parent.twidth;
			Flickable{
				id: settingsflick;
				anchors.fill: parent;
				contentWidth: width;
				clip: true;
				contentHeight: mainlayout.height;
				onContentYChanged: {
					if(moving /* flicking*/)
					timer.restart();
				}
				Column{
					id: mainlayout;
					width: parent.width;
					spacing: constants._iSpacingSmall;
					SwitcherWidget{
						iMargins: constants._iSpacingMedium;
						sText: qsTr("Show danmaku");
						checked: root.bShowDanmaku;
						enabled: settings.bOpenDanmaku;
						onCheckedChanged: {
							timer.restart();
							root.bShowDanmaku = checked;
						}
					}

					SwitcherWidget{
						iMargins: constants._iSpacingMedium;
						sText: qsTr("Mute");
						checked: video.muted;
						onCheckedChanged: {
							timer.restart();
							video.muted = checked;
						}
					}

					SelectWidget{
						id: fillmodellist;
						iMargins: constants._iSpacingMedium;
						sText: qsTr("Fill mode");
						aOptions: [
							{
								name: qsTr("Fit"),
								value: PPSHVideo.PreserveAspectFit
							},
							{
								name: qsTr("Crop"),
								value: PPSHVideo.PreserveAspectCrop
							},
							{
								name: qsTr("Stretch"),
								value: PPSHVideo.Stretch
							},
						]
						vCurrentValue: PPSHVideo.PreserveAspectFit;
						onSelected: {
							timer.restart();
							video._SetFillMode(value);
						}
					}
					ProgressWidget{
						sText: qsTr("Total progress");
						sCurText: settingrect.visible ? Util.FormatDuration(root.iTotalPlayedTime / 1000) : "00:00";
						sTotalText: settingrect.visible ? Util.FormatDuration(root.__totalTime /1000) : "59:59";
						value: root.iTotalPlayedTime;
						minimumValue: 0;
						maximumValue: root.__totalTime;
						onClicked: {
							timer.restart();
							root.seekForTotal(value);
						}
					}
					Button{
						anchors.horizontalCenter: parent.horizontalCenter;
						text: qsTr("Copy url");
						enabled: root.source != "";
						width: Math.min(constants._iSizeTooBig, parent.width);
						onClicked: {
							timer.restart();
							if(video.source != "") controller._CopyToClipboard(video.source.toString());
							else controller._ShowMessage(qsTr("Video source is empty"));
						}
					}
					Button{
						anchors.horizontalCenter: parent.horizontalCenter;
						text: qsTr("Open externally");
						enabled: root.source != "";
						width: Math.min(constants._iSizeTooBig, parent.width);
						onClicked: {
							timer.restart();
							root.playExternally(video.source);
						}
					}
				}
			}
			ScrollDecorator{
				flickableItem: settingsflick;
			}
		}
	}

	RectWidget{
		id: toolbar;
		property int theight: root.iBarHeight;
		anchors.left: parent.left;
		anchors.right: parent.right;
		anchors.bottom: parent.bottom;
		color: "#000000";
		z: 2;
		opacity: root.fBarOpacity;
		clip: true;
		visible: height !== 0;
		iStart: 0;
		iTarget: theight;
		sProperty: "height";
		iDuration: root.iAnimInterval;
		state: constants._sHideState;
		onStateChanged: {
			if(state === constants._sHideState){
				settingbar.state = constants._sHideState;
			}
		}

		ToolIcon{
			id: play;
			width: height;
			iconId: video.paused ? "toolbar-mediacontrol-play" : "toolbar-mediacontrol-pause";
			anchors.left: parent.left;
			anchors.verticalCenter: parent.verticalCenter;
			enabled: video.playing;
			visible: parent.height === parent.theight;
			onClicked: {
				timer.restart();
				video.paused = !video.paused;
			}
		}

		ProgressBar {
			id: progressBar;
			anchors.left: play.right;
			anchors.right: prevpart.left;
			anchors.verticalCenter: parent.verticalCenter;
			visible: parent.height === parent.theight;
			minimumValue: 0;
			maximumValue: video.duration || 0;
			value: video.position || 0;
			MouseArea{
				function __Seek(mouse_x)
				{
					timer.restart();
					root.seek(mouse_x / width);
				}
				anchors.centerIn: parent;
				enabled: video.duration !== 0;
				width: parent.width;
				height: 5 * parent.height;
				onClicked: {
					__Seek(mouse.x);
				}
				onPositionChanged:{
					if(pressed)
					{
						__Seek(mouse.x);
					}
				}
			}
		}

		Row{
			anchors.left: progressBar.left;
			anchors.right: progressBar.right;
			anchors.top: parent.top;
			anchors.bottom: progressBar.top;
			clip: true;
			visible: parent.height === parent.theight;
			Text{
				width: parent.width / 2;
				height: parent.height;
				color: "#ffffff";
				horizontalAlignment: Text.AlignLeft;
				verticalAlignment: Text.AlignVCenter;
				font.pixelSize: constants._iFontMedium;
				text: parent.visible ? Util.FormatDuration(video.position / 1000) : "";
			}

			Text{
				id: durationtext;
				width: parent.width / 2;
				height: parent.height;
				color: "#ffffff";
				horizontalAlignment: Text.AlignRight;
				verticalAlignment: Text.AlignVCenter;
				font.pixelSize: constants._iFontMedium;
				text: parent.visible ? Util.FormatDuration(video.duration / 1000) : "";
			}
		}
		ToolIcon{
			id: prevpart;
			width: height;
			visible: parent.height === parent.theight && root.bCanPrev;
			iconId: "toolbar-mediacontrol-previous";
			anchors.right: nextpart.left;
			anchors.verticalCenter: parent.verticalCenter;
			onClicked: {
				timer.restart();
				root.playPart(constants._sPrevPage);
			}
		}
		ToolIcon{
			id: nextpart;
			width: height;
			visible: parent.height === parent.theight && root.bCanNext;
			iconId: "toolbar-mediacontrol-next";
			anchors.right: stop.left;
			anchors.verticalCenter: parent.verticalCenter;
			onClicked: {
				timer.restart();
				root.playPart(constants._sNextPage);
			}
		}

		ToolIcon{
			id: stop;
			width: height;
			iconId: "toolbar-mediacontrol-stop";
			anchors.right: parent.right;
			anchors.verticalCenter: parent.verticalCenter;
			visible: parent.height === parent.theight;
			enabled: video.playing;
			onClicked: {
				timer.restart();
				root.stop();
			}
		}
	}

	Timer{
		id: timer;
		interval: root.iStateDuration;
		repeat: false;
		running: toolbar.state === constants._sShowState;
		onTriggered: {
			toolbar.state = constants._sHideState;
		}
	}
}
