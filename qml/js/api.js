.pragma library

Qt.include("util.js");

var BCategorys = [
{
	name: "动画",
	rid: "1",
	pid: "0",
	children: [
	{
		name: "MAD·AMV",
		rid: "24",
	},
	{
		name: "MMD·3D",
		rid: "25",
	},
	{
		name: "短片·手书·配音",
		rid: "47",
	},
	{
		name: "综合",
		rid: "27",
	},
	],
},
{
	name: "番剧",
	rid: "13",
	pid: "0",
	children: [
	{
		name: "连载动画",
		rid: "33",
	},
	{
		name: "完结动画",
		rid: "32",
	},
	{
		name: "资讯",
		rid: "51",
	},
	{
		name: "官方延伸",
		rid: "152",
	},
	{
		name: "新番时间表",
		rid: "",
	},
	{
		name: "番剧索引",
		rid: "",
	},
	],
},
{
	name: "国创",
	rid: "167",
	pid: "0",
	children: [
	{
		name: "国产动画",
		rid: "153",
	},
	{
		name: "国产原创相关",
		rid: "168",
	},
	{
		name: "布袋戏",
		rid: "169",
	},
	{
		name: "动态漫·广播剧",
		rid: "195",
	},
	{
		name: "资讯",
		rid: "170",
	},
	{
		name: "新番时间表",
		rid: "",
	},
	{
		name: "国产动画索引",
		rid: "",
	},
	],
},
{
	name: "音乐",
	rid: "3",
	pid: "0",
	children: [
	{
		name: "原创音乐",
		rid: "28",
	},
	{
		name: "翻唱",
		rid: "31",
	},
	{
		name: "VOCALOID·UTAU",
		rid: "30",
	},
	{
		name: "电音",
		rid: "194",
	},
	{
		name: "演奏",
		rid: "59",
	},
	{
		name: "MV",
		rid: "193",
	},
	{
		name: "音乐现场",
		rid: "29",
	},
	{
		name: "音乐综合",
		rid: "130",
	},
	{
		name: "音频",
		rid: "",
	},
	],
},
{
	name: "舞蹈",
	rid: "129",
	pid: "0",
	children: [
	{
		name: "宅舞",
		rid: "20",
	},
	{
		name: "三次元舞蹈",
		rid: "154",
	},
	{
		name: "舞蹈教程",
		rid: "156",
	},
	],
},
{
	name: "游戏",
	rid: "4",
	pid: "0",
	children: [
	{
		name: "单机游戏",
		rid: "17",
	},
	{
		name: "电子竞技",
		rid: "171",
	},
	{
		name: "手机游戏",
		rid: "172",
	},
	{
		name: "网络游戏",
		rid: "65",
	},
	{
		name: "桌游棋牌",
		rid: "173",
	},
	{
		name: "GMV",
		rid: "121",
	},
	{
		name: "音游",
		rid: "136",
	},
	{
		name: "Mugen",
		rid: "19",
	},
	{
		name: "游戏赛事",
		rid: "",
	},
	],
},
{
	name: "科技",
	rid: "36",
	pid: "0",
	children: [
	{
		name: "趣味科普人文",
		rid: "124",
	},
	{
		name: "野生技术协会",
		rid: "122",
	},
	{
		name: "演讲公开课",
		rid: "39",
	},
	{
		name: "星海",
		rid: "96",
	},
	{
		name: "机械",
		rid: "98",
	},
	{
		name: "汽车",
		rid: "176",
	},
	],
},
{
	name: "数码",
	rid: "188",
	pid: "0",
	children: [
	{
		name: "手机平板",
		rid: "95",
	},
	{
		name: "电脑装机",
		rid: "189",
	},
	{
		name: "摄影摄像",
		rid: "190",
	},
	{
		name: "影音智能",
		rid: "191",
	},
	],
},
{
	name: "生活",
	rid: "160",
	pid: "0",
	children: [
	{
		name: "搞笑",
		rid: "138",
	},
	{
		name: "日常",
		rid: "21",
	},
	{
		name: "美食圈",
		rid: "76",
	},
	{
		name: "动物圈",
		rid: "75",
	},
	{
		name: "手工",
		rid: "161",
	},
	{
		name: "绘画",
		rid: "162",
	},
	{
		name: "运动",
		rid: "163",
	},
	{
		name: "其他",
		rid: "174",
	},
	],
},
{
	name: "鬼畜",
	rid: "119",
	pid: "0",
	children: [
	{
		name: "鬼畜调教",
		rid: "22",
	},
	{
		name: "音MAD",
		rid: "26",
	},
	{
		name: "人力VOCALOID",
		rid: "126",
	},
	{
		name: "教程演示",
		rid: "127",
	},
	],
},
{
	name: "时尚",
	rid: "155",
	pid: "0",
	children: [
	{
		name: "美妆",
		rid: "157",
	},
	{
		name: "服饰",
		rid: "158",
	},
	{
		name: "健身",
		rid: "164",
	},
	{
		name: "T台",
		rid: "159",
	},
	{
		name: "风尚标",
		rid: "192",
	},
	],
},
{
	name: "广告",
	rid: "165",
	pid: "0",
	children: [
	],
},
{
	name: "娱乐",
	rid: "5",
	pid: "0",
	children: [
	{
		name: "综艺",
		rid: "71",
	},
	{
		name: "明星",
		rid: "137",
	},
	{
		name: "Korea相关",
		rid: "131",
	},
	],
},
{
	name: "影视",
	rid: "181",
	pid: "0",
	children: [
	{
		name: "影视杂谈",
		rid: "182",
	},
	{
		name: "影视剪辑",
		rid: "183",
	},
	{
		name: "短片",
		rid: "85",
	},
	{
		name: "预告·资讯",
		rid: "184",
	},
	{
		name: "特摄",
		rid: "86",
	},
	],
},
{
	name: "放映厅",
	rid: "",
	pid: "0",
	children: [
		],
},
{
	name: "放映厅·纪录片",
	rid: "177",
	pid: "0",
	children: [
	{
		name: "人文·历史",
		rid: "37",
	},
	{
		name: "科学·探索·自然",
		rid: "178",
	},
	{
		name: "军事",
		rid: "179",
	},
	{
		name: "社会·美食·旅行",
		rid: "180",
	},
	{
		name: "纪录片索引",
		rid: "",
	},
	],
},
{
	name: "放映厅·电影",
	rid: "23",
	pid: "0",
	children: [
	{
		name: "华语电影",
		rid: "147",
	},
	{
		name: "欧美电影",
		rid: "145",
	},
	{
		name: "日本电影",
		rid: "146",
	},
	{
		name: "其他国家",
		rid: "83",
	},
	{
		name: "电影索引",
		rid: "",
	},
	],
},
{
	name: "放映厅·电视剧",
	rid: "11",
	pid: "0",
	children: [
	{
		name: "国产剧",
		rid: "185",
	},
	{
		name: "海外剧",
		rid: "187",
	},
	{
		name: "电视剧索引",
		rid: "",
	},
	],
},
	];

