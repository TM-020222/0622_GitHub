#define _CRT_SECURE_NO_WARNINGS
//ヘッダファイル読み込み
#include"game.h"		//ゲーム全体のヘッダファイル
#include "keyboard.h"	//キーボードの処理
#include "FPS.h"		//FPSの処理

//マクロ定義
#define TAMA_MAX 10		//弾の総数


//動画の構造体
struct MOVIE
{
	int handle = -1;		//動画のハンドル(管理番号)
	char path[255];			//動画の場所(パス)

	int x;					//X位置
	int y;					//Y位置
	int width;				//幅
	int height;				//高さ

	int volume = 255;		//ボリューム 0-255

	//BOOL IsDraw = FALSE;	//動画が描画できる？
};

//音楽の構造体
struct AUDIO
{
	int handle = -1;		//動画のハンドル(管理番号)
	char path[255];			//動画の場所(パス)

	int volume = -1;		//ボリューム 0-255
	int playtype = -1;		//BGM or SE
};

//画像の構造体
struct IMAGE
{
	int handle = -1;		//画像のハンドル(管理番号)
	char path[255];			//画像の場所(パス)
	int x;					//X位置
	int y;					//Y位置
	int width;				//幅
	int height;				//高さ

	BOOL IsDraw = FALSE;	//画像が描画できる？
};

//キャラクタの構造体
typedef struct CHARACTOR
{
	IMAGE img;

	int speed = 1;			//移動速度

	RECT coll;				//当たり判定の領域(四角)
}CHARA;

struct TAMA
{
	int handle[255];	//画像のハンドル
	char path[255];		//画像のパス

	int divX;			//縦の分割数
	int divY;			//横の分割数
	int divMax;			//分割総数

	int AnimationCnt = 0;		//アニメーションカウンタ
	int AnimationCntMax = 0;	//アニメーションカウンタMAX

	int NowIndex = 0;			//現在の画像の要素数

	int x;						//X位置
	int y;						//Y位置
	int width;					//幅
	int height;					//高さ

	int speed;					//速度

	RECT coll;					//当たり判定

	BOOL IsDraw = FALSE;		//描画できるか
};

//グローバル変数
//シーンを管理する変数
GAME_SCENE GameScene;		//現在のゲームのシーン
GAME_SCENE OldGameScene;	//前回のゲームのシーン
GAME_SCENE NextGameScene;	// 次 のゲームのシーン

//画面の切り替え
BOOL IsFadeOut = FALSE;	//フェードアウト
BOOL IsFadeIn  = FALSE;	//フェードイン

//弾の構造体変数
TAMA tama_moto;			//元
TAMA tama[TAMA_MAX];	//実際に使う
TAMA explosion;

//シーン切り替え
int fadeTimeMill = 2000;						//切り替えミリ秒
int fadeTimeMax = fadeTimeMill / 1000 * 60;		//ミリ秒をフレーム秒に変換

//フェードアウト
int fadeOutCntInit = 0;				//初期値
int fadeOutCnt = fadeOutCntInit;	//フェードアウトのカウンタ
int fadeOutCntMax = fadeTimeMax;	//フェードアウトのカウンタMAX

//フェードイン
int fadeInCntInit = fadeTimeMax;	//初期値
int fadeInCnt = fadeInCntInit;		//フェードインのカウンタ
int fadeInCntMax = 0;				//フェードインのカウンタMAX 0?

//弾の画像のハンドル
const int tamaDivMax = 4;

//プロトタイプ宣言
VOID Title(VOID);		//タイトル画面
VOID TitleProc(VOID);	//タイトル画面(処理)
VOID TitleDraw(VOID);	//タイトル画面(描画)

VOID Play(VOID);		//プレイ画面
VOID PlayProc(VOID);	//プレイ画面(処理)
VOID PlayDraw(VOID);	//プレイ画面(描画)

VOID End(VOID);			//エンド画面
VOID EndProc(VOID);		//エンド画面(処理)
VOID EndDraw(VOID);		//エンド画面(描画)

VOID Change(VOID);		//切り替え画面
VOID ChangeProc(VOID);	//切り替え画面(処理)
VOID ChangeDraw(VOID);	//切り替え画面(描画)

VOID Over(VOID);		//ゲームオーバー画面
VOID OverProc(VOID);	//ゲームオーバー画面(処理)
VOID OverDraw(VOID);	//ゲームオーバー画面(描画)

VOID DrawTama(TAMA* tama);	//弾の描画

BOOL GameLoad(VOID);	//ゲームデータのロード

