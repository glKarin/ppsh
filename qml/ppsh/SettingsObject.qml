import QtQuick 1.1

QtObject {
	id: root;

	objectName: "idSettingsObject";
	property int iOrientation: _UT.GetSetting("general/orientation");
	onIOrientationChanged: _UT.SetSetting("general/orientation", iOrientation);

	property int iThemeColor: _UT.GetSetting("general/theme_color");
	onIThemeColorChanged: _UT.SetSetting("general/theme_color", iThemeColor);

	property bool bNightMode: _UT.GetSetting("general/night_mode");
	onBNightModeChanged: _UT.SetSetting("general/night_mode", bNightMode);

	property int iDefaultBrowser: _UT.GetSetting("general/default_browser");
	onIDefaultBrowserChanged: _UT.SetSetting("general/default_browser", iDefaultBrowser);

	property bool bTouchIconDrag: _UT.GetSetting("general/touch_icon_drag");
	onBTouchIconDragChanged: _UT.SetSetting("general/touch_icon_drag", bTouchIconDrag);

	property bool bFullscreen: _UT.GetSetting("general/fullscreen");
	onBFullscreenChanged: _UT.SetSetting("general/fullscreen", bFullscreen);



	property int iVideoQuality: _UT.GetSetting("player/video_quality");
	onIVideoQualityChanged: _UT.SetSetting("player/video_quality", iVideoQuality);

	property bool bOpenDanmaku: _UT.GetSetting("player/open_danmaku");
	onBOpenDanmakuChanged: _UT.SetSetting("player/open_danmaku", bOpenDanmaku);

	property real fDanmakuOpacity: _UT.GetSetting("player/danmaku_opacity");
	onFDanmakuOpacityChanged: _UT.SetSetting("player/danmaku_opacity", fDanmakuOpacity);

	property real fDanmakuSize: _UT.GetSetting("player/danmaku_size");
	onFDanmakuSizeChanged: _UT.SetSetting("player/danmaku_size", fDanmakuSize);

	property real fDanmakuSpeed: _UT.GetSetting("player/danmaku_speed");
	onFDanmakuSpeedChanged: _UT.SetSetting("player/danmaku_speed", fDanmakuSpeed);

	property int iDanmakuLimit: _UT.GetSetting("player/danmaku_limit");
	onIDanmakuLimitChanged: _UT.SetSetting("player/danmaku_limit", iDanmakuLimit);

	property int eDanmakuScreenMode: _UT.GetSetting("player/danmaku_screen_mode");
	onEDanmakuScreenModeChanged: _UT.SetSetting("player/danmaku_screen_mode", eDanmakuScreenMode);




	property bool bBrowserHelper: _UT.GetSetting("browser/helper");
	onBBrowserHelperChanged: _UT.SetSetting("browser/helper", bBrowserHelper);

	property bool bBrowserDblZoom: _UT.GetSetting("browser/dbl_zoom");
	onBBrowserDblZoomChanged: _UT.SetSetting("browser/dbl_zoom", bBrowserDblZoom);

	property bool bBrowserLoadImage: _UT.GetSetting("browser/load_image");
	onBBrowserLoadImageChanged: _UT.SetSetting("browser/load_image", bBrowserLoadImage);


	function _ResetSetting(name)
	{
		_UT.ResetSetting(name);

		iOrientation = _UT.GetSetting("general/orientation");
		iThemeColor = _UT.GetSetting("general/theme_color");
		bNightMode = _UT.GetSetting("general/night_mode");
		iDefaultBrowser = _UT.GetSetting("general/default_browser");
		bTouchIconDrag = _UT.GetSetting("general/touch_icon_drag");
		bFullscreen = _UT.GetSetting("general/fullscreen");

		iVideoQuality = _UT.GetSetting("player/video_quality");
		bOpenDanmaku = _UT.GetSetting("player/open_danmaku");
		fDanmakuOpacity = _UT.GetSetting("player/danmaku_opacity");
		fDanmakuSize = _UT.GetSetting("player/danmaku_size");
		fDanmakuSpeed = _UT.GetSetting("player/danmaku_speed");
		iDanmakuLimit = _UT.GetSetting("player/danmaku_limit");
		eDanmakuScreenMode = _UT.GetSetting("player/danmaku_screen_mode");

		bBrowserHelper = _UT.GetSetting("browser/helper");
		bBrowserDblZoom = _UT.GetSetting("browser/dbl_zoom");
		bBrowserLoadImage = _UT.GetSetting("browser/load_image");
	}
}