var idCategory = {
	name: "Home",
	rid: "0",
	children: BCategorys,
};

function GetChannelNameById(id)
{
	var f = function(id, obj){
		if(obj.rid == id)
			return obj.name;
		if(!obj.children)
			return false;
		for(var i in obj.children)
		{
			var r = f(id, obj.children[i]);
			if(r)
				return r;
		}
		return false;
	};

	return f(id.toString(), idCategory);
}

var __JSON_Print = function(json)
{
	console.log(JSON.stringify(json));
};

var idWebAPI = {
	HOT_KEYWORD: "http://s.search.bilibili.com/main/hotword",
	SEARCH: "http://api.bilibili.com/x/web-interface/search/type", //?jsonp=jsonp&search_type=video|bili_user&highlight=1&keyword=n950&from_source=banner_search&page=1 &order=click|pubdate|dm|stow
	RECOMMEND: "http://comment.bilibili.com/recommendnew,%1",
	COMMENT: "http://api.bilibili.com/x/v2/reply", //?type=1&sort=2&oid=49156714&pn=1&nohot=1
	VIDEO_DETAIL: "http://api.bilibili.com/x/web-interface/view", //?aid=37606630
	RANKING: "http://api.bilibili.com/x/web-interface/ranking", //?rid=0&day=3&jsonp=jsonp
	PLAYURL: "http://api.bilibili.com/x/player/playurl?avid=%1&cid=%2&qn=%3&type=&otype=json&fnver=0&fnval=16",
	DANMAKU_XML: "http://comment.bilibili.com/%1.xml", // http://api.bilibili.com/x/v1/dm/list.so?oid={cid}
	DEFAULT_KEYWORD: "http://api.bilibili.com/x/web-interface/search/default",
	CHANNELS_IDS: "http://api.bilibili.com/x/web-interface/online", //?jsonp=jsonp
	CATEGORY_NEWLIST: "http://api.bilibili.com/x/web-interface/newlist", //?rid=20&type=0&pn=1&ps=20&jsonp=jsonp
	CATEGORY_RANKING: "http://api.bilibili.com/x/web-interface/ranking/region", //?jsonp=jsonp&rid=20&day=7&original=0
	CATEGORY_DYNAMIC: "http://api.bilibili.com/x/web-interface/dynamic/region", //?&jsonp=jsonp&pn=1&ps=5&rid=20
	CATEGORY: "http://api.bilibili.com/archive_rank/getarchiverankbypartion", //?jsonp=jsonp&tid=27&pn=2 &ps=?
	USER_DETAIL: "http://api.bilibili.com/x/web-interface/card", //?mid=2989565&jsonp=jsonp,
	USER_VIDEOS: "http://space.bilibili.com/ajax/member/getSubmitVideos", //?mid=3487048&pagesize=30&tid=0&page=2&keyword=&order=update|click|stow



	__MakePreviewPath: function(pic){
		if(pic.indexOf("http://") !== -1 || pic.indexOf("https://") !== -1)
			return pic;
		return "http:" + pic;
	},


	CheckResponse: function(json){
		if(json.code != 0)
			return "[%1]: %2".arg(json.code).arg(json.message);
		else
			return 0;
	},

	MakeHotKeyword: function(json, container, limit){
		if(!Array.isArray(json.list))
			return -1;
		var push = Array.isArray(container) ? "push" : "append";
		var list = json.list;
		for(var i in list)
		{
			if(limit && i >= limit)
				break;

			var e = list[i];
			var item = {
				keyword: e.keyword,
			};
			container[push](item);
		}
		return 0;
	},

	MakeSearchResult: function(json, container, limit){
		if(!json.data)
			return -1;
		if(!Array.isArray(json.data.result))
			return -1;
		var push = Array.isArray(container) ? "push" : "append";
		var list = json.data.result;
		for(var i in list)
		{
			if(limit && i >= limit)
				break;

			var e = list[i];
			var item = {
				aid: e.id.toString(),
				title: e.title,
				up: e.author,
				view_count: e.play,
				danmu_count: e.video_review,
				duration: e.duration,
				preview: this.__MakePreviewPath(e.pic),
			};
			container[push](item);
		}
		return 0;
	},

	MakeRecommend: function(json, container, limit){
		if(!Array.isArray(json.data))
			return -1;
		var push = Array.isArray(container) ? "push" : "append";
		var list = json.data;
		for(var i in list)
		{
			if(limit && i >= limit)
				break;

			var e = list[i];
			var item = {
				aid: e.aid.toString(),
				title: e.title,
				up: e.owner ? e.owner.name : "",
				view_count: e.stat ? e.stat.view : 0,
				danmu_count: e.stat ? e.stat.danmaku : 0,
				preview: this.__MakePreviewPath(e.pic),
			};
			container[push](item);
		}
		return 0;
	},

	MakeComment: function(json, container, limit){
		if(!json.data)
			return -1;
		if(!Array.isArray(json.data.replies))
			return -1;
		var push = Array.isArray(container) ? "push" : "append";
		var list = json.data.replies;
		for(var i in list)
		{
			if(limit && i >= limit)
				break;

			var e = list[i];
			var item = {
				name: e.member ? e.member.uname : "",
				uid: e.member ? e.member.mid.toString() : "",
				avatar: e.member ? e.member.avatar : "",
				level: e.member ? e.member.level_info.current_level : 0,
				vip: e.member ? e.member.vip.vipType : 0,

				content: e.content ? e.content.message : "",
				create_time: e.ctime,
				rpid: e.rpid,
				like: e.like,
				reply_count: e.rcount,
				floor: e.floor,
			};
			container[push](item);
		}
		return 0;
	},

	MakeContent: function(json, container, limit){
		if(!Array.isArray(json.pages))
			return -1;
		var push = Array.isArray(container) ? "push" : "append";
		var list = json.pages;
		for(var i in list)
		{
			if(limit && i >= limit)
				break;

			var e = list[i];
			var item = {
				cid: e.cid.toString(),
				page: e.page,
				name: e.part,
				duration: e.duration,
			};
			container[push](item);
		}
		return 0;
	},

	MakeRanking: function(json, container, limit){
		if(!json.data)
			return -1;
		if(!Array.isArray(json.data.list))
			return -1;
		var push = Array.isArray(container) ? "push" : "append";
		var list = json.data.list;
		for(var i in list)
		{
			if(limit && i >= limit)
				break;

			var e = list[i];
			var item = {
				aid: e.aid.toString(),
				title: e.title,
				up: e.author,
				view_count: e.play,
				danmu_count: e.video_review,
				duration: e.duration,
				preview: e.pic,
			};
			container[push](item);
		}
		return 0;
	},

	MakeChannels: function(json, container){
		if(!json.data)
			return -1;
		if(!json.data.region_count)
			return -1;
		var push = Array.isArray(container) ? "push" : "append";
		var list = json.data.region_count;
		for(var i in list)
		{
			var e = list[i];
			var item = {
				value: i,
				count: e,
				name: i,
			};
			container[push](item);
		}
		return 0;
	},

	MakeCategoryDynamic: function(json, container, limit){
		if(!json.data)
			return -1;
		if(!Array.isArray(json.data.archives))
			return -1;
		var push = Array.isArray(container) ? "push" : "append";
		var list = json.data.archives;
		for(var i in list)
		{
			if(limit && i >= limit)
				break;

			var e = list[i];
			var item = {
				aid: e.aid.toString(),
				title: e.title,
				up: e.owner ? e.owner.name : "",
				view_count: e.stat ? e.stat.view : 0,
				danmu_count: e.stat ? e.stat.view : 0,
				duration: FormatDuration(e.duration),
				preview: e.pic,
			};
			container[push](item);
		}
		return 0;
	},

	MakeCategoryRanking: function(json, container, limit){
		if(!Array.isArray(json.data))
			return -1;
		var push = Array.isArray(container) ? "push" : "append";
		var list = json.data;
		for(var i in list)
		{
			if(limit && i >= limit)
				break;

			var e = list[i];
			var item = {
				aid: e.aid.toString(),
				title: e.title,
				up: e.author,
				view_count: e.play,
				danmu_count: e.video_review,
				duration: e.duration,
				preview: e.pic,
			};
			container[push](item);
		}
		return 0;
	},

	MakeCategoryNewlist: function(json, container, limit){
		return this.MakeCategoryDynamic(json, container, limit);
	},

	MakeVideoInfo: function(json, ret){
		if(!json.data)
			return -1;

		var r = ret ? ret : {};
		var d = json.data;

		r.aid = d.aid.toString();
		r.preview = d.pic;
		r.title = d.title;
		r.desc = d.desc;
		r.videos = d.videos;

		r.create_time = d.ctime;
		r.danmu_count = d.stat ? d.stat.danmaku : 0;
		r.reply = d.stat ? d.stat.reply : 0;
		r.view_count = d.stat ? d.stat.view : 0;

		r.like = d.stat ? d.stat.like : 0;
		r.coin = d.stat ? d.stat.coin : 0;
		r.share = d.stat ? d.stat.share : 0;
		r.dislike = d.stat ? d.stat.dislike : 0;
		r.favorite = d.stat ? d.stat.favorite : 0;

		r.up = d.owner ? d.owner.name : "";
		r.uid = d.owner ? d.owner.mid.toString() : "";
		r.avatar = d.owner ? d.owner.face : "";

		return ret ? 0 : r;
	},

	MakeUserInfo: function(json, ret){
		if(!json.data)
			return -1;

		var r = ret ? ret : {};
		var data = json.data;
		var card = data.card;

		r.uid = card.mid.toString();
		r.preview = card.face;
		r.avatar = card.face;
		r.up = card.name;
		r.sign = card.sign;
		r.sex = card.sex;
		r.current_level = card.level_info ? card.level_info.current_level : 0;
		r.following = card.friend;
		r.follower = data.follower || card.fans;
		r.vipType = card.vip ? card.vip.vipType : 0;
		r.vipStatus = card.vip ? card.vip.vipStatus : 0;
		r.official = card.official_verify ? card.official_verify.desc : "";
		r.archive_count = data.archive_count;
		r.article_count = data.article_count;

		return ret ? 0 : r;
	},

	MakeCategory: function(json, container, limit){
		return this.MakeCategoryDynamic(json, container, limit);
	},

	MakeUserVideos: function(json, container, limit, t_container){
		if(!json.data)
			return -1;
		if(!Array.isArray(json.data.vlist))
			return -1;
		{
			var push = Array.isArray(container) ? "push" : "append";
			var list = json.data.vlist;
			for(var i in list)
			{
				if(limit && i >= limit)
					break;

				var e = list[i];
				var item = {
					aid: e.aid.toString(),
					title: e.title,
					up: e.author,
					view_count: e.play,
					danmu_count: e.video_review,
					duration: e.length,
					preview: this.__MakePreviewPath(e.pic),
				};
				container[push](item);
			}
		}

		if(t_container && json.data.tlist)
		{
			var push = Array.isArray(t_container) ? "push" : "append";
			var list = json.data.tlist;
			var count = 0;
			for(var i in list)
				count += list[i].count;
			t_container[push]({
				value: "0",
				name: qsTr("All") + "[" + count + "]",
			});
			for(var i in list)
			{
				var e = list[i];
				var item = {
					value: e.tid.toString(),
					name: e.name + "[" + e.count + "]",
				};
				t_container[push](item);
			}
		}

		return 0;
	},

	MakeVideoUrl: function(json, container, quality){
		if(!json.data)
			return -1;

		var sort_dash_video = function(video){
			var arr = [];
			for(var i in video)
			{
				var v = video[i];
				var q = parseInt(v.id);
				if(q == 16)
					var has = false;
				for(var j in arr)
				{
					var a = arr[j];
					if(a.quality == q)
					{
						a.video.push(v);
						has = true;
						break;
					}
				}
				if(!has)
				{
					var a = {
						quality: q,
						video: [v],
					};
					arr.push(a);
				}
			}
			arr.sort(function(a, b){
				return a.quality - b.quality;
			});
			return arr;
		};

		var sort_dash_video_codec = function(arr)
		{
			var Codecs = [
				"avc1.64001E",
				"hev1.1.6.L120.90",
			];
			arr.sort(function(a, b){
				if(a.codesc !== b.codecs)
					return Codecs.indexOf(a.codecs) - Codecs.indexOf(b.codecs);
				return 0;
			});
		};

		var get_dash_audio = function(audios, qn)
		{
			var a = audios[audios.length - 1];
			var aq = qn == "16" ? 30216 : 30280;
			for(var i in audios)
			{
				if(audios[i].id == aq)
				{
					a = audios[i];
					break;
				}
			}
			if(a)
				return a.baseUrl || a.base_url;
			else
				return "";
		}

		var push = Array.isArray(container) ? "push" : "append";
		var data = json.data;
		var total_size = 0;
		var arr = [];
		var title = "part";
		if (Array.isArray(data.durl))
		{
			var durl = data.durl;
			for(var i in durl)
			{
				var e = durl[i];
				arr.push({
					title: title + "[" + i + "]", 
					name: e.url ? i : "" + i + "*",
					url: e.url,
					value: i,
					duration: e.length,
					size: e.size,
				});
				total_size += e.size;
			}
			var item = {
				name: title,
				index: 0,
				size: total_size,
				duration: data.timelength,
				part: arr,
				type: "durl",
			};
			container[push](item);
		}
		else
		{ // dash
			var urls = sort_dash_video(data["dash"].video);
			var audio = get_dash_audio(data["dash"].audio, quality);
			var first = urls[0];
			sort_dash_video_codec(first.video);
			var video = first.video;
			for(var i in video)
			{
				var e = video[i];
				var eu = e.baseUrl || e.base_url;
				arr.push({
					title: title + "[" + i + "]", 
					name: eu ? i : "" + i + "*",
					url: eu,
					value: i,
					duration: 0,
					size: 0,
				});
				total_size += 0;
			}
			var item = {
				name: title,
				index: 0,
				size: total_size,
				duration: data.timelength,
				part: arr,
				type: "dash",

				audio: audio,
			};
			container[push](item);
		}
		return 0;
	},

	MakeDanmaku: function(json, container, limit){
		var int_to_color = function(i){
			var s = parseInt(arr[3]).toString(16);
			while(s.length < 6)
				s = "0" + s;
			return "#" + s;
		};
		var parse_p = function(p){
			var arr = p.split(",");
			// "p":"36.47000,1,25,16777215,1506225558,0,1c71d303,3833584698"
			// 0: time: float, second
			// 1: mode: 1 - slide, 5 - top, 4 - bottom, 7 - special
			// 2: size: 25 - normal, 18 - small
			// 3: color: int(10)
			// 4: timestamp: second
			// 5: 0
			// 6: UID ?: int(16)
			// 7: ID
			var r = {
				time: parseInt(Number(arr[0]) * 1000),
				mode: parseInt(arr[1]),
				size: Number(arr[2]),
				color: int_to_color(arr[3]),
			};
			return r;
		};

		var push = Array.isArray(container) ? "push" : "append";
		if(json["tag"] !== "i")
			return -1;
		var list = json["children"];
		var c = 0;
		for(var i in list)
		{
			if(limit && c >= limit)
				break;

			var e = list[i];
			if(e["tag"] !== "d")
				continue;
			var item = parse_p(e.params["p"]);
			item.content = e.children;
			container[push](item);
			c++;
		}
		return 0;
	},
};

var idAPI = idWebAPI;
