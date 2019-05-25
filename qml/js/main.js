.pragma library

Qt.include("network.js");
Qt.include("api.js");
Qt.include("util.js");
Qt.include("database.js");

var _UT;
var db = new idDatabase("ppsh", "PPSH database", 2 * 1024 * 1024);

function Init(ut)
{
	_UT = ut;
	db.Create('keyword', '(tid INTEGER PRIMARY KEY AUTOINCREMENT, keyword TEXT NOT NULL UNIQUE, ts INTEGER DEFAULT 0)');
	db.Create('view', '(tid INTEGER PRIMARY KEY AUTOINCREMENT, aid TEXT NOT NULL UNIQUE, title TEXT, preview TEXT, up TEXT, type INTEGER, ts INTEGER DEFAULT 0)');
}

function Request(url, method, args, success, fail, type)
{
    var req = new idNetwork(url, method, args, type);

    req.Request(success, fail);
		return req;
}

// SearchPage
function GetHotKeyword(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}
		if(idAPI.MakeHotKeyword(json, data.model, data.limit) === 0)
		{
			if(typeof(success) === "function") success();
		}
		else
			f(json.message);
	}
	Request(idAPI.HOT_KEYWORD, "GET", undefined, s, f);
}

// ResultPage
function SearchKeyword(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}
		if(idAPI.MakeSearchResult(json, data.model, data.type) === 0)
		{
			var page_json = null;
			if(data.type === "live")
			{
				var room = json.data.pageinfo.live_room;
				page_json = {
					"page": json.data.page,
					"pageSize": json.data.pagesize,
					"numResults":room.numResults,
						"numPages": room.numPages,
				};
			}
			else
				page_json = json.data;
			var page_data = __GetPageData(page_json, ["page", "pagesize", "numResults", "numPages"]);
			if(typeof(success) === "function") success(page_data);
		}
		else
			f(json.message);
	};
	var opt = {
		jsonp: "jsonp",
		search_type: "video",
//	highlight: 1,
		keyword: data.keyword,
	};
	if(data.pageNo !== undefined)
		opt.page = data.pageNo;
	if(data.order !== undefined)
		opt.order = data.order;
	if(data.type !== undefined)
		opt.search_type = data.type;
	Request(idAPI.SEARCH, "GET", opt, s, f);
}

// DetailPage
function GetVideoDetail(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}
		if(idAPI.MakeContent(json, data.model) !== 0)
			f(json.message);

		var r = new Object();
		if(idAPI.MakeVideoInfo(json, r) === 0)
		{
			if(typeof(success) === "function") success(r);
		}
		else
			f(json.message);
	};
	var opt = {
		aid: data.aid,
	};
	Request(idAPI.VIDEO_DETAIL, "GET", opt, s, f);
}

function GetRecommend(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}
		if(idAPI.MakeRecommend(json, data.model, data.limit) === 0)
		{
			if(typeof(success) === "function") success();
		}
		else
			f(json.message);
	};
	Request(idAPI.RECOMMEND.arg(data.aid), "GET", undefined, s, f);
}

function GetComment(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}
		if(idAPI.MakeComment(json, data.model) === 0)
		{
			var page_data = __GetPageData(json.data.page, ["num", "size", "count", null]);
			if(typeof(success) === "function") success(page_data);
		}
		else
			f(json.message);
	};
	var opt = {
		type: 1,
		sort: 0, // 2: hot
		oid: data.aid,
		nohot: 1,
	};
	if(data.pageNo !== undefined)
		opt.pn = data.pageNo;
	if(data.order !== undefined)
		opt.sort = data.order;
	if(data.type !== undefined)
		opt.type = data.type;
	Request(idAPI.COMMENT, "GET", opt, s, f);
}

function GetReply(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}
		if(idAPI.MakeComment(json, data.model) === 0)
		{
			var page_data = __GetPageData(json.data.page, ["num", "size", "count", null]);
			if(typeof(success) === "function") success(page_data);
		}
		else
			f(json.message);
	};
	var opt = {
		type: 1,
		oid: data.aid,
		root: data.rid,
	};
	if(data.pageNo !== undefined)
		opt.pn = data.pageNo;
	if(data.pageSize !== undefined)
		opt.ps = data.pageSize;
	if(data.type !== undefined)
		opt.type = data.type;
	Request(idAPI.COMMENT_REPLY, "GET", opt, s, f);
}

