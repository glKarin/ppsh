import QtQuick 1.1
import com.nokia.meego 1.1
import "../../js/main.js" as Script
import "../../js/util.js" as Util

Rectangle {
	id: root;

	property real fControlOpacity: 0.8;
	property bool __playerEnabled: true;
	signal exit;
	signal stop;
	signal trigger;

	width: 320;
	height: 240;
	color: "#000000";
	objectName: "idVideoPlayerWidget";

	Connections{
		target: _PLAYER;
		onError: {
			if(root.__playerEnabled) controller._ShowMessage("[%1]: %2 -> %3(%4)".arg(qsTr("ERROR")).arg(pid).arg(errno).arg(errstr));
		}
	}

	Connections{
		target: Qt.application;
		onActiveChanged: {
			if(!Qt.application.active) obj._Pause();
		}
	}

	Streamtype{
		id: st;
		onStreamtypeLoaded: {
			if(suc) obj._LoadContents();
		}
		onDanmakuLoaded: {
			if(settings.bOpenDanmaku)
			{
				if(suc) d._Load(danmaku);
			}
		}
	}
	Connections{
		target: _CONNECTOR;
		onFinished: {
			if(settings.bOpenDanmaku)
			{
				st._LoadDanmaku(name, error, value);
			}
		}
	}


	VideoWidget{
		id: v;
		anchors.fill: parent;
		control: c;
		onErrorTriggered: {
			controller._ShowMessage("[%1]: %2 - %3".arg(qsTr("ERROR")).arg(errno).arg(errstr));
		}
		onEndOfMedia: {
			obj._EndOfMedia();
		}
		onPositionChanged: {
			if(settings.bOpenDanmaku)
			{
				if(playing) d._LoadDanmaku(v.position);
			}
		}
		onPlayingChanged: {
			d._Toggle(playing);
		}
	}

	DanmakuWidget{
		id: d;
		anchors.fill: parent;
		anchors.bottomMargin: settings.eDanmakuScreenMode === 0 ? 0 : parent.height * (1 - settings.eDanmakuScreenMode / 4);
		z: 1;
		enabled: settings.bOpenDanmaku;
		iBaseTime: st.playedDuration;
		//iPlayTime: v.position;
		bRunning: v.playing;
		visible: enabled && c.bShowDanmaku;
		onVisibleChanged: {
			d._Seek(v.position);
		}
	}

	VideoControllerWidget{
		id: c;
		anchors.fill: parent;
		video: v;
		z: 20;
		sTitle: st.title;
		fBarOpacity: root.fControlOpacity;
		bCanPrev: st.playPart > 0;
		bCanNext: st.playPart < st.playQueue.length - 1;
		iTotalTime: st.totalDuration;
		iBaseTime: st.playedDuration;
		onPlayExternally: {
			obj._Pause();
			var url = source == "" ? st._GetVideoUrl() : source;
			obj._PlayExternally(url);
		}
		onExit: {
			obj._Exit();
		}
		onShowContentsWidget: {
			if(contentsrect.state === constants._sShowState)
			{
				if(contentsrow.state === constants._sHideState) contentsrect._Toggle(false);
				else if(contentsrow.state === constants._sShowState) contentsrect.__Toggle(1);
			}
			else if(contentsrect.state === constants._sHideState)
			{
				contentsrect._Toggle(true);
				contentsrect.__Toggle(1);
			}
		}
		onShowPartsWidget: {
			if(contentsrect.state === constants._sShowState)
			{
				if(contentsrow.state === constants._sShowState) contentsrect._Toggle(false);
				else if(contentsrow.state === constants._sHideState) contentsrect.__Toggle(2);
			}
			else if(contentsrect.state === constants._sHideState)
			{
				contentsrect._Toggle(true);
				contentsrect.__Toggle(2);
			}
		}
		onSeek: {
			obj._Seek(value);
		}
		onSeekForTotal: {
			obj._SeekForTotal(value);
		}
		onStop: {
			obj._Stop();
		}
		onPlayPart: {
			obj._LoadPart(where);
		}

		RectWidget{
			id: contentsrect;
			property int __swapBarLength: constants._iSizeXL;
			property int __swapDuration: 180;
			function __Toggle(on)
			{
				if(on === undefined)
				{
					if(contentsrow.state === constants._sHideState) contentsrow.state = constants._sShowState;
					else if(contentsrow.state === constants._sShowState) contentsrow.state = constants._sHideState;
				}
				else
				{
					contentsrow.state = on === 2 ? constants._sShowState : constants._sHideState;
				}
			}

			anchors.right: parent.right;
			anchors.top: parent.top;
			anchors.bottom: parent.bottom;
			anchors.topMargin: c.iBarHeight;
			anchors.bottomMargin: c.iBarHeight;
			width: constants._iPartsListWidth;
			color: constants._cLighterColor;
			visible: width !== 0;
			state: constants._sHideState;
			opacity: parent.fBarOpacity;
			iStart: 0;
			iTarget: constants._iContentsListWidth;
			sProperty: "width";
			iDuration: parent.iAnimInterval;
			clip: true;

			Row{
				id: contentsrow;
				anchors.top: parent.top;
				anchors.bottom: parent.bottom;
				visible: parent.width === parent.iTarget;
				state: constants._sHideState;
				states: [
					State{
						name: constants._sShowState; // 2
						PropertyChanges{
							target: contentsrow;
							x: -(contentsrect.width - swapbar.width);
						}
					},
					State{
						name: constants._sHideState; // 1
						PropertyChanges{
							target: contentsrow;
							x: 0;
						}
					}
				]
				transitions: [
					Transition{
						from: constants._sHideState;
						to: constants._sShowState;
						NumberAnimation{
							target: contentsrow;
							property: "x";
							duration: contentsrect.__swapDuration;
							easing.type: Easing.OutExpo;
						}
					},
					Transition{
						from: constants._sShowState;
						to: constants._sHideState;
						NumberAnimation{
							target: contentsrow;
							property: "x";
							duration: contentsrect.__swapDuration;
							easing.type: Easing.InExpo;
						}
					}
				]

				StreamListWidget{
					id: contentslist;
					width: contentsrect.width - swapbar.width;
					height: parent.height;
					model: st.contents;
					onClicked: {
						contentsrect.__Toggle(2);
						obj._PlayContent(index, data);
					}
				}
				Item{
					id: swapbar;
					width: contentsrect.__swapBarLength;
					height: parent.height;
					clip: true;
					Rectangle{
						id: vsep;
						anchors.top: parent.top;
						anchors.bottom: parent.bottom;
						anchors.right: contentsrow.state === constants._sShowState ? swaptext.right : swaptext.left;
						width: 1;
						color: constants._cDarkerColor;
					}
					Text{
						id: swaptext;
						anchors.top: parent.top;
						anchors.left: parent.left;
						anchors.right: parent.right;
						anchors.leftMargin: vsep.width;
						anchors.rightMargin: vsep.width;
						height: constants._iSizeXL;
						horizontalAlignment: Text.AlignHCenter;
						verticalAlignment: Text.AlignVCenter;
						clip: true;
						wrapMode: Text.WrapAnywhere;
						font.pixelSize: constants._iFontLarge;
						font.bold: true;
						color: constants._cDarkestColor;
						text: swapicon.state === constants._sShowState ? qsTr("Part") : qsTr("Eposode");
					}
					ToolIcon{
						id: swapicon;
						anchors.centerIn: parent;
						anchors.left: parent.left;
						anchors.right: parent.right;
						anchors.leftMargin: vsep.width;
						anchors.rightMargin: vsep.width;
						height: width;
						iconId: "toolbar-next";
						state: contentsrow.state;

						transform: Rotation {
							id: rotation;
							origin: Qt.vector3d(swapicon.width / 2, swapicon.height / 2, 0);
							axis: Qt.vector3d(0, 1, 0);
							angle: 0;
						}
						states: [
							State{
								name: constants._sShowState;
								PropertyChanges {
									target: rotation;
									angle: -180;
								}
							},
							State{
								name: constants._sHideState;
								PropertyChanges {
									target: rotation;
									angle: 0;
								}
							}
						]
						transitions: [
							Transition {
								RotationAnimation {
									direction: RotationAnimation.Clockwise;
									duration: contentsrect.__swapDuration;
								}
							}
						]
						onClicked: {
							contentsrect.__Toggle();
						}
					}
					ToolIcon{
						anchors.bottom: parent.bottom;
						anchors.left: parent.left;
						anchors.right: parent.right;
						anchors.leftMargin: vsep.width;
						anchors.rightMargin: vsep.width;
						height: width;
						iconId: "toolbar-close";
						onClicked: {
							contentsrect._Toggle(false);
						}
					}
				}
				StreamGridWidget{
					id: partslist;
					width: contentsrect.width - swapbar.width;
					height: parent.height;
					model: st.playQueue;
					onClicked: {
						obj._LoadPart(index, data);
					}
				}
			}
		}
	}

	BusyIndicator{
		id: indicator;
		anchors.centerIn: parent;
		z: 99;
		platformStyle: BusyIndicatorStyle{
			size: "large";
			inverted: true;
		}
		visible: (v.playing && v.bufferProgress !== 1.0) || (st.bLoading);
		running: visible;
	}

	MouseArea{
		anchors.fill: parent;
		onClicked: {
			c._ToggleState();
		}
		onDoubleClicked: {
			v._TogglePlaying();
		}
		onPressAndHold: {
			root.trigger();
		}
	}

	QtObject{
		id: obj;

		// request a new content
		function _PlayContent(index, data)
		{
			console.log("Get content -> " + data.name);
			v._Reset();
			d._Reset();
			c._Reset();
			partslist.model = [];
			st._GetStreamtype(index);
		}

		// load content info, and play first part automatic
		function _LoadContents(suc)
		{
			contentslist.currentIndex = st.playType;
			partslist.model = st.playQueue;
			partslist.currentIndex = 0;
			_LoadPart(0);
		}

		// ready to play a part of content
		function _LoadPart(index, data, pos)
		{
			console.log("Play part -> " + index);
			if(contentsrect.state === constants._sShowState) contentsrect.__Toggle(2);
			var r = st._PlayPart(index, true);
			if(r >= 0)
			{
				var p = st._GetVideoUrl();
				partslist.currentIndex = st.playPart;
				_LoadSource(p);
			}
		}

		// play a part
		function _LoadSource(url, pos)
		{
			console.log("Play source -> " + url);
			if(st.isDashFormat)
			{
				_PlayExternally(url);
			}
			else
			{
				d._Seek();
				v._Load(url, pos);
				//c._Load();
			}
		}

		function _PlayExternally(url)
		{
			controller._ShowMessage(qsTr("Waiting KMPlayer for playing video(If can not play, please set MPlayer for decoding)..."));
			var headers = [
				"Referer: https://www.bilibili.com",
				"User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.84 Safari/537.36",
			];
			if(st.isDashFormat) _PLAYER.Play(url, st._GetDashAudio(), headers);
			else _PLAYER.Play(url, undefined, headers);
		}

		// seek position of video for current part
		function _Seek(value)
		{
			if(v.seekable)
			{
				var pos = v.duration * value;
				d._Seek(pos);
				v._SetPosition(pos);
			}
			else controller._ShowMessage(qsTr("Can not support seek for this video"));
		}

		// seek position of video for total progress
		function _SeekForTotal(value)
		{
			if(v.seekable)
			{
				var r = st._GetPartByDuration(st.totalDuration * value);
				if(r)
				{
					if(r.part !== st.playPart) _LoadPart(r.part, undefined, r.millisecond);
					else
					{
						d._Seek(r.millisecond);
						v._SetPosition(r.millisecond);
					}
				}
			}
			else
			{
				controller._ShowMessage(qsTr("Can not support seek for this video"));
			}
		}

		// play next part or stop
		function _EndOfMedia()
		{ 
			var r = st._PlayPart();
			if(r < 0) _Stop(); // no more
			else _LoadPart(r);
		}

		// when all parts of a content played finished, or stop button clicked
		function _Stop()
		{
			st._Stop();
			v._Stop();
			d._Stop();
			contentsrect.__Toggle(1);
			contentsrect._Toggle(true);
			root.stop();
		}

		// when close button clicked
		function _Exit()
		{
			_DeInit();
			root.exit();
		}

		function _Pause()
		{
			v._TogglePlaying(false);
		}
	}

	function _Init(id, cids, index, type)
	{
		if(st.mid !== id)
		{
			st._Reset();
			st._Load(id, cids, type);
			_Load(index);
		}
		else
		{
			if(st.playPart !== index)
			{
				_Load(index);
			}
		}
	}

	function _Load(index)
	{
		st._GetStreamtype(index);
	}

	function _DeInit()
	{
		v._Reset();
		d._Reset();
		st._Reset();
		__playerEnabled = false;
		_PLAYER.Stop();
	}

	Component.onDestruction: {
		//console.log("********** video player destroyed");
		_DeInit();
	}

}
