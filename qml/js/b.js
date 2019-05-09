.pragma library

var idBangumi = {
	"filter": {
		name: "过滤",
		value: "filter",
		options: [
		{
			name: "类型",
			value: "season_version",
			options: [
			{
				name: "全部",
				value: "-1",
			},
			{
				name: "正片",
				value: "1",
			},
				{
					name: "剧场版",
					value: "2",
				},
				{
					name: "其他",
					value: "3",
				},
			]
		},
		{
			name: "地区",
			value: "area",
			options: [
			{
				name: "全部",
				value: "-1",
			},
			{
				name: "日本",
				value: "2",
			},
				{
					name: "美国",
					value: "3",
				},
				{
					name: "其他",
					value: "1,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52",
				},
			]
		},
		{
			name: "状态",
			value: "is_finish",
			options: [
			{
				name: "全部",
				value: "-1",
			},
			{
				name: "完结",
				value: "1",
			},
				{
					name: "连载",
					value: "0",
				},
			]
		},
			{
				name: "付费",
				value: "season_status",
				options: [
				{
					name: "全部",
					value: "-1",
				},
				{
					name: "付费",
					value: "2,6",
				},
					{
						name: "大会员",
						value: "4",
					},
					{
						name: "免费",
						value: "1",
					},
				]
			},
			{
				name: "季度",
				value: "season_month",
				options: [
				{
					name: "全部",
					value: "-1",
				},
				{
					name: "1月",
					value: "1",
				},
					{
						name: "4月",
						value: "4",
					},
					{
						name: "7月",
						value: "7",
					},
						{
							name: "10月",
							value: "10",
						},
						]
			},
				{
					name: "时间",
					value: "pub_date",
					options: [
					{
						name: "全部",
						value: "-1",
					},
					{
						name: "2019",
						value: "2019",
					},
						{
							name: "2018",
							value: "2018",
						},
						{
							name: "2017",
							value: "2017",
						},
							{
								name: "2016",
								value: "2016",
							},
							{
								name: "2015",
								value: "2015",
							},
								{
									name: "2014-2010",
									value: "2014-2010",
								},
								{
									name: "2009-2005",
									value: "2009-2005",
								},
									{
										name: "2004-2000",
										value: "2004-2000",
									},
									{
										name: "90年代",
										value: "90年代",
									},
										{
											name: "80年代",
											value: "80年代",
										},
										{
											name: "更早",
											value: "更早",
										},
											]
				},
				{
					name: "风格",
					value: "style_id",
					options: [
					{
						name: "全部",
						value: "-1",
					},
					{
						name: "原创",
						value: "137",
					},
						{
							name: "漫画改",
							value: "135",
						},
						{
							name: "小说改",
							value: "117",
						},
							{
								name: "游戏改",
								value: "136",
							},
							{
								name: "布袋戏",
								value: "150",
							},
								{
									name: "热血",
									value: "20",
								},
								{
									name: "奇幻",
									value: "57",
								},
									{
										name: "战斗",
										value: "95",
									},
									{
										name: "搞笑",
										value: "70",
									},
										{
											name: "日常",
											value: "16",
										},
										{
											name: "科幻",
											value: "71",
										},
											{
												name: "萌系",
												value: "81",
											},
											{
												name: "治愈",
												value: "21",
											},
												{
													name: "校园",
													value: "93",
												},
												{
													name: "少儿",
													value: "24",
												},
													{
														name: "泡面",
														value: "44",
													},
													{
														name: "恋爱",
														value: "110",
													},
														{
															name: "后宫",
															value: "5",
														},
														{
															name: "猎奇",
															value: "87",
														},
															{
																name: "少女",
																value: "9",
															},
															{
																name: "魔法",
																value: "122",
															},
																{
																	name: "历史",
																	value: "67",
																},
																{
																	name: "机战",
																	value: "105",
																},
																	{
																		name: "致郁",
																		value: "22",
																	},
																	{
																		name: "神魔",
																		value: "139",
																	},
																		{
																			name: "声控",
																			value: "98",
																		},
																		{
																			name: "运动",
																			value: "23",
																		},
																			{
																				name: "励志",
																				value: "138",
																			},
																			{
																				name: "音乐",
																				value: "72",
																			},
																				{
																					name: "推理",
																					value: "124",
																				},
																				{
																					name: "社团",
																					value: "127",
																				},
																					{
																						name: "智斗",
																						value: "103",
																					},
																					{
																						name: "催泪",
																						value: "104",
																					},
																						{
																							name: "美食",
																							value: "106",
																						},
																						{
																							name: "装逼",
																							value: "94",
																						},
																							{
																								name: "偶像",
																								value: "121",
																							},
																							{
																								name: "乙女",
																								value: "115",
																							},
																								{
																									name: "职场",
																									value: "140",
																								},
																								]
				}
		]
	},
	"sort": {
		name: "顺序",
		value: "sort",
		options: [
		{
			name: "降序",
			value: "0",
		},
		{
			name: "升序",
			value: "1",
		}
		]
	},
	"order": {
		name: "排序",
		value: "order",
		options: [
		{
			name: "追番人数",
			value: "3",
		},
		{
			name: "更新时间",
			value: "0",
		},
			{
				name: "最高评分",
				value: "4",
			},
			{
				name: "播放数量",
				value: "2",
			},
				{
					name: "开播时间",
					value: "5",
				}
		]
	}
};