// MainPage
function GetRanking(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}
		if(idAPI.MakeRanking(json, data.model, data.limit) === 0)
		{
			var page_data = {
				info: json.data.info,
			};
			if(typeof(success) === "function") success(page_data);
		}
		else
			f(json.message);
	};
	var opt = {
		rid: 0,
		day: 1,
		jsonp: "jsonp",
	};
	if(data.rid !== undefined)
		opt.rid = data.rid;
	if(data.day !== undefined)
		opt.day = data.day;
	Request(idAPI.RANKING, "GET", opt, s, f);
}

// RankingPage
function GetChannels(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}
		if(idAPI.MakeChannels(json, data.model) === 0)
		{
			var page_data = {
				all_count: json.data.all_count,
				web_online: json.data.web_online,
				play_online: json.data.play_online,
			};
			if(typeof(success) === "function") success(page_data);
		}
		else
			f(json.message);
	};
	var opt = {
		jsonp: "jsonp",
	};
	Request(idAPI.CHANNELS_IDS, "GET", opt, s, f);
}

function GetCategoryRanking(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}

		if(idAPI.MakeCategoryRanking(json, data.model, data.limit) === 0)
		{
			if(typeof(success) === "function") success();
		}
		else
			f(json.message);
	};
	var opt = {
		jsonp: "jsonp",
		rid: data.rid,
		original: 0,
		day: 7,
	};
	if(data.original !== undefined)
		opt.original = data.original;
	if(data.day !== undefined)
		opt.day = data.day;
	Request(idAPI.CATEGORY_RANKING, "GET", opt, s, f);
}

function GetCategoryNewlist(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}

		if(idAPI.MakeCategoryNewlist(json, data.model) === 0)
		{
			var page_data = __GetPageData(json.data.page, ["num", "size", "count", null]);
			if(typeof(success) === "function") success(page_data);
		}
		else
			f(json.message);
	};
	var opt = {
		jsonp: "jsonp",
		rid: data.rid,
		type: 0,
	};
	if(data.pageNo !== undefined)
		opt.pn = data.pageNo;
	if(data.pageSize !== undefined)
		opt.ps = data.pageSize;
	if(data.type !== undefined)
		opt.type = data.type;
	Request(idAPI.CATEGORY_NEWLIST, "GET", opt, s, f);
}

function GetCategoryDynamic(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}

		if(idAPI.MakeCategoryDynamic(json, data.model) === 0)
		{
			var page_data = __GetPageData(json.data.page, ["num", "size", "count", null]);

			if(typeof(success) === "function") success(page_data);
		}
		else
			f(json.message);
	};
	var opt = {
		jsonp: "jsonp",
		rid: data.rid,
	};
	if(data.pageNo !== undefined)
		opt.pn = data.pageNo;
	if(data.pageSize !== undefined)
		opt.ps = data.pageSize;
	Request(idAPI.CATEGORY_DYNAMIC, "GET", opt, s, f);
}

// CategoryPage
function GetCategory(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}

		if(idAPI.MakeCategory(json, data.model) === 0)
		{
			var page_data = __GetPageData(json.data.page, ["num", "size", "count", null]);

			if(typeof(success) === "function") success(page_data);
		}
		else
			f(json.message);
	};
	var opt = {
		jsonp: "jsonp",
		tid: data.rid,
		ps: 20,
	};
	if(data.pageNo !== undefined)
		opt.pn = data.pageNo;
	if(data.pageSize !== undefined)
		opt.ps = data.pageSize;
	Request(idAPI.CATEGORY, "GET", opt, s, f);
}

function GetFullChannels(data, success, fail)
{
	if(data.local)
	{
		if(typeof(success) === "function") success(idCategory);
		return;
	}

	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}
		if(idAPI.MakeFullChannels(json, data.model) === 0)
		{
			if(typeof(success) === "function") success(data.model);
		}
		else
		{
			if(typeof(success) === "function") success(idCategory);
		}
	};
	Request(idAPI.CHANNELS, "GET", undefined, s, f);
}

// UserPage
function GetUserDetail(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}
		var r = new Object();
		if(idAPI.MakeUserInfo(json, r) === 0)
		{
			if(typeof(success) === "function") success(r);
		}
		else
			f(json.message);
	};
	var opt = {
		mid: data.uid,
		jsonp: "jsonp",
	};
	Request(idAPI.USER_DETAIL, "GET", opt, s, f);
}

