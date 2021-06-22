#pragma once

//ゲーム全体のヘッダーファイル

//ヘッダーファイルの読み込み
#include "DxLib.h"		//DxLibを使うときに必要
//#include "keyboard.h"	//キーボードの処理
//#include "FPS.h"		//FPSの処理
#include<time.h>

#define GAME_TITLE			"ゲームタイトル"
#define GAME_WIDTH			1280	//ゲーム画面の幅
#define GAME_HEIGHT			720		//ゲーム画面の高さ
#define GAME_COLOR			32		//ゲームの色域
#define GAME_ICON_ID		333		//ゲームのアイコンのID
#define GAME_WINDOW_BAR		0		//ウィンドウバーの種類

#define GAME_DEBUG			TRUE	//デバッグモード

//列挙型
enum GAME_SCENE
{
	GAME_SCENE_TITLE,
	GAME_SCENE_PLAY,
	GAME_SCENE_END,
	GAME_SCENE_CHANGE,
	GAME_SCENE_OVER
};	//ゲームのシーン
