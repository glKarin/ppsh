import QtQuick 1.1

QtObject {
    id: root;
		objectName: "idConstantsObject";

    property int _iHeaderHeight: 64;
    property int _iHeaderZ: 100;
    property int _iMenuZ: 200;
    property color _cHeaderTitleColor: "#ffffff";

    property int _iMaxZ: 999;
		property int _iMaxWidth: 480;
		property int _iMaxHeight: 800;

    property int _iFontMicro: 10;
    property int _iFontTiny: 12;
    property int _iFontSmall: 16;
    property int _iFontMedium: 18;
    property int _iFontLarge: 20;
    property int _iFontXL: 24;
    property int _iFontXXL: 28;
    property int _iFontXXXL: 32;
    property int _iFontBig: 40;
    property int _iFontTooBig: 48;
    property int _iFontSuper: 64;


		property int _iSpacingMicro: 1;
		property int _iSpacingTiny: 2;
		property int _iSpacingSmall: 4;
		property int _iSpacingMedium: 6;
		property int _iSpacingLarge: 8;
		property int _iSpacingXL: 10;
		property int _iSpacingXXL: 12;
		property int _iSpacingXXXL: 16;
		property int _iSpacingBig: 18;
		property int _iSpacingTooBig: 20;
		property int _iSpacingSuper: 24;


    property int _iSizeMicro: 4;
    property int _iSizeTiny: 16;
    property int _iSizeSmall: 24;
    property int _iSizeMedium: 32;
    property int _iSizeLarge: 48;
    property int _iSizeXL: 64;
    property int _iSizeXXL: 80;
    property int _iSizeXXXL: 128;
    property int _iSizeBig: 180;
    property int _iSizeTooBig: 240;
    property int _iSizeSuper: 320;


		property color _cLightestColor: !_bInverted ? "#ffffff" : "#000000";
		property color _cLighterColor: !_bInverted ? "#cccccc" : "#333333";
		property color _cLightColor: !_bInverted ? "#999999" : "#666666";
		property color _cDarkColor: !_bInverted ? "#666666" : "#999999";
		property color _cDarkerColor: !_bInverted ? "#333333" : "#cccccc";
		property color _cDarkestColor: !_bInverted ? "#000000" : "#ffffff";

		property color _cGlobalColor: _bInverted ? _cNightColor : _cThemeColor;
		property color _cThemeColor: _aThemeColors[settings.iThemeColor].value;
		property bool _bInverted: settings.bNightMode;
		property color _cTransparent: "#00000000";
		property color _cNightColor: "#2d2d2d";

		property variant _aThemeColors: [
			{
				name: "<font color=%1>%2</font>".arg("#fb7299").arg(qsTr("Girl pink(default)")),
				value: "#fb7299",
			},
			{
				name: "<font color=%1>%2</font>".arg("#f44336").arg(qsTr("Soviet red")),
				value: "#f44336",
			},
			{
				name: "<font color=%1>%2</font>".arg("#ffc107").arg(qsTr("Orange yellow")),
				value: "#ffc107",
			},
			{
				name: "<font color=%1>%2</font>".arg("#8bc34a").arg(qsTr("Sea green")),
				value: "#8bc34a",
			},
			{
				name: "<font color=%1>%2</font>".arg("#2196f3").arg(qsTr("Sky blue")),
				value: "#2196f3",
			},
			{
				name: "<font color=%1>%2</font>".arg("#9c27b0").arg(qsTr("Gay purple")),
				value: "#9c27b0",
			},
		];

		property variant _aVideoQualitys: [
			/*
			{
				name: qsTr("Automatic"),
				value: 0,
			},
			*/
			{
				name: qsTr("360P"),
				value: 16,
			},
			{
				name: qsTr("480P"),
				value: 32,
			},
			{
				//name: qsTr("720P(mp4)"),
				name: qsTr("720P"),
				value: 48,
			},
			/*
			{
				name: qsTr("720P"),
				value: 64,
			},
			{
				name: qsTr("720P 60"),
				value: 74,
			},
			*/
			{
				name: qsTr("1080P"),
				value: 80,
			},
			/*
			{
				name: qsTr("1080P 60"),
				value: 116,
			},
			*/
		];

		property variant _aRankColors: [
			"#bfbfbf", // 0
			"#bfbfbf", // 1
			"#95ddb2", // 2
			"#92d1e5", // 3
			"#ffb37c", // 4
			"#ff6c00", // 5
			"#ff0000", // 6
		];


		property string _sPrevPage: "_Prev";
		property string _sNextPage: "_Next";
		property string _sThisPage: "_This";
		property string _sFirstPage: "_First";

		property string _sShowState: "_Show";
		property string _sHideState: "_Hide";

		property string _sAlignLeft: "_Left";
		property string _sAlignCenter: "_Center";
		property string _sAlignRight: "_Right";

		property int _iContentsListWidth: 320;
		property int _iSettingBarWidth: 320;

		property int _eVideoType: 1;
		property int _eArticleType: 2;
		property int _eBangumiType: 3;
		property int _eUserType: 5;
		property int _eAdType: 6;
		property int _eKeywordType: 7;
		property int _eChannelType: 8;
		property int _eLiveType: 9;


		function _GetLevelColor(lvl)
		{
			var i = typeof(lvl) !== "number" ? parseInt(lvl) : lvl;
			if(i > 6) i = 6;
			else if(i < 0) i = 0;
			return constants._aRankColors[i];
		}

		function _GetVipName(vip)
		{
			return vip == 2 ? qsTr("Super VIP") : (vip == 1 ? qsTr("VIP") : "");
		}
	}
