import QtQuick 1.1
import "../js/util.js" as Util

QtObject {
	id: root;

	objectName: "idControllerObject";
	property variant __queryDialog: null;

	// page
	function _OpenSettingPage(im)
	{
		if(_IsCurrentPage("Setting")) return;
		var page = Qt.createComponent(Qt.resolvedUrl("SettingPage.qml"));
		pageStack.push(page, undefined, im);
	}

	function _OpenUrl(url, type, im)
	{
		var t = type === undefined ? settings.iDefaultBrowser : type;
		if(t == 0)
		{
			//if(_IsCurrentPage("Browser")) return;
			var page = Qt.createComponent(Qt.resolvedUrl("BrowserPage.qml"));
			var p= pageStack.push(page, undefined, im);
			p._Init(url);
		}
		else Qt.openUrlExternally(url);
	}

	function _OpenUserPage(uid, im)
	{
		if(_IsCurrentPage("User")) return;
		var page = Qt.createComponent(Qt.resolvedUrl("UserPage.qml"));
		var p = pageStack.push(page, undefined, im);
		p._Init(uid);
	}

	function _OpenCategoryPage(im)
	{
		if(_IsCurrentPage("Category")) return;
		var page = Qt.createComponent(Qt.resolvedUrl("CategoryPage.qml"));
		var p = pageStack.push(page, undefined, im);
		p._Init();
	}

	function _OpenHistoryPage(im)
	{
		if(_IsCurrentPage("History")) return;
		var page = Qt.createComponent(Qt.resolvedUrl("HistoryPage.qml"));
		var p = pageStack.push(page, undefined, im);
		//p._Init();
	}

	function _OpenRankingPage(im)
	{
		if(_IsCurrentPage("Ranking")) return;
		var page = Qt.createComponent(Qt.resolvedUrl("RankingPage.qml"));
		var p = pageStack.push(page, undefined, im);
		p._Init();
	}

	function _OpenSearchPage(im)
	{
		if(_IsCurrentPage("Search")) return;
		var page = Qt.createComponent(Qt.resolvedUrl("SearchPage.qml"));
		var p = pageStack.push(page, undefined, im);
		p._Init();
	}

	function _OpenResultPage(kw, im)
	{
		if(_IsCurrentPage("Result")) return;
		var page = Qt.createComponent(Qt.resolvedUrl("ResultPage.qml"));
		var p = pageStack.push(page, undefined, im);
		p._Init(kw);
	}

	function _OpenDetailPage(aid, im)
	{
		var page = Qt.createComponent(Qt.resolvedUrl("DetailPage.qml"));
		var p = pageStack.push(page, undefined, im);
		p._Init(aid);
	}

	function _OpenAboutPage(im)
	{
		if(_IsCurrentPage("About")) return;
		var page = Qt.createComponent(Qt.resolvedUrl("AboutPage.qml"));
		pageStack.push(page, undefined, im);
	}

	function _OpenPlayer(aid, contents, index, player)
	{
		var cids = [];
		Util.ModelForeach(contents, function(e, i){
			cids.push({
				cid: e.cid,
				name: e.name,
			});
		});
		player._Init(aid, cids, index);
	}

	function _OpenPlayerPage(aid)
	{
		if(_IsCurrentPage("Player")) return;
		var page = Qt.createComponent(Qt.resolvedUrl("PlayerPage.qml"));
		var p = pageStack.push(page, undefined, true);
		p._Init(aid);
	}

	// hide
	function __Test(data)
	{
		if(_UT.dev === 0)
		{
			_ShowMessage("Only for developer");
			return;
		}

		_OpenPlayerPage("14266370");
	}

	function _OpenTestRequestPage(im)
	{
		if(_IsCurrentPage("TestRequest")) return;
		var page = Qt.createComponent(Qt.resolvedUrl("test/TestRequestPage.qml"));
		pageStack.push(page, undefined, im);
	}

	// util
	function _ShowMessage(msg)
	{
		console.log(msg);
		infobanner._ShowMessage(msg);
	}

	function _CopyToClipboard(text, name)
	{
		_UT.CopyToClipboard(text);
		_ShowMessage(qsTr("Copy %1 to clipboard successful").arg(name ? name : qsTr("data")));
		console.log("Copy data -> " + text);
	}

	function _Query(title, message, acceptText, rejectText, acceptCallback, rejectCallback)
	{
		if(!__queryDialog)
		{
			__queryDialog = Qt.createComponent("component/DynamicQueryDialog.qml");
		}
		var msg = Array.isArray(message) ? message.join("\n") : message;
		var prop = {
			titleText: title,
			message: msg + "\n",
			acceptButtonText: acceptText,
			rejectButtonText: rejectText
		};
		var diag = __queryDialog.createObject(pageStack.currentPage, prop);
		if (typeof(acceptCallback) === "function") diag.accepted.connect(acceptCallback);
		if (typeof(rejectCallback) === "function") diag.rejected.connect(rejectCallback);
	}

	function _IsCurrentPage(name)
	{
		return(pageStack.currentPage.objectName === "id" + name + "Page");
	}
}
