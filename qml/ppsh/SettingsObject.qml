import QtQuick 1.1

QtObject {
	id: root;

	objectName: "idSettingsObject";
	property int iOrientation: _UT.GetSetting("generals/orientation");
	onIOrientationChanged: _UT.SetSetting("generals/orientation", iOrientation);

	property int iThemeColor: _UT.GetSetting("generals/theme_color");
	onIThemeColorChanged: _UT.SetSetting("generals/theme_color", iThemeColor);

	property bool bNightMode: _UT.GetSetting("generals/night_mode");
	onBNightModeChanged: _UT.SetSetting("generals/night_mode", bNightMode);

	property int iDefaultBrowser: _UT.GetSetting("generals/default_browser");
	onIDefaultBrowserChanged: _UT.SetSetting("generals/default_browser", iDefaultBrowser);

	property bool bTouchIconDrag: _UT.GetSetting("generals/touch_icon_drag");
	onBTouchIconDragChanged: _UT.SetSetting("generals/touch_icon_drag", bTouchIconDrag);

	property bool bFullscreen: _UT.GetSetting("generals/fullscreen");
	onBFullscreenChanged: _UT.SetSetting("generals/fullscreen", bFullscreen);



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

		iOrientation = _UT.GetSetting("generals/orientation");
		iThemeColor = _UT.GetSetting("generals/theme_color");
		bNightMode = _UT.GetSetting("generals/night_mode");
		iDefaultBrowser = _UT.GetSetting("generals/default_browser");
		bTouchIconDrag = _UT.GetSetting("generals/touch_icon_drag");
		bFullscreen = _UT.GetSetting("generals/fullscreen");

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