//音楽のロード
BOOL LoadAudio(AUDIO* audio, const char* path, int volume, int playtype);

//画像のロード
BOOL LoadImg(IMAGE* image, const char* path);

//ゲームの画像の分割読み込み
BOOL LoadImageDiv(TAMA* handle, const char* path, int divX, int divY, int DivMax);

VOID GameInit(VOID);	//ゲームの初期化
VOID TitleInit(VOID);	//タイトルの初期化

VOID CollUpdate(CHARA* chara);	//当たり判定の領域を更新
VOID CollUpdateTama(TAMA* tama);	//当たり判定の領域を更新

BOOL CubeCollision(RECT A, RECT B);		//矩形と矩形の当たり判定

VOID ChangeScene(GAME_SCENE scene);	//シーン切り替え

// プログラムは WinMain から始まります
//Windowsのプログラミング方法 = (WinAPI)で動いている
//DxLibはDirectXという、ゲームプログラミングを簡単に使える仕組み
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	SetOutApplicationLogValidFlag(FALSE);
	//Log.txtを出力しない

	ChangeWindowMode(TRUE);			//ウィンドウモードに設定
	SetMainWindowText(GAME_TITLE);	//ウィンドウのタイトル文字
	SetGraphMode(GAME_WIDTH, GAME_HEIGHT, GAME_COLOR);
	//ウィンドウの解像度を設定

	SetWindowSize(GAME_WIDTH, GAME_HEIGHT);
	//ウィンドウの大きさを設定

	SetBackgroundColor(255, 255, 255);
	//デフォルトの背景の色

	SetWindowIconID(GAME_ICON_ID);
	//アイコンファイルを読み込み

	SetWindowStyleMode(GAME_WINDOW_BAR);
	//ウィンドウバーの状態

	SetWaitVSyncFlag(TRUE);
	//ディスプレイの垂直同期を有効にする

	SetAlwaysRunFlag(TRUE);
	//ウィンドウをずっとアクティブにする

	if (DxLib_Init() == -1)		// ＤＸライブラリ初期化処理
	{
		return -1;			// エラーが起きたら直ちに終了
	}

	SetDrawScreen(DX_SCREEN_BACK);

	//最初のシーンは、タイトル画面から
	GameScene = GAME_SCENE_TITLE;

	//ゲーム全体の初期化

	//ゲームの読み込み
	if (!GameLoad())
	{
		//データの読み込みに失敗したとき
		DxLib_End();	//DxLib終了
		return -1;		//強制終了
	}

	//ゲームの初期化
	GameInit();

	//タイトルの初期化
	TitleInit();

	while (1)
	{
		//キーが押されたとき
		/*if (CheckHitKeyAll() != 0) { break; }*/

		//メッセージを受け取り続ける
		if (ProcessMessage() != 0) { break; }

		//画面消去
		if (ClearDrawScreen() != 0) break;
		
		//キーボード入力の更新
		AllKeyUpdate();

		//FPS値の更新
		FPSUpdate();

		//ESCキーで強制終了
		if (KeyClick(KEY_INPUT_ESCAPE) == TRUE) break;

		//以前のシーンを取得
		if (GameScene != GAME_SCENE_CHANGE)
			OldGameScene = GameScene;

		//シーンごとに処理を行う
		switch (GameScene)
		{
		case GAME_SCENE_TITLE:
			Title();			//タイトル画面
			break;
		case GAME_SCENE_PLAY:
			Play();				//プレイ画面
			break;
		case GAME_SCENE_END:
			End();				//エンド画面
			break;
		case GAME_SCENE_CHANGE:
			Change();			//切り替え画面
			break;
		case GAME_SCENE_OVER:	//ゲームオーバー画面
			Over();
			break;
		default:
			break;
		}

		//シーンを切り替える
		if (OldGameScene != GameScene)
		{
			//現在のシーンが切り替え画面でないとき
			if (GameScene != GAME_SCENE_CHANGE)
			{
				NextGameScene = GameScene;		//次のシーンを保存
				GameScene = GAME_SCENE_CHANGE;	//画面切り替えシーンに変える
			}
		}

		//FPSを描画
		FPSDraw();

		//FPS値を待つ
		FPSWait();
		
		ScreenFlip();	//ダブルバッファリングした画面を描画;
	}

	//終わるときの処理

	//読み込んだ画像を開放
	for (int i = 0; i < tamaDivMax; i++) { DeleteGraph(tama_moto.handle[i]); }

	DxLib_End();				// ＤＸライブラリ使用の終了処理

	return 0;				// ソフトの終了 
}