function GetUserVideos(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var pn = data.pageNo ? data.pageNo : 1;
	var ps = data.pageSize ? data.pageSize : 20;
	var s = function(json){
		var res = json.status ? 0 : -1;
		if(res !== 0)
		{
			f(res);
			return;
		}
		if(idAPI.MakeUserVideos(json, data.model, undefined, data.tmodel) === 0)
		{
			var count = 0;
			if(data.tid == "0")
			{
				for(var k in json.data.tlist)
					count += json.data.tlist[k].count;
			}
			else
			{
				for(var k in json.data.tlist)
				{
					if(k == data.tid)
					{
						count = json.data.tlist[k].count;
						break;
					}
				}
			}
			var fake_json_data = {
				"pageNo": pn,
				"pageSize": ps,
				"totalCount": count,
			};
			var page_data = __GetPageData(fake_json_data, ["pageNo", "pageSize", "totalCount", null]);
			if(typeof(success) === "function") success(page_data);
		}
		else
			f(json.message);
	};
	var opt = {
		page: pn,
		pagesize: ps,
		mid: data.uid,
		tid: data.tid,
		order: data.order,
		keyword: "",
	};
	if(data.keyword !== undefined)
		opt.keyword = data.keyword;
	Request(idAPI.USER_VIDEOS, "GET", opt, s, f);
}

function GetUserArticles(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}
		if(idAPI.MakeUserArticles(json, data.model) === 0)
		{
			var page_data = __GetPageData(json.data, ["pn", "ps", "count", null]);
			if(typeof(success) === "function") success(page_data);
		}
		else
			f(json.message);
	};
	var opt = {
		mid: data.uid,
		sort: data.order,
		jsonp: "jsonp",
	};
	if(data.pageNo !== undefined)
		opt.pn = data.pageNo;
	if(data.pageSize !== undefined)
		opt.ps = data.pageSize;
	Request(idAPI.USER_ARTICLES, "GET", opt, s, f);
}

// BangumiPage
function GetBangumi(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}
		if(idAPI.MakeBangumi(json, data.model) === 0)
		{
			var page_data = __GetPageData(json.result.page, ["num", "size", "total", null]);
			if(typeof(success) === "function") success(page_data);
		}
		else
			f(json.message);
	};
	var opt = {
		season_version: -1,
		area: -1,
		is_finish: -1,
		season_status: -1,
		season_month: -1,
		pub_date: -1,
		style_id: -1,

		copyright: -1,
		st: 1,
		season_type: 1,

		sort: 0,
	};
	if(data.pageNo !== undefined)
		opt.page = data.pageNo;
	if(data.pageSize !== undefined)
		opt.pagesize = data.pageSize;
	if(data.order !== undefined)
		opt.order = data.order;
	if(data.sort !== undefined)
		opt.sort = data.sort;

	if(data.season_version !== undefined)
		opt.season_version = data.season_version;
	if(data.area !== undefined)
		opt.area = data.area;
	if(data.is_finish !== undefined)
		opt.is_finish = data.is_finish;
	if(data.season_status !== undefined)
		opt.season_status = data.season_status;
	if(data.season_month !== undefined)
		opt.season_month = data.season_month;
	if(data.pub_date !== undefined)
		opt.pub_date = data.pub_date;
	if(data.style_id !== undefined)
		opt.style_id = data.style_id;

	Request(idAPI.BANGUMI, "GET", opt, s, f);
}

// BangumiDetailPage
function GetBangumiDetail(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}
		if(data.season_model)
		{
			if(idAPI.MakeBangumiSeasons(json, data.season_model) !== 0)
				f(json.message);
		}

		if(data.episode_model)
		{
			if(idAPI.MakeBangumiEpisode(json, data.episode_model) !== 0)
				f(json.message);
		}

		var r = new Object();
		if(idAPI.MakeBangumiInfo(json, r) === 0)
		{
			if(typeof(success) === "function") success(r);
		}
		else
			f(json.message);
	};
	var opt = {
		season_id: data.sid,
	};
	Request(idAPI.BANGUMI_DETAIL, "GET", opt, s, f);
}

