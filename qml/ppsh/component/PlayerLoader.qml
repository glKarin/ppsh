import QtQuick 1.1

Item{
	id: root;
	property alias tools: row.children;
	property alias bToolsVisible: row.visible;
	property alias item: loader.item;
	signal exit;
	signal stop;
	signal menu;

	clip: true;
	objectName: "idPlayerLoaderWidget";

	Loader{
		id: loader;
		anchors.fill: parent;
	}

	Row{
		id: row;
		anchors.top: parent.top;
		anchors.right: parent.right;
		height: constants._iSizeXL;
		spacing: constants._iSpacingMedium;
		visible: loader.item === null;
	}

	function _Load(aid, cids, index)
	{
		if(loader.item === null)
		{
			loader.sourceComponent = Qt.createComponent(Qt.resolvedUrl("VideoPlayer.qml"));
			if (loader.status === Loader.Ready)
			{
				var item = loader.item;
				item.exit.connect(__Exit);
				item.stop.connect(__Stop);
				item.trigger.connect(root.menu);
				controller._OpenPlayer(aid, cids, index, item);
				console.log(qsTr("Load video player successful"));
				return 0;
			}
			else
			{
				console.log(qsTr("Load video player fail"));
				return -1;
			}
		}
		else
		{
			var item = loader.item;
			item._Load(index);
			return 1;
		}
	}

	function __Exit()
	{
		if(loader.item !== null)
		{
			loader.sourceComponent = undefined;
		}
		root.exit();
	}

	function _DeInit()
	{
		if(loader.item !== null)
		{
			var item = loader.item;
			item._DeInit();
			loader.sourceComponent = undefined;
		}
	}

	function __Stop()
	{
		root.stop();
	}

	Component.onDestruction: {
		_DeInit();
	}
}
