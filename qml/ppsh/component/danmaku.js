// This properties is for Danmaku.qml
// For some reason of QML, array must use js-array, and not QVariantList.

var st_danmaku = [];
var Modes = [1, 5, 4];

var iIndex = 0;
var __current = 0;
var __currentY_slide = 0; // y offset: mode == 1
var __currentY_top = 0; // y offset: mode == 5
var __currentY_bottom = 0; // y offset: mode == 4
var __current_slide = 0;
var __current_top = 0;
var __current_bottom = 0;

var __lastTimestamp = 0;
