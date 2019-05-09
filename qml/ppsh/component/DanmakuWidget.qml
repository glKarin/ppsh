import QtQuick 1.1
import com.nokia.meego 1.1
import "../../js/main.js" as Script
import "../../js/util.js" as Util
import "danmaku.js" as D
import "itemlist.js" as L

Item{
	id: root;

	property int iBaseTime: 0;
	property int iPlayTime: 0;
	property int iDuration: 2000;
	property int iMaxDuration: 4500;
	property int iMinDuration: 3500;
	property int iDiscardDuration: 1000;
	property int iMax: settings.iDanmakuLimit;
	property bool bRunning: false;

	objectName: "idDanmakuWidget";
	clip: true;

	Component{
		id: danmakucomp;
		Text{
			id: textroot;
			property real size: 25;
			property int mode: 1; // 1 - slide, 5 - top, 4 - bottom
			property int duration: 0;

			opacity: settings.fDanmakuOpacity;
			visible: settings.bOpenDanmaku;
			font.pixelSize: size * settings.fDanmakuSize;
			anchors.horizontalCenter: (mode === 4 || mode === 5) ? parent.horizontalCenter : undefined;
			textFormat: Text.PlainText;

			// 1, 4, 5
			PropertyAnimation{
				id: anim1;
				running: true; //root.bRunning;
				target: textroot;
				property: textroot.mode === 1 ? "x" : "visible";
				from: textroot.mode === 1 ? root.width : true;
				to: textroot.mode == 1 ? -textroot.width : false;
				duration: textroot.duration / settings.fDanmakuSpeed / (textroot.mode === 1 ? 1 : 2);
				onCompleted: {
					textroot.__Destroy();
				}
			}
			/*
			// 5, 4
			Timer{
				id: anim2;
				interval: textroot.duration / settings.fDanmakuSpeed / 2;
				repeat: false;
				running: (textroot.mode === 5 || textroot.mode === 4) && root.bRunning;
				onTriggered: {
					textroot.__Destroy();
				}
			}
			*/

			function __Destroy()
			{
				textroot.destroy();
			}

			Component.onCompleted: {
				if(mode === 5) D.__current_top++;
				else if(mode === 4) D.__current_bottom++;
				else D.__current_slide++;

				D.__current++;
			}

			Component.onDestruction: {
				if(mode === 5 && D.__current_top > 0) D.__current_top--;
				else if(mode === 4 && D.__current_bottom > 0) D.__current_bottom--;
				else if(D.__current_slide > 0) D.__current_slide--;

				if(D.__current > 0) D.__current--;
				//console.log(D.__current, D.__current_top, D.__current_bottom, D.__current_slide, textroot.text, "destroy", mode);
			}
		}
	}

	function _LoadDanmaku(pos)
	{
		if(!bRunning || !enabled) return;

		var time = iBaseTime + (pos !== undefined ? pos : iPlayTime);
		var h = 0;
		// maybe less than 0 when call _Stop function and danmaku text destroy
		/*
		if(D.__current_slide < 0) D.__current_slide = 0;
		if(D.__current_top < 0) D.__current_top = 0;
		if(D.__current_bottom < 0) D.__current_bottom = 0;
		*/

		if(D.__current_slide <= 0) D.__currentY_slide = 0;
		if(D.__current_top <= 0) D.__currentY_top = 0;
		if(D.__current_bottom <= 0) D.__currentY_bottom = 0;

		var i;
		for(i = D.iIndex; i < D.st_danmaku.length; i++)
		{
			var e = D.st_danmaku[i];
			if(e.time > time) break; // after
			if(iMax > 0) // if limit max count
			{
				if(D.__current >= iMax) break; // out limit
				//if(iDiscardDuration > 0 && time - e.time > iDiscardDuration) continue; // delta time greater, discard
			}

			if(iDiscardDuration > 0 && time - e.time > iDiscardDuration) continue; // delta time greater, discard
			if(D.Modes.indexOf(e.mode) === -1) continue; // mode not support

			h = e.size * settings.fDanmakuSize + constants._iSpacingSmall;

			if(e.mode === 5)
			{
				if(D.__currentY_top + h > root.height) D.__currentY_top = 0;
			}
			else if(e.mode === 4)
			{
				if(D.__currentY_bottom + h > root.height) D.__currentY_bottom = 0;
			}
			else
			{
				if(D.__currentY_slide + h > root.height) D.__currentY_slide = 0;
			}

			var prop = {
				mode: e.mode,
				text: e.content,
				size: e.size,
				color: e.color,
				z: e.mode,
				y: e.mode === 5 ? D.__currentY_top : (e.mode === 4 ? (root.height - D.__currentY_bottom - h) : D.__currentY_slide),
				duration: Math.max(Math.min(root.iDuration * ((root.width / (e.size * e.content.length))), root.iMaxDuration), root.iMinDuration),
			};

			if(e.mode === 5) D.__currentY_top += h;
			else if(e.mode === 4) D.__currentY_bottom += h;
			else D.__currentY_slide += h;

			var item = danmakucomp.createObject(root, prop);
			L.Push(item);
		}
		D.iIndex = i;
		iPlayTime = pos;
		D.__lastTimestamp = time;
	}

	function _Seek(pos)
	{
		var time = iBaseTime + (pos ? pos : 0);
		var i = time > D.__lastTimestamp ? D.iIndex : 0;
		//console.log(time, D.__lastTimestamp, i);
		for(; i < D.st_danmaku.length; i++)
		{
			if(D.st_danmaku[i].time >= time) break;
		}
		D.iIndex = i;
	}

	function _Toggle(on)
	{
		if(on === undefined)
		{
			bRunning = !bRunning;
		}
		else
		{
			bRunning = on ? true : false;
		}
	}

	function _Reset()
	{
		L.Clear();
		root.children = [];
		_Stop();
		iPlayTime = 0;
		D.st_danmaku = [];
	}

	function _Load(str)
	{
		_Reset();
		D.st_danmaku = _UT.MakeDanmaku_cpp(str);
		//_Toggle(true);
	}

	function _Stop()
	{
		//_Toggle(false);
		//D.iIndex = D.st_danmaku.length > 0 ? D.st_danmaku - 1 : 0;
		D.__current = 0;
		D.__currentY_slide = 0;
		D.__currentY_top = 0;
		D.__currentY_bottom = 0;
		D.__current_slide = 0;
		D.__current_top = 0;
		D.__current_bottom = 0;
		D.__lastTimestamp = 0;
		D.iIndex = 0;
	}

}