/// <summary>
/// ゲームデータを読み込み
/// </summary>
/// <returns>読み込めたらTRUE,読み込めなかったらFALSE</returns>
BOOL GameLoad(VOID)
{
	//分割数を設定
	tama_moto.x = 4;
	tama_moto.y = 1;

	//画像を分割して読み込み
	if (LoadImageDiv(&tama_moto,
		".\\image\\tama.png",
		tama_moto.x, tama_moto.y , 4) == FALSE)
		return FALSE;

	//位置を設定
	tama_moto.x = GAME_WIDTH / 2 - tama_moto.width / 2;		//中央
	tama_moto.y = GAME_HEIGHT / 2 - tama_moto.height / 2;	//中央

	//速度
	tama_moto.speed = 1;

	//アニメーションを変える速度
	tama_moto.AnimationCntMax = 10;

	//当たり判定の更新
	CollUpdateTama(&tama_moto);

	//画像を表示しない
	tama_moto.IsDraw = FALSE;

	//すべての弾に情報をコピー
	for (int i = 0; i < TAMA_MAX; i++)
	{
		tama[i] = tama_moto;
	}

	return TRUE;
}

/// <summary>
/// 画像を分割してメモリに読み込み
/// </summary>
/// <param name="handle">ハンドル配列の先頭アドレス</param>
/// <param name="path">画像のパス</param>
/// <param name="divX">分割するときの横の数</param>
/// <param name="divY">分割するときの縦の数</param>
/// <returns></returns>
BOOL LoadImageDiv(TAMA *tama,const char *path,int divX,int divY,int divMax)
{
	//アドレスのコピー
	strcpyDx(tama->path, path);
	//弾の読み込み
	int IsTamaLoad = -1;

	//一時的に画像のハンドルを用意する
	tama->handle[0] = LoadGraph(path);

	if (tama->handle[0] == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//ウィンドウハンドル
			path,				//本文
			"画像読み込みエラー",	//タイトル
			MB_OK);
		return FALSE;
	}

	//画像の幅と高さを取得
	int tamawidth = -1;
	int tamaheight = -1;
	GetGraphSize(tama->handle[0], &tamawidth, &tamaheight);

	//分割して読み込み
	IsTamaLoad = LoadDivGraph(
		path,							//画像のパス
		divMax,						//分割総数
		divX, divY,								//横の分割、縦の分割
		tamawidth / divX, tamaheight / divY,		//幅、高さ
		&tama->handle[0]);							//連続で管理する配列の先頭アドレス

	if (IsTamaLoad == -1)
	{
		MessageBox(
			GetMainWindowHandle(),	//ウィンドウハンドル
			path,				//本文
			"画像分割エラー",	//タイトル
			MB_OK);
		return FALSE;
	}

	//一時的に読み込んだハンドルを開放
	//DeleteGraph(tamahandle);

	return TRUE;
}

/// <summary>
/// 音楽をメモリに読み込み
/// </summary>
/// <param name="audio">Audio構造体変数のアドレス</param>
/// <param name="path">Audioの音楽パス</param>
/// <param name="volume">ボリューム</param>
/// <param name="playtype">DX_PLAYTYPE_LOOP or DX_PLAYTYPE_BACK</param>
/// <returns></returns>
BOOL LoadAudio(AUDIO* audio, const char* path, int volume, int playtype)
{
	//音楽の読み込み
	strcpyDx(audio->path, path);					//パスのコピー
	audio->handle = LoadSoundMem(audio->path);	//音楽の読み込み

	//音楽が読み込めなかった時は、エラーが入る
	if (audio->handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),			//メインのウィンドウハンドル
			audio->path,					//メッセージ本文
			TEXT("音楽読み込みエラー！"),	//メッセージタイトル
			MB_OK							//ボタン
		);

		return FALSE;
	}
	//その他の設定
	audio->volume = volume;
	audio->playtype = playtype;

	return TRUE;
}

/// <summary>
/// 画像をメモリに読み込み
/// </summary>
/// <param name=""></param>
BOOL LoadImg(IMAGE* image, const char* path)
{
	//画像の読み込み
	strcpyDx(image->path, path);					//パスのコピー
	image->handle = LoadGraph(image->path);	//音楽の読み込み

	//画像が読み込めなかった時は、エラーが入る
	if (image->handle == -1)
	{
		MessageBox(
			GetMainWindowHandle(),			//メインのウィンドウハンドル
			image->path,					//メッセージ本文
			TEXT("画像読み込みエラー！"),	//メッセージタイトル
			MB_OK							//ボタン
		);

		return FALSE;
	}
	//その他の設定
	//画像の幅と高さを取得
	GetGraphSize(image->handle, &image->width, &image->height);

	return TRUE;
}