var BCategorys = [
{
	name: "动画",
	rid: "1",
	pid: "0",
	children: [
	{
		name: "MAD·AMV",
		rid: "24",
		pid: "1",
	},
	{
		name: "MMD·3D",
		rid: "25",
		pid: "1",
	},
		{
			name: "短片·手书·配音",
			rid: "47",
			pid: "1",
		},
		{
			name: "综合",
			rid: "27",
			pid: "1",
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
		pid: "13",
	},
	{
		name: "完结动画",
		rid: "32",
		pid: "13",
	},
		{
			name: "资讯",
			rid: "51",
			pid: "13",
		},
		{
			name: "官方延伸",
			rid: "152",
			pid: "13",
		},
			{
				name: "新番时间表",
				rid: "",
				pid: "13",
			},
			{
				name: "番剧索引",
				rid: "",
				pid: "13",
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
		pid: "167",
	},
	{
		name: "国产原创相关",
		rid: "168",
		pid: "167",
	},
		{
			name: "布袋戏",
			rid: "169",
			pid: "167",
		},
		{
			name: "动态漫·广播剧",
			rid: "195",
			pid: "167",
		},
			{
				name: "资讯",
				rid: "170",
				pid: "167",
			},
			{
				name: "新番时间表",
				rid: "",
				pid: "167",
			},
				{
					name: "国产动画索引",
					rid: "",
					pid: "167",
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
		pid: "3",
	},
	{
		name: "翻唱",
		rid: "31",
		pid: "3",
	},
		{
			name: "VOCALOID·UTAU",
			rid: "30",
			pid: "3",
		},
		{
			name: "电音",
			rid: "194",
			pid: "3",
		},
			{
				name: "演奏",
				rid: "59",
				pid: "3",
			},
			{
				name: "MV",
				rid: "193",
				pid: "3",
			},
				{
					name: "音乐现场",
					rid: "29",
					pid: "3",
				},
				{
					name: "音乐综合",
					rid: "130",
					pid: "3",
				},
					{
						name: "音频",
						rid: "",
						pid: "3",
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
		pid: "129",
	},
	{
		name: "三次元舞蹈",
		rid: "154",
		pid: "129",
	},
		{
			name: "舞蹈教程",
			rid: "156",
			pid: "129",
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
		pid: "4",
	},
	{
		name: "电子竞技",
		rid: "171",
		pid: "4",
	},
		{
			name: "手机游戏",
			rid: "172",
			pid: "4",
		},
		{
			name: "网络游戏",
			rid: "65",
			pid: "4",
		},
			{
				name: "桌游棋牌",
				rid: "173",
				pid: "4",
			},
			{
				name: "GMV",
				rid: "121",
				pid: "4",
			},
				{
					name: "音游",
					rid: "136",
					pid: "4",
				},
				{
					name: "Mugen",
					rid: "19",
					pid: "4",
				},
					{
						name: "游戏赛事",
						rid: "",
						pid: "4",
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
		pid: "36",
	},
	{
		name: "野生技术协会",
		rid: "122",
		pid: "36",
	},
		{
			name: "演讲公开课",
			rid: "39",
			pid: "36",
		},
		{
			name: "星海",
			rid: "96",
			pid: "36",
		},
			{
				name: "机械",
				rid: "98",
				pid: "36",
			},
			{
				name: "汽车",
				rid: "176",
				pid: "36",
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
		pid: "188",
	},
	{
		name: "电脑装机",
		rid: "189",
		pid: "188",
	},
		{
			name: "摄影摄像",
			rid: "190",
			pid: "188",
		},
		{
			name: "影音智能",
			rid: "191",
			pid: "188",
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
		pid: "160",
	},
	{
		name: "日常",
		rid: "21",
		pid: "160",
	},
		{
			name: "美食圈",
			rid: "76",
			pid: "160",
		},
		{
			name: "动物圈",
			rid: "75",
			pid: "160",
		},
			{
				name: "手工",
				rid: "161",
				pid: "160",
			},
			{
				name: "绘画",
				rid: "162",
				pid: "160",
			},
				{
					name: "运动",
					rid: "163",
					pid: "160",
				},
				{
					name: "其他",
					rid: "174",
					pid: "160",
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
		pid: "119",
	},
	{
		name: "音MAD",
		rid: "26",
		pid: "119",
	},
		{
			name: "人力VOCALOID",
			rid: "126",
			pid: "119",
		},
		{
			name: "教程演示",
			rid: "127",
			pid: "119",
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
		pid: "155",
	},
	{
		name: "服饰",
		rid: "158",
		pid: "155",
	},
		{
			name: "健身",
			rid: "164",
			pid: "155",
		},
		{
			name: "T台",
			rid: "159",
			pid: "155",
		},
			{
				name: "风尚标",
				rid: "192",
				pid: "155",
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
		pid: "5",
	},
	{
		name: "明星",
		rid: "137",
		pid: "5",
	},
		{
			name: "Korea相关",
			rid: "131",
			pid: "5",
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
		pid: "181",
	},
	{
		name: "影视剪辑",
		rid: "183",
		pid: "181",
	},
		{
			name: "短片",
			rid: "85",
			pid: "181",
		},
		{
			name: "预告·资讯",
			rid: "184",
			pid: "181",
		},
			{
				name: "特摄",
				rid: "86",
				pid: "181",
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
		pid: "177",
	},
	{
		name: "科学·探索·自然",
		rid: "178",
		pid: "177",
	},
		{
			name: "军事",
			rid: "179",
			pid: "177",
		},
		{
			name: "社会·美食·旅行",
			rid: "180",
			pid: "177",
		},
			{
				name: "纪录片索引",
				rid: "",
				pid: "177",
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
		pid: "23",
	},
	{
		name: "欧美电影",
		rid: "145",
		pid: "23",
	},
		{
			name: "日本电影",
			rid: "146",
			pid: "23",
		},
		{
			name: "其他国家",
			rid: "83",
			pid: "23",
		},
			{
				name: "电影索引",
				rid: "",
				pid: "23",
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
		pid: "11",
	},
	{
		name: "海外剧",
		rid: "187",
		pid: "11",
	},
		{
			name: "电视剧索引",
			rid: "",
			pid: "11",
		},
	],
},
	];

