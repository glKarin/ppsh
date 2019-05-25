.pragma library

Qt.include("b.js");
Qt.include("util.js");

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
	COMMENT: "http://api.bilibili.com/x/v2/reply", //?type=1&sort=2&oid=49156714&pn=1&nohot=1&ps
	COMMENT_REPLY: "http://api.bilibili.com/x/v2/reply/reply", //?type=1&ps=10&oid=49156714&pn=1&josnp=jsonp&root=rpid
	VIDEO_DETAIL: "http://api.bilibili.com/x/web-interface/view", //?aid=37606630
	RANKING: "http://api.bilibili.com/x/web-interface/ranking", //?rid=0&day=3&jsonp=jsonp
	PLAYURL: "http://api.bilibili.com/x/player/playurl?avid=%1&cid=%2&qn=%3&type=&otype=json&fnver=0&fnval=16&ep_id=%4",
	BANGUMI_PLAYURL: "http://api.bilibili.com/pgc/player/web/playurl?avid=%1&cid=%2&qn=%3&type=&otype=json&fnver=0&fnval=16",
	DANMAKU_XML: "http://comment.bilibili.com/%1.xml", // http://api.bilibili.com/x/v1/dm/list.so?oid={cid}
	DEFAULT_KEYWORD: "http://api.bilibili.com/x/web-interface/search/default",
	CHANNELS_IDS: "http://api.bilibili.com/x/web-interface/online", //?jsonp=jsonp
	CATEGORY_NEWLIST: "http://api.bilibili.com/x/web-interface/newlist", //?rid=20&type=0&pn=1&ps=20&jsonp=jsonp
	CATEGORY_RANKING: "http://api.bilibili.com/x/web-interface/ranking/region", //?jsonp=jsonp&rid=20&day=7&original=0
	CATEGORY_DYNAMIC: "http://api.bilibili.com/x/web-interface/dynamic/region", //?&jsonp=jsonp&pn=1&ps=5&rid=20
	CATEGORY: "http://api.bilibili.com/archive_rank/getarchiverankbypartion", //?jsonp=jsonp&tid=27&pn=2 &ps=?
	USER_DETAIL: "http://api.bilibili.com/x/web-interface/card", //?mid=2989565&jsonp=jsonp,
	USER_VIDEOS: "http://space.bilibili.com/ajax/member/getSubmitVideos", //?mid=3487048&pagesize=30&tid=0&page=2&keyword=&order=update|click|stow
	USER_ARTICLES: "http://api.bilibili.com/x/space/article", //?mid=12345667&pn=1&ps=12&sort=publish_time|view|fav&jsonp=jsonp
	LIVE_ROOM_DETAIL: "http://api.live.bilibili.com/room/v1/Room/get_info", //?room_id=%1",
	LIVE_URL: "http://api.live.bilibili.com/room/v1/Room/playUrl", //?cid=%1&quality=0&platform=web",
	LIVE_CHANNELS: "http://api.live.bilibili.com/room/v1/Area/getList",
	LIVE_ROOMS: "http://api.live.bilibili.com/room/v3/area/getRoomList", //?platform=web&parent_area_id=pid&cate_id=0&area_id=aid&sort_type=online|live_time&page=3&page_size=30&tag_version=1
	LIVE_USER: "https://api.live.bilibili.com/live_user/v1/UserInfo/get_anchor_in_room", //?roomid=946585
	LIVE_RECOMMEND: "http://api.live.bilibili.com/room/v1/room/get_recommend_by_room", //?room_id=946585&count=8
	CHANNELS: "http://app.bilibili.com/x/region/list/old?build=2310&platform=ios&device=phone",
	BANGUMI_DETAIL: "http://bangumi.bilibili.com/view/web_api/season", //?season_id=425",
	BANGUMI_RECOMMEND: "http://api.bilibili.com/pgc/web/recommend/related/recommend", //?season_id=425
	BANGUMI: "http://bangumi.bilibili.com/media/web_api/search/result", //?season_version=类型-1|1正片|2剧场版|3其他&area=地区-1|2日本|3美国|1,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52其他&is_finish=状态-1|1完结|0连载&copyright=-1&season_status=付费-1|2,6付费|4大会员|1免费&season_month=季度-1|1|4|7|10月&pub_date=时间-1|2019|2018|2017|2016|2015|2014-2010|2009-2005|2004-2000|90年代|80年代|更早&style_id=风格-1&order=3(追番人数)|0更新时间|4最高评分|2播放数量|5开播时间&st=1&sort=0降|1升&page=1&season_type=1&pagesize=20"



		M_ARTICLE_URL: "http://www.bilibili.com/read/mobile/%1",


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

	MakeSearchResult: function(json, container, type, limit){
		if(!json.data)
			return -1;
		var result = json.data.result;
		if(type === "live")
			result = result.live_room;
		if(!Array.isArray(result))
			return -1;
		var push = Array.isArray(container) ? "push" : "append";
		var list = result;
		for(var i in list)
		{
			if(limit && i >= limit)
				break;

			var e = list[i];
			var item = type === "bili_user" ? 
			{
				mid: e.mid.toString(),
				title: e.uname,
				subtitle: e.usign,
				sign: e.level,
				preview: this.__MakePreviewPath(e.upic),
				type: 5,
				extras: [
				{
					value: e.fans,
					unit: qsTr("fans"),
				},
				{
					value: e.videos,
					unit: qsTr("archives"),
				},
				],
			}
			:
				(type === "media_bangumi" ? 
				 {
					 mid: (e.media_id || e.season_id).toString(),
					 title: e.title,
					 subtitle: e.areas + (e.styles ? " | " + e.styles : ""),
					 sign: e.angle_title || "",
					 preview: this.__MakePreviewPath(e.cover),
					 type: 3,
					 extras: [
					 {
						 value: FormatDateTime(e.pubtime, "DATE"),
					 },
					 {
						 value: e.media_score ? e.media_score.score : "",
						 unit: qsTr("score"),
					 },
					 ],
				 }
				 :
				 (type === "article" ? 
					{
						mid: e.id.toString(),
						title: e.title,
						subtitle: FormatDateTime(e.pub_time, "DATE"), //e.desc,
						sign: "",
						preview: e.image_urls.length ? this.__MakePreviewPath(e.image_urls[0]) : "",
						type: 2,
						extras: [
						{
							name: qsTr("View"),
							value: e.view,
						},
						{
							name: qsTr("Like"),
							value: e.like,
						},
							{
								name: qsTr("Reply"),
								value: e.reply,
							},
						],
					}
			:
				(type === "live" ? 
				 {
						mid: e.roomid.toString(),
						title: e.title,
						subtitle: e.uname,
						sign: e.live_status == 1 ? qsTr("Living") : qsTr("Unlive"),
						preview: this.__MakePreviewPath(e.cover),
						type: 9,
						extras: [
						{
							name: qsTr("Online"),
							value: e.online,
						},
						{
							name: qsTr("Category"),
							value: e.cate_name,
						},
						],
				 }
				 :
				 {
					 mid: e.id.toString(),
					 title: e.title,
					 subtitle: e.author,
					 sign: e.duration,
					 preview: this.__MakePreviewPath(e.pic),
					 type: 1,
					 extras: [
					 {
						 name: qsTr("Play"),
						 value: e.play,
					 },
					 {
						 name: qsTr("Danmaku"),
						 value: e.video_review,
					 },
					 ],
				 }
			)));
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
		var f = function(e){
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

				reply: e.replies ? [] : null,
			};
			if(Array.isArray(e.replies))
			{
				for(var si in e.replies)
				{
					var se = e.replies[si];
					item.reply.push(f(se));
				}
			}
			return item;
		};
		var push = Array.isArray(container) ? "push" : "append";
		var list = json.data.replies;
		for(var i in list)
		{
			if(limit && i >= limit)
				break;

			var e = list[i];
			var item = f(e);
			container[push](item);
		}
		return 0;
	},

	MakeContent: function(json, container, limit){
		if(!json.data)
			return -1;
		if(!Array.isArray(json.data.pages))
			return -1;
		var push = Array.isArray(container) ? "push" : "append";
		var list = json.data.pages;
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

	MakeFullChannels: function(json, container){
		if(!json.result)
			return -1;
		if(!Array.isArray(json.result.root))
			return -1;
		if(!json.result.child)
			return -1;
		var push = Array.isArray(container) ? "push" : "append";
		var list = json.result.root;
		var child = json.result.child;
		var covers = json.result.covers;
		for(var i in list)
		{
			var e = list[i];
			var item = {
				name: e.typename || e.captionname || e.searchname,
				rid: e.tid.toString(),
				children: [],
				pid: "0",
			};
			var c = child[e.tid.toString()];
			if(c)
			{
				for(var j in c)
				{
					var se = c[j];
					var subitem = {
						name: se.typename,
						rid: se.tid.toString(),
						pid: e.tid.toString,
					};
					item.children.push(subitem);
				}
			}
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
					mid: e.aid.toString(),
					title: e.title,
					subtitle: e.author,
					sign: e.length,
					preview: this.__MakePreviewPath(e.pic),
					type: 1,
					extras: [
					{
						name: qsTr("Play"),
						value: e.play,
					},
					{
						name: qsTr("Danmaku"),
						value: e.video_review,
					},
					],
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

	MakeUserArticles: function(json, container, limit){
		if(!json.data)
			return -1;
		if(!Array.isArray(json.data.articles))
			return -1;
		{
			var push = Array.isArray(container) ? "push" : "append";
			var list = json.data.articles;
			for(var i in list)
			{
				if(limit && i >= limit)
					break;

				var e = list[i];
				var item = {
					mid: e.id.toString(),
					title: e.title,
					subtitle: e.author ? e.author.name : "",
					preview: e.image_urls.length ? e.image_urls[0] : "",
					type: 2,
					extras: [
					{
						name: qsTr("View"),
						value: e.stats ? e.stats.view : 0,
					},
					{
						name: qsTr("Reply"),
						value: e.stats ? e.stats.reply : 0,
					},
					],
				};
				container[push](item);
			}
		}

		return 0;
	},

	MakeBangumiUrl: function(json, container, quality){
		if(!json.result)
			return -1;

		return this.MakePlayUrl(json.result, container, quality);
	},

	MakeBangumi: function(json, container, limit){
		if(!json.result)
			return -1;
		if(!Array.isArray(json.result.data))
			return -1;
		var push = Array.isArray(container) ? "push" : "append";
		var list = json.result.data;
		for(var i in list)
		{
			if(limit && i >= limit)
				break;

			var e = list[i];
			var item = {
				sid: e.season_id.toString(),
				title: e.title,
				follow: e.order ? e.order.follow : 0,
				view_count: e.order ? e.order.play : 0,
				score: e.order ? e.order.score : 0,
				index_show: e.index_show || "",
				preview: e.cover,
				badge: e.badge || "",
			};
			container[push](item);
		}
		return 0;
	},

	MakeBangumiInfo: function(json, ret){
		if(!json.result)
			return -1;

		var r = ret ? ret : {};
		var d = json.result;

		r.mid = d.media_id.toString();
		r.preview = d.cover;
		r.title = d.title;
		r.desc = d.evaluate;
		r.newest_ep = d.newest_ep ? d.newest_ep.desc : "";

		r.rating_count = d.rating ? d.rating.count : 0;
		r.rating_score = d.rating ? d.rating.score : 0;

		r.danmu_count = d.stat ? d.stat.danmakus : 0;
		r.reply = d.stat ? d.stat.reply : 0;
		r.view_count = d.stat ? d.stat.views : 0;
		r.coin = d.stat ? d.stat.coins : 0;
		r.share = d.stat ? d.stat.share : 0;
		r.favorite = d.stat ? d.stat.favorites : 0;

		r.up = d.up_info ? d.up_info.uname : "";
		r.uid = d.up_info ? d.up_info.mid.toString() : "";
		r.avatar = d.up_info ? d.up_info.avatar : "";

		r.seasons = Array.isArray(d.seasons) ? d.seasons.length : 0;
		r.episodes = Array.isArray(d.episodes) ? d.episodes.length : 0;
		r.first_aid = r.episodes > 0 ? d.episodes[0].aid.toString() : "";

		return ret ? 0 : r;
	},

	MakeBangumiEpisode: function(json, container, limit){
		if(!json.result)
			return -1;
		if(!Array.isArray(json.result.episodes))
			return -1;
		var push = Array.isArray(container) ? "push" : "append";
		var list = json.result.episodes;
		for(var i in list)
		{
			if(limit && i >= limit)
				break;

			var e = list[i];
			var item = {
				aid: e.aid.toString(),
				cid: e.cid.toString(),
				page: e.index,
				duration: e.duration,
				badge: e.badge || "",
				name: e.index_title,
				epid: e.ep_id.toString(),
			};
			container[push](item);
		}
		return 0;
	},

	MakeBangumiSeasons: function(json, container){
		if(!json.result)
			return -1;
		if(!Array.isArray(json.result.seasons))
			return -1;
		var push = Array.isArray(container) ? "push" : "append";
		var list = json.result.seasons;
		for(var i in list)
		{
			var e = list[i];
			var item = {
				name: e.season_title,
				value: (e.season_id || e.media_id).toString(),
			};
			container[push](item);
		}
		return 0;
	},

	MakeBangumiRecommend: function(json, container, limit){
		if(!Array.isArray(json.result))
			return -1;
		var push = Array.isArray(container) ? "push" : "append";
		var list = json.result;
		for(var i in list)
		{
			if(limit && i >= limit)
				break;

			var e = list[i];
			var item = {
				sid: e.season_id.toString(),
				title: e.title,
				follow: e.stat ? e.stat.follow : 0,
				view_count: e.stat ? e.stat.view : 0,
				danmu_count: e.stat ? e.stat.danmaku : 0,
				index_show: e.new_ep ? e.new_ep.index_show : "",
				preview: e.cover,
				badge: e.badge || "",
				rating_score: e.rating ? e.rating.score : 0,
				rating_count: e.rating ? e.rating.count : 0,
			};
			container[push](item);
		}
		return 0;
	},

	MakeVideoUrl: function(json, container, quality){
		if(!json.data)
			return -1;

		return this.MakePlayUrl(json.data, container, quality);
	},

	MakePlayUrl: function(data, container, quality){
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

	MakeLive: function(json, container, limit){
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
				rid: e.roomid.toString(),
				title: e.title,
				uname: e.uname,
				online: e.online,
				preview: e.system_cover,
				area: e.parent_name + "/" + e.area_name,
			};
			container[push](item);
		}
		return 0;
	},

	MakeLiveChannels: function(json, container){
		if(!Array.isArray(json.data))
			return -1;
		var push = Array.isArray(container) ? "push" : "append";
		var list = json.data;
		for(var i in list)
		{
			var e = list[i];
			var item = {
				name: e.name,
				rid: e.id.toString(),
				children: [],
				pid: "0",
			};
			for(var si in e.list)
			{
				var se = e.list[si];
				var subitem = {
					name: se.name,
					rid: se.id.toString(),
					pid: se.parent_id.toString(),
					//old_area_id: se.old_area_id.toString(),
					//pname: se.parent_name,
				};
				item.children.push(subitem);
			}
			container[push](item);
		}
		return 0;
	},

	MakeLiveRecommend: function(json, container, limit){
		if(!json.data)
			return -1;
		if(!Array.isArray(json.data.recommend))
			return -1;
		var push = Array.isArray(container) ? "push" : "append";
		var list = json.data.recommend;
		for(var i in list)
		{
			if(limit && i >= limit)
				break;

			var e = list[i];
			var item = {
				rid: e.room_id.toString(),
				title: e.title,
				up: e.uname,
				online: e.online,
				preview: e.pic,
			};
			container[push](item);
		}
		return 0;
	},

	MakeLiveRoomInfo: function(json, ret){
		if(!json.data)
			return -1;

		var r = ret ? ret : {};
		var d = json.data;

		r.room_id = d.room_id.toString();
		r.is_portrait = d.is_portrait;
		r.live_status = d.live_status;
		r.bg = d.background;
		r.preview = d.keyframe;
		r.title = d.title;
		r.desc = d.description;
		r.live_time = d.live_time;
		r.online = d.online;
		r.follower = d.attention;
		r.area = d.parent_area_name + "/" + d.area_name;

		r.uid = d.uid.toString();
		r.avatar = d.user_cover;
		r.up = "";
		r.official = d.new_pendants ? (d.new_pendants.badge ? d.new_pendants.badge.desc : "") : "";

		return ret ? 0 : r;
	},

	MakeLiveUserInfo: function(json, ret){
		if(!json.data)
			return -1;
		if(!json.data.info)
			return -1;

		var r = ret ? ret : {};
		var d = json.data.info;

		r.uid = d.uid.toString();
		r.avatar = d.face;
		r.up = d.uname;
		r.sex = d.gender;
		r.official = d.official_verify ? d.official_verify.desc : "";
		r.vip = d.vip_type;
		r.level = 0;
		if(json.data.level)
		{
			var d2 = json.data.level;
			r.level = d2.master_level ? d2.master_level.level : 0;
		}

		return ret ? 0 : r;
	},

	MakeLiveQualitys: function(json, container, roomid, nosort){
		if(!json.data)
			return -1;

		var push = Array.isArray(container) ? "push" : "append";
		if(!Array.isArray(json.data.accept_quality))
			return -1;
		var qs= json.data.accept_quality;
		var desc = json.data.quality_description;

		var list = [];
		for(var i in qs)
			list.push(qs[i]);
		if(!nosort)
			list.sort(function(a, b){
				return Number(a) - Number(b);
			});

		for(var i in list)
		{
			var e = list[i];
			var item = {
				name: qsTr("Unknow quality"),
				value: e.toString(),
				cid: e.toString(),
			};
			if(roomid !== undefined)
				item.aid = roomid.toString();
			for(var di in desc)
			{
				if(desc[di].qn == e)
				{
					item["name"] = desc[di].desc;
					break;
				}
			}
			container[push](item);
		}
		return 0;
	},

	MakeLiveStreams: function(json, container){
		if(!json.data)
			return -1;
		if(!Array.isArray(json.data.durl))
			return -1;

		var push = Array.isArray(container) ? "push" : "append";
		var list = json.data.durl;
		var desc = json.data.quality_description;
		var title = "quality";
		var q = json.data.current_qn;
		if(Array.isArray(desc))
		{
			for(var i in desc)
			{
				if(desc[i].qn == q)
				{
					title = desc[i].desc;
					break;
				}
			}
		}
		for(var i in list)
		{
			var e = list[i];
			var item = {
				name: title + (e.url ? "" : "*"), 
				page: Number(i) + 1,
				url: e.url,
				quality: q,
			};
			container[push](item);
		}

		return 0;
	},

	MakeLiveUrl: function(json, container, quality){
		if(!json.data)
			return -1;

		var push = Array.isArray(container) ? "push" : "append";
		var desc = json.data.quality_description;
		var title = "quality";
		var q = json.data.current_qn;
		if(Array.isArray(desc))
		{
			for(var i in desc)
			{
				if(desc[i].qn == q)
				{
					title = desc[i].desc;
					break;
				}
			}
		}
		var arr = [];
		if (Array.isArray(json.data.durl))
		{
			var durl = json.data.durl;
			for(var i in durl)
			{
				var e = durl[i];
				arr.push({
					title: title + "[" + i + "]", 
					name: e.url ? i : "" + i + "*",
					url: e.url,
					value: i,
					duration: e.length,
					size: 0, // 
				});
			}
			var item = {
				name: title,
				index: 0,
				size: 0, //
				duration: 0,
				part: arr,
				type: "durl",
			};
			container[push](item);
		}

		return 0;
	},
};

var idAPI = idWebAPI;