/// <summary>
/// ゲームデータを初期化
/// </summary>
/// <param name=""></param>
VOID GameInit(VOID)
{
	

	return;
}

/// <summary>
/// タイトルの画像を初期化
/// </summary>
/// <param name=""></param>
VOID TitleInit(VOID)
{

}

/// <summary>
/// シーンを切り替える関数
/// </summary>
/// <param name="scene">シーン</param>
VOID ChangeScene(GAME_SCENE scene)
{
	GameScene = scene;	//シーンを切り替え
	IsFadeIn = FALSE;	//フェードインしない
	IsFadeOut = TRUE;	//フェードアウトする

	return;
}

/// <summary>
/// タイトル画面
/// </summary>
VOID Title(VOID)
{
	TitleProc();	//処理
	TitleDraw();	//描画

	return;
}

/// <summary>
/// タイトル画面の処理
/// </summary>
VOID TitleProc(VOID)
{
	
	
	//プレイシーンへ切り替える
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//シーンを切り替え
		//次のシーンの初期化をここで行うと楽

		//ゲームの初期化
		GameInit();

		//プレイ画面に切り替え
		ChangeScene(GAME_SCENE_PLAY);

		return;
	}

	return;
}

/// <summary>
/// タイトル画面の描画
/// </summary>
VOID TitleDraw(VOID)
{
	DrawTama(&tama[0]);


	DrawString(0, 0, "タイトル画面", GetColor(0, 0, 0));
	
	return;
}

///弾の描画
VOID DrawTama(TAMA* tama)
{
	//弾の描画
	DrawGraph(0, 0, tama->handle[tama->NowIndex], TRUE);

	if (tama->AnimationCnt < tama->AnimationCntMax)
	{
		tama->AnimationCnt++;
	}
	else
	{
		//弾の添え字が弾の分割数の最大よりも小さいとき
		if (tama->NowIndex < tamaDivMax - 1)
		{
			tama->NowIndex++;
		}
		else
		{
			tama->NowIndex = 0;
		}
		tama->AnimationCnt = 0;
	}
}

/// <summary>
/// プレイ画面
/// </summary>
VOID Play(VOID)
{
	PlayProc();	//処理
	PlayDraw();	//描画

	return;
}

/// <summary>
/// プレイ画面の処理
/// </summary>
VOID PlayProc(VOID)
{
	//エンドシーンへ切り替える
	if (KeyClick(KEY_INPUT_RETURN) == TRUE && GAME_DEBUG == TRUE)
	{

		//シーンを切り替え
		//次のシーンの初期化をここで行うと楽

		//エンド画面に切り替え
		ChangeScene(GAME_SCENE_END);

		return;
	}

	if (KeyDown(KEY_INPUT_UP) == TRUE)
	{
		
	}
	if (KeyDown(KEY_INPUT_DOWN) == TRUE)
	{
		
	}
	if (KeyDown(KEY_INPUT_LEFT) == TRUE)
	{
		
	}
	if (KeyDown(KEY_INPUT_RIGHT) == TRUE)
	{
		
	}
	
	return;
}

/// <summary>
/// プレイ画面の描画
/// </summary>
VOID PlayDraw(VOID)
{
	//背景動画を描画

	
	DrawString(0, 0, "プレイ画面", GetColor(0,0,0));
	return;
}

/// <summary>
/// エンド画面
/// </summary>
VOID End(VOID)
{
	EndProc();	//処理
	EndDraw();	//描画

	return;
}

/// <summary>
/// エンド画面の処理
/// </summary>
VOID EndProc(VOID)
{
	//タイトルシーンへ切り替える
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//シーンを切り替え
		//次のシーンの初期化をここで行うと楽

		//タイトルの初期化
		TitleInit();

		//タイトル画面に切り替え
		ChangeScene(GAME_SCENE_TITLE);

		return;
	}
	
	return;
}

/// <summary>
/// エンド画面の描画
/// </summary>
VOID EndDraw(VOID)
{
	DrawString(0, 0, "エンド画面", GetColor(0, 0, 0));
	
	return;
}

/// <summary>
/// 切り替え画面
/// </summary>
VOID Change(VOID)
{
	ChangeProc();	//処理
	ChangeDraw();	//描画

	return;
}

