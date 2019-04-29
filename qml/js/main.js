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
		if(idAPI.MakeSearchResult(json, data.model) === 0)
		{
			var page_data = __GetPageData(json.data, ["page", "pagesize", "numResults", "numPages"]);
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
	if(data.pageNo)
		opt.page = data.pageNo;
	if(data.order)
		opt.order = data.order;
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
		var d = json.data;
		if(!d)
		{
			f(json.message);
			return;
		}
		if(idAPI.MakeContent(d, data.model) !== 0)
			f(json.message);

		var r = new Object();
		if(idAPI.MakeVideoInfo(json, r) === 0)
		{
			if(typeof(success) === "function") success(r);
		}
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
	if(data.pageNo)
		opt.pn = data.pageNo;
	Request(idAPI.COMMENT, "GET", opt, s, f);
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
	if(data.hasOwnProperty("rid"))
		opt.rid = data.rid;
	if(data.hasOwnProperty("day"))
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
	if(data.original)
		opt.original = data.original;
	if(data.day)
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
	if(data.pageNo)
		opt.pn = data.pageNo;
	if(data.pageSize)
		opt.ps = data.pageSize;
	if(data.type)
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
	if(data.pageNo)
		opt.pn = data.pageNo;
	if(data.pageSize)
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
	if(data.pageNo)
		opt.pn = data.pageNo;
	if(data.pageSize)
		opt.ps = data.pageSize;
	Request(idAPI.CATEGORY, "GET", opt, s, f);
}

function GetFullChannels(data, success, fail)
{
	if(typeof(success) === "function") success(idCategory);
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
		var d = json.data;
		if(!d)
		{
			f(json.message);
			return;
		}

		var r = new Object();
		if(idAPI.MakeUserInfo(json, r) === 0)
		{
			if(typeof(success) === "function") success(r);
		}
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
				"pageNo": data.pageNo,
				"pageSize": data.pageSize,
				"totalCount": count,
			};
			var page_data = __GetPageData(fake_json_data, ["pageNo", "pageSize", "totalCount", null]);
			if(typeof(success) === "function") success(page_data);
		}
		else
			f(json.message);
	};
	var opt = {
		mid: data.uid,
		tid: data.tid,
		order: data.order,
		keyword: "",
	};
	if(data.pageNo)
		opt.page = data.pageNo;
	if(data.pageSize)
		opt.pagesize = data.pageSize;
	if(data.keyword)
		opt.keyword = data.keyword;
	Request(idAPI.USER_VIDEOS, "GET", opt, s, f);
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
		if(props[0]) r.pageNo = obj[props[0]];
		if(props[1]) r.pageSize = obj[props[1]];
		if(props[2]) r.totalCount = obj[props[2]];

		if(props[3])
			r.pageCount = obj[props[3]];
		else
			r.pageCount = r.pageSize !== 0 ? Math.ceil(r.totalCount / r.pageSize) : 0;
	}
	return r;
}

