import QtQuick 1.1
import com.nokia.meego 1.1

Page {
	id: root;

	property bool bBusy: false;
	property bool bFull: false;
	property string sTitle;

	orientationLock: settings.iOrientation === 1 ? PageOrientation.LockPortrait : (settings.iOrientation === 2 ? PageOrientation.LockLandscape : PageOrientation.Automatic);
	objectName: "idBasePage";

	BusyIndicator{
		id: indicator;
		anchors.centerIn: parent;
		z: constants._iMaxZ;
		running: root.bBusy;
		visible: running;
		platformStyle: BusyIndicatorStyle{
			size: "large";
			inverted: constants._bInverted;
		}
	}

	function _Init()
	{
	}

	function _DeInit()
	{
	}
}