function GetBangumiRecommend(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}
		if(idAPI.MakeBangumiRecommend(json, data.model, data.limit) === 0)
		{
			if(typeof(success) === "function") success();
		}
		else
			f(json.message);
	};
	var opt = {
		season_id: data.sid,
	};
	Request(idAPI.BANGUMI_RECOMMEND, "GET", opt, s, f);
}

// LivePage
function GetLiveChannels(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}
		if(idAPI.MakeLiveChannels(json, data.model) === 0)
		{
			if(typeof(success) === "function") success(data.model);
		}
		else
			f(json.message);
	};
	Request(idAPI.LIVE_CHANNELS, "GET", undefined, s, f);
}

function GetLive(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var pn = data.pageNo ? data.pageNo : 1;
	var ps = data.pageSize ? data.pageSize : 20;
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}
		if(idAPI.MakeLive(json, data.model, data.limit) === 0)
		{
			var fake_json_data = {
				"pageNo": pn,
				"pageSize": ps,
				"totalCount": json.data.count,
			};
			var page_data = __GetPageData(fake_json_data, ["pageNo", "pageSize", "totalCount", null]);
			if(typeof(success) === "function") success(page_data);
		}
		else
			f(json.message);
	};
	var opt = {
		page: pn,
		page_size: ps,
		parent_area_id: 0,
		cate_id: 0,
		area_id: 0,
		sort_type: "online",
		platform: "web",
		//tag_version: 1,
	};
	if(data.pid !== undefined)
		opt.parent_area_id = data.pid;
	if(data.cid !== undefined)
		opt.cate_id = data.cid;
	if(data.aid !== undefined)
		opt.area_id = data.aid;
	if(data.order !== undefined)
		opt.sort_type = data.order;
	Request(idAPI.LIVE_ROOMS, "GET", opt, s, f);
}

// LiveDetailPage
function GetLiveRecommend(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}
		if(idAPI.MakeLiveRecommend(json, data.model, data.limit) === 0)
		{
			if(typeof(success) === "function") success();
		}
		else
			f(json.message);
	};
	var opt = {
		room_id: data.rid,
		count: 8,
	};
	if(data.count !== undefined)
		opt.count = data.count;
	Request(idAPI.LIVE_RECOMMEND, "GET", opt, s, f);
}

function GetLiveDetail(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}
		var r = new Object();
		if(idAPI.MakeLiveRoomInfo(json, r) === 0)
		{
			if(typeof(success) === "function") success(r);
		}
		else
			f(json.message);
	};
	var opt = {
		room_id: data.rid,
	};
	Request(idAPI.LIVE_ROOM_DETAIL, "GET", opt, s, f);
}

function GetLiveUserDetail(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}
		var r = new Object();
		if(idAPI.MakeLiveUserInfo(json, r) === 0)
		{
			if(typeof(success) === "function") success(r);
		}
		else
			f(json.message);
	};
	var opt = {
		roomid: data.rid,
	};
	Request(idAPI.LIVE_USER, "GET", opt, s, f);
}

function GetLiveQualityStreams(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}
		if(data.quality_model)
		{
			if(idAPI.MakeLiveQualitys(json, data.quality_model, data.rid, data.nosort) === 0)
			{
				if(typeof(success) === "function") success();
			}
			else
				f(json.message);
		}

		if(data.stream_model)
		{
			if(idAPI.MakeLiveStreams(json, data.stream_model) === 0)
			{
				if(typeof(success) === "function") success();
			}
			else
				f(json.message);
		}
	};
	var opt = {
		cid: data.rid,
		quality: 0,
		platform: "web",
	};
	if(data.quality !== undefined)
		opt.quality = data.quality;
	Request(idAPI.LIVE_URL, "GET", opt, s, f);
}

// ArticlePage
function GetArticleDetail(data, success, fail)
{
	var s = function(r){
		if(typeof(success) === "function") success(r);
	};
	s(idAPI.M_ARTICLE_URL.arg(data.aid));
}

// PlayerPage
function GetVideoUrl(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}
		if(idAPI.MakeVideoUrl(json, data.model, data.quality) !== 0)
			fail(json.message);
		if(typeof(success) === "function") success();
	};
	Request(idAPI.PLAYURL.arg(data.aid).arg(data.cid).arg(data.quality), "GET", undefined, s, f);
}

