import QtQuick 1.1
import com.nokia.meego 1.1
import QtMobility.systeminfo 1.1
import karin.ppsh 1.0

PPSHVideo {
	id: root;

	property Item control;
	signal endOfMedia;
	signal errorTriggered(int errno, string errstr);
	objectName: "idVideoWidget";
	
	headersEnabled: true;
	requestHeaders: [
		{
			name: "User-Agent",
			value: "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_12_6) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.84 Safari/537.36",
		},
		{
			name: "Referer",
			value: "https://www.bilibili.com",
		},
	];
	onError:{
		if(error !== PPSHVideo.NoError){
			root.errorTriggered(error, errorString);
			stop();
			source = "";
			position = 0;
		}
	}
	onStatusChanged: {
		if(status === PPSHVideo.EndOfMedia)
		{
			position = 0;
			root.endOfMedia();
		}
	}
	volume: (devinfo.voiceRingtoneVolume > 50 ? 50 : devinfo.voiceRingtoneVolume < 20 ? 20 : devinfo.voiceRingtoneVolume) / 100;
	focus: true
	Keys.onSpacePressed: _TogglePlaying();
	Keys.onLeftPressed: position -= 5000;
	Keys.onRightPressed: position += 5000;
	onPlayingChanged: {
		screensaver.setScreenSaverDelayed(playing);
	}

	DeviceInfo {
		id: devinfo;
	}
	ScreenSaver{
		id: screensaver;
		//screenSaverDelayed: root.playing && !root.paused;
	}

	function _SetFillMode(value)
	{
		switch(value)
		{
			case 0:
			fillMode = PPSHVideo.Stretch;
			break;
			case 2:
			fillMode = PPSHVideo.PreserveAspectCrop;
			break;
			case 1:
			default:
			fillMode = PPSHVideo.PreserveAspectFit;
			break;
		}
	}

	function _TogglePlaying(on)
	{
		if(on === undefined) paused = !paused;
		else
		{
			if(on) play();
			else pause();
		}
	}

	function _Stop()
	{
		stop();
		position = 0;
	}

	function _Reset()
	{
		_Stop();
		source = "";
	}

	function _Load(src, pos)
	{
		source = src;
		if(source != "")
		{
			fillMode = PPSHVideo.PreserveAspectFit;
			play();

			if(pos && seekable) position = pos;
		}
	}

	function _SetPosition(pos)
	{
		if(!seekable)
			return;

		if(source != "")
		{
			var p = pos === undefined ? 0 : pos;
			position = p;
		}
	}

	function _SetPercent(per)
	{
		if(!seekable)
			return;

		if(video.source != "")
		{
			var p = duration * per;
			position = p;
		}
	}
}