/// <summary>
/// 切り替え画面の処理
/// </summary>
VOID ChangeProc(VOID)
{
	//フェードイン
	if (IsFadeIn == TRUE)
	{
		if (fadeInCnt > fadeInCntMax)
		{
			fadeInCnt--;	//カウンタを減らす
		}
		else	//フェードイン処理が終わったら
		{
			fadeInCnt = fadeInCntInit;	//カウンタ初期化
			IsFadeIn = FALSE;			//フェードイン終了
		}
	}
	
	//フェードアウト
	if (IsFadeOut == TRUE)
	{
		if (fadeOutCnt < fadeOutCntMax)
		{
			fadeOutCnt++;	//カウンタを増やす
		}
		else	//フェードアウト処理が終わったら
		{
			fadeOutCnt = fadeOutCntInit;	//カウンタ初期化
			IsFadeOut = FALSE;			//フェードアウト終了
		}
	}


	//切り替え処理終了
	if (IsFadeIn == FALSE && IsFadeOut == FALSE)
	{
		//フェードインしていない、フェードアウトもしていないとき
		GameScene = NextGameScene;	//次のシーンに切り替え
		OldGameScene = GameScene;	//以前のゲームシーン更新
	}

	return;
}

/// <summary>
/// 切り替え画面の描画
/// </summary>
VOID ChangeDraw(VOID)
{
	//以前のシーンを描画
	switch (OldGameScene)
	{
	case GAME_SCENE_TITLE:
		TitleDraw();		//タイトル画面の描画
		break;
	case GAME_SCENE_PLAY:
		PlayDraw();			// プレイ 画面の描画
		break;
	case GAME_SCENE_END:
		EndDraw();			// エンド 画面の描画
		break;
	case GAME_SCENE_OVER:
		OverDraw();
		break;
	default:
		break;
	}

	//フェードイン
	if (IsFadeIn == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)fadeInCnt / fadeInCntInit) * 255);	//0-255
	}

	//フェードアウト
	if (IsFadeOut == TRUE)
	{
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, ((float)fadeOutCnt / fadeOutCntMax) * 255);	//0-255
	}

	//四角を描画
	DrawBox(0, 0, GAME_WIDTH, GAME_HEIGHT, GetColor(0, 0, 0),TRUE);

	//半透明終了
	SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

	DrawString(0, 16, "切り替え画面", GetColor(128, 128, 128));
	return;
}

/// <summary>
/// ゲームオーバー画面
/// </summary>
VOID Over(VOID)
{
	OverProc();	//処理
	OverDraw();	//描画

	return;
}

/// <summary>
/// ゲームオーバー画面の処理
/// </summary>
VOID OverProc(VOID)
{
	//タイトルシーンへ切り替える
	if (KeyClick(KEY_INPUT_RETURN) == TRUE)
	{
		//シーンを切り替え
		//次のシーンの初期化をここで行うと楽

		//タイトルの初期化
		TitleInit();

		//タイトル画面に切り替え
		ChangeScene(GAME_SCENE_TITLE);

		return;
	}
	
	return;
}

/// <summary>
/// ゲームオーバー画面の描画
/// </summary>
/// <param name=""></param>
VOID OverDraw(VOID)
{
	DrawString(0, 0, "ゲームオーバー画面", GetColor(0, 0, 0));
	//フェードイン
	
	return;
}

/// <summary>
/// 当たり判定の領域更新
/// </summary>
/// <param name="chara">当たり判定の領域</param>
VOID CollUpdate(CHARA* chara)
{
	chara->coll.left = chara->img.x;
	chara->coll.top = chara->img.y;
	chara->coll.right = chara->img.x + chara->img.width;
	chara->coll.bottom = chara->img.y + chara->img.height;

	return;
}

/// <summary>
/// 当たり判定の領域更新
/// </summary>
/// <param name="chara">当たり判定の領域</param>
VOID CollUpdateTama(TAMA* tama)
{
	tama->coll.left = tama->x;
	tama->coll.top = tama->y;
	tama->coll.right = tama->x + tama->width;
	tama->coll.bottom = tama->y + tama->height;

	return;
}

BOOL CubeCollision(RECT A,RECT B)
{
	if (A.left<B.right			//矩形Aの左辺X座標 < 矩形Bの右辺X座標かつ
		&& A.right>B.left		//矩形Aの右辺X座標 > 矩形Bの左辺X座標かつ
		&& A.top<B.bottom		//矩形Aの上辺Y座標 < 矩形Bの下辺Y座標かつ
		&& A.bottom>B.top)		//矩形Aの下辺Y座標 > 矩形Bの上辺Y座標
	{
		//当たっているとき
		return TRUE;
	}
	else
	{
		//当たっていないとき
		return FALSE;
	}
}