function GetBangumiUrl(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}
		if(idAPI.MakeBangumiUrl(json, data.model, data.quality) !== 0)
			fail(json.message);
		if(typeof(success) === "function") success();
	};
	Request(idAPI.BANGUMI_PLAYURL.arg(data.aid).arg(data.cid).arg(data.quality).arg(data.epid), "GET", undefined, s, f);
}

function GetLiveUrl(data, success, fail)
{
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		var res = idAPI.CheckResponse(json);
		if(res !== 0)
		{
			f(res);
			return;
		}
		if(idAPI.MakeLiveUrl(json, data.model, data.cid) !== 0)
			fail(json.message);
		if(typeof(success) === "function") success();
	};
	var opt = {
		cid: data.aid,
		quality: data.cid,
		platform: "web",
	};
	Request(idAPI.LIVE_URL, "GET", opt, s, f);
}

function GetDanmaku(data, success, fail)
{
	var usingCPP = (typefof(data.parser) === "string" && data.parser.toUpperCase() === "JS");
	var f = function(message){
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(xml){
		var res = false;
		if(usingCPP)
		{
			var ds = _UT.MakeDanmaku_cpp(xml, data.limit);
			if(ds)
			{
				data.model = ds;
				res = true;
			}
		}
		else
		{
			var json = _UT.XML_Parse(xml);
			res = (idAPI.MakeDanmaku(json, data.model, data.limit) === 0);
		}
		if(res)
		{
			if(typeof(success) === "function") success();
		}
		else
			f(qsTr("Get danmaku data fail"));
	};
	Request(idAPI.DANMAKU_XML.arg(data.cid), s, f);
}

// history
function AddViewHistory(aid, title, preview, up, type)
{
	db.Insert(db.Table("view"), [null, aid, title, preview, up, type, Date.now()]);
}

function ViewHistoryCount()
{
	return db.Count(db.Table("view"));
}

function GetViewHistory(model)
{
	db.Select(db.Table("view"), model);
}

function RemoveViewHistory(tid)
{
	if(tid !== undefined)
		db.Delete(db.Table("view"), "tid", tid.toString());
	else
		db.Drop(db.Table("view"));
}

// keyword
function AddKeywordHistory(kw)
{
	//db.Delete("keyword", "keyword", "'" + kw + "'");
	db.Insert(db.Table("keyword"), [null, kw, Date.now()]);
}

function KeywordHistoryCount()
{
	return db.Count(db.Table("keyword"));
}

function GetKeywordHistory(model)
{
	db.Select(db.Table("keyword"), model);
}

function RemoveKeywordHistory(keyword)
{
	if(keyword !== undefined)
		db.Delete(db.Table("keyword"), "keyword", "'" + keyword + "'");
	else
		db.Drop(db.Table("keyword"));
}

// other
function __GetPageData(obj, props)
{
	var r = {
		pageNo: 1,
		pageSize: 0,
		pageCount: 0,
		totalCount: 0,
	};
	if(obj)
	{
		if(props[0]) r.pageNo = obj[props[0]] || r.pageNo;
		if(props[1]) r.pageSize = obj[props[1]] || r.pageSize;
		if(props[2]) r.totalCount = obj[props[2]] || r.totalCount;

		if(props[3])
			r.pageCount = obj[props[3]] || r.pageCount;
		else
			r.pageCount = r.pageSize !== 0 ? Math.ceil(r.totalCount / r.pageSize) : 0;
	}
	return r;
}

// test
function TEST(data, success, fail)
{
	var f = function(message){
		console.log(message);
		if(typeof(fail) === "function")
			fail(message);
	};
	var s = function(json){
		console.log(JSON.stringify(json));
		if(typeof(success) === "function") success(json);
	};
	var room_id = data.id;
	var api_url = "http://www.douyutv.com/api/v1/";
	var	args = "room/%1?aid=wp&client_sys=wp&time=%2".arg(room_id).arg((Date.now() / 1000).toString());
	var	auth_md5 = (args + "zNzMV1y4EMxOHS6I5WKm"); //.encode("utf-8")
	var auth_str = Qt.md5(auth_md5);
	var json_request_url = "%1%2&auth=%3".arg(api_url).arg(args).arg(auth_str);

	var url = json_request_url;
	console.log(url);
	Request(url, "GET", undefined, s, f);
}
