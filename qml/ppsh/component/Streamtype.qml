import QtQuick 1.1
import "../../js/main.js" as Script
import "streamtype.js" as ST

QtObject{
	id: root;

	property int type: constants._eVideoType; // video bangumi
	property variant contents: []; // contents list: cid, title, duration
	// current
	property string mid; // mark
	property string aid;
	property string cid;
	property string epid;
	property int playType; // content index
	property string title; // content title
	property variant playQueue: []; // part model
	property variant danmaku: "";
	property int totalDuration: 0;
	property int playedDuration: 0;
	property bool isDashFormat: false;

	property int playPart: 0;

	property string sQuality: settings.iVideoQuality;
	property bool bLoading: false;
	property bool bUsingCache: true;

	signal streamtypeLoaded(string aid, string cid, string epid, bool suc);
	signal danmakuLoaded(string aid, bool suc);
	objectName: "idStreamtypeObect";

	function __GetStream()
	{
		for(var i in ST.streamtypes)
		{
			if(ST.streamtypes[i].index == playType)
			{
				return ST.streamtypes[i];
			}
		}
		return null;
	}

	function _LoadDanmaku(name, error, value)
	{
		if(ST.__danmakuRequest !== name) return;

		ST.__danmakuRequest = "";
		if(error !== 0)
		{
			controller._ShowMessage("[%1]: %2 -> %3(%4)".arg(qsTr("ERROR")).arg(name).arg(error).arg(value));
			danmakuLoaded(cid, false);
			return;
		}

		danmaku = value;
		root.danmakuLoaded(cid, true);
	}

	// js ajax
	function _GetDanmaku()
	{
		bLoading = true;
		var id = cid;

		var data = {
			cid: id,
			model: [],
			parser: "CPP",
		};

		var f = function(err){
			bLoading = false;
			root.danmakuLoaded(id, false);
			controller._ShowMessage(err);
		};
		var s = function(){
			if(data.model.length > 0)
			{
				root.danmakuLoaded(id, true);
				root.bLoading = false;
			}
			else console.log(qsTr("No video danmaku data"));
		};

		Script.GetDanmaku(data, s, f);
	}

	function _Load(id, _cids, t)
	{
		mid = id;
		contents = _cids;
		type = t !== undefined ? t : constants._eVideoType;
	}

	// it will change current play content
	function _GetStreamtype(idx)
	{
		if(contents.length === 0) return -1;

		var index = idx === undefined ? 0 : (typeof(idx) === "number" ? idx : _GetIndexByCid(idx));

		aid = contents[index].aid;
		cid = contents[index].cid;
		epid = contents[index].epid;
		if(!aid || !cid) return -1;
		var ida = aid;
		var id = cid;
		var idep = epid;
		playType = index;
		var t = contents[index].name;
		if(!bUsingCache) ST.streamtypes = [];
		_Ready();
		bLoading = true;
		if(bUsingCache)
		{
			for(var i in ST.streamtypes)
			{
				if(ST.streamtypes[i].index == index)
				{
					console.log("Using cache");
					__SetPlayQueue();
					if(type != constants._eLiveType)
					{
						if(settings.bOpenDanmaku) ST.__danmakuRequest = _CONNECTOR.Request(Script.idAPI.DANMAKU_XML.arg(id), "GET_DANMAKU_XML");
					}
					bLoading = false;
					root.streamtypeLoaded(ida, id, idep, true);
					return;
				}
			}
		}
		var data = {
			aid: ida,
			cid: id,
			epid: idep,
			quality: sQuality,
			model: ST.streamtypes,
		};

		var f = function(err){
			bLoading = false;
			controller._ShowMessage(err);
			root.streamtypeLoaded(ida, id, idep, false);
		};
		var s = function(){
			if(data.model.length > 0)
			{
				data.model[data.model.length - 1].index = index;
				data.model[data.model.length - 1].name = t;
				//ST.streamtypes = data.model; // now, streamtype in js file, so do not need to assign it
				root.__SetPlayQueue();
				if(type != constants._eLiveType)
				{
					if(settings.bOpenDanmaku) ST.__danmakuRequest = _CONNECTOR.Request(Script.idAPI.DANMAKU_XML.arg(id), "GET_DANMAKU_XML");
				}
				root.bLoading = false;
				root.streamtypeLoaded(ida, id, idep, true);
			}
			else f(qsTr("No video url data"));
		};

		if(type === constants._eBangumiType) Script.GetBangumiUrl(data, s, f);
		else if(type === constants._eLiveType) Script.GetLiveUrl(data, s, f);
		else Script.GetVideoUrl(data, s, f);
		return playType;
	}

	function __SetPlayQueue()
	{
		var stream = __GetStream();
		if(!stream) return;

		playType = stream.index;
		playQueue = stream.part;
		title = stream.name;
		totalDuration = stream.duration;
		playedDuration = 0;
		isDashFormat = stream.type === "dash";
	}

	// get video url
	function _GetVideoUrl(vp)
	{
		if(playQueue.length === 0) return false;

		var part = vp === undefined ? playPart : vp;
		var url = playQueue[/*playQueue.length ? playQueue[part].value : */part].url;
		return url;
	}

	// play stop
	function _Ready()
	{
		playPart = 0;
		playQueue = [];
		title = "";
		totalDuration = 0;
		playedDuration = 0;
		isDashFormat = false;
		danmaku = "";
	}

	function _EndOfMedia()
	{
		return _PlayPart();
	}
	// prev/next
	// it will change current part
	function _PlayPart(where, play)
	{
		if(playQueue.length === 0) return -3;
		var r = -128;

		var w = where !== undefined ? where : constants._sNextPage; // endOfMedia
		if(w === constants._sNextPage)
		{
			if(playPart < playQueue.length - 1)
			{
				if(play)
				{
					playPart++;
					playedDuration = _GetCurrentPlayedDuration();
					r = playPart;
				}
				else r = playPart + 1;
			}
			else
			{
				r = -1; // end of list
			}
		}
		else if(w === constants._sPrevPage)
		{
			if(playPart > 0)
			{
				if(play)
				{
					playPart--;
					playedDuration = _GetCurrentPlayedDuration();
					r = playPart;
				}
				else r = playPart - 1;
			}
			else
			{
				r = -2; // this is first
			}
		}
		else
		{
			if(typeof(w) === "number")
			{
				if(w < 0) r = -2;
				else if(w >= playQueue.length) r = -1;
				else
				{
					if(play)
					{
						playPart = w;
						playedDuration = _GetCurrentPlayedDuration();
						r = playPart;
					}
					else r = w;
				}
			}
		}
		return r; // nothing
	}

	// set state to end
	function _Stop()
	{
		if(playQueue.length === 0) return;

		playPart = playQueue.length - 1;
	}

	function _Reset()
	{
		_Ready();
		ST.streamtypes = [];
		playType = 0;
	}

	// get played time on current part = duration of before current part
	function _GetCurrentPlayedDuration(p)
	{
		if(playQueue.length === 0) return 0;

		var stream = __GetStream();
		if(!stream) return 0;

		var s = 0;
		var part = p === undefined ? playPart : p;
		for(var i = 0; i < part; i++)
		{
			s += playQueue[i].duration;
		}
		return Math.min(s, stream.duration);
	}

	// get current part by time
	function _GetPartByDuration(millisecond)
	{
		var stream = __GetStream();
		if(!stream) return null;

		if(playQueue.length === 0) return null;

		var ms = parseInt(millisecond);
		if(ms > stream.duration) return null;

		var cms = 0;
		var lms = 0;
		for(var i = 0; i < playQueue.length; i++)
		{
			cms += playQueue[i].duration;
			if(ms <= cms)
			{
				return({part: i, millisecond: ms - lms});
			}
			lms = cms;
		}
		return null;
	}

	function _GetDashAudio()
	{
		var stream = __GetStream();
		if(!stream) return;

		return stream.audio;
	}

	function _GetIndexByCid(id)
	{
		for(var i in contents)
		{
			if(contents[i].cid == id)
			{
				return i;
			}
		}
		return -1;
	}

}