var idCategory = {
	name: "Home",
	rid: "0",
	pid: "-1",
	children: BCategorys,
};

var idSearch = [
	{
		name: qsTr("Video"),
		value: "video",
		order: [
			{
				name: qsTr("Relation"),
				value: "",
			},
			{
				name: qsTr("Click"),
				value: "click",
			},
			{
				name: qsTr("Publish"),
				value: "pubdate",
			},
			{
				name: qsTr("Danmaku"),
				value: "dm",
			},
			{
				name: qsTr("Stow"),
				value: "stow",
			},
		],
	},
	{
		name: qsTr("Bangumi"),
		value: "media_bangumi",
		order: [
			{
				name: qsTr("Default"),
				value: "",
			},
		],
	},
	{
		name: qsTr("User"),
		value: "bili_user",
		order: [
			{
				name: qsTr("Default"),
				value: "",
			},
			{
				name: qsTr("Fans"),
				value: "fans",
			},
			{
				name: qsTr("Level"),
				value: "level",
			},
		],
	},
	{
		name: qsTr("Article"),
		value: "article",
		order: [
			{
				name: qsTr("Default"),
				value: "",
			},
			{
				name: qsTr("Publish"),
				value: "pubdate",
			},
			{
				name: qsTr("View"),
				value: "click",
			},
			{
				name: qsTr("Like"),
				value: "attention",
			},
			{
				name: qsTr("Comment"),
				value: "scores",
			},
		],
	},
	{
		name: qsTr("Live"),
		value: "live",
		order: [
			{
				name: qsTr("Default"),
				value: "",
			},
		],
	},
	{
		name: qsTr("Live user"),
		value: "live_user",
		order: [
			{
				name: qsTr("Default"),
				value: "",
			},
		],
	},
	{
		name: qsTr("Movie"),
		value: "media_ft",
		order: [
			{
				name: qsTr("Default"),
				value: "",
			},
		],
	},
	{
		name: qsTr("Topic"),
		value: "topic",
		order: [
			{
				name: qsTr("Default"),
				value: "",
			},
		],
	},
	{
		name: qsTr("Photo"),
		value: "photo",
		order: [
			{
				name: qsTr("Default"),
				value: "",
			},
			{
				name: qsTr("Stow"),
				value: "stow",
			},
			{
				name: qsTr("Publish"),
				value: "pubdate",
			},
		],
	},
	];

var idUserArchive = {
	"1": [
	{
		name: qsTr("Update"),
		value: "update",
	},
	{
		name: qsTr("Click"),
		value: "click",
	},
		{
			name: qsTr("Stow"),
			value: "stow",
		},
	],
	"2": [
	{
		name: qsTr("Publish"),
		value: "publish_time",
	},
	{
		name: qsTr("View"),
		value: "view",
	},
		{
			name: qsTr("Stow"),
			value: "fav",
		},
	],
};
