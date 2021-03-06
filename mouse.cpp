//マウス処理のソースファイル

//ヘッダーファイル読み込み
#include "game.h"
#include "mouse.h"
#include "FPS.h"

//グローバル変数

//マウス入力を取得
MOUSE mouse;

//関数

/// <summary>
/// マウスの入力情報を更新する
/// </summary>
VOID MouseUpdate(VOID)
{
	//マウスの以前の情報を取得
	mouse.oldpoint = mouse.point;

	//マウスの以前のボタン入力を取得
	mouse.oldbutton[MOUSE_INPUT_LEFT] = mouse.button[MOUSE_INPUT_LEFT];
	mouse.oldbutton[MOUSE_INPUT_MIDDLE] = mouse.button[MOUSE_INPUT_MIDDLE];
	mouse.oldbutton[MOUSE_INPUT_RIGHT] = mouse.button[MOUSE_INPUT_RIGHT];

	//マウスの座標を取得
	GetMousePoint(&mouse.point.x, &mouse.point.y);

	//マウスの座標が画面外の場合は、画面内に収める
	if (mouse.point.x < 0) { mouse.point.x = 0; }
	if (mouse.point.x > GAME_WIDTH) { mouse.point.x = GAME_WIDTH; }
	if (mouse.point.y < 0) { mouse.point.y = 0; }
	if (mouse.point.y > GAME_HEIGHT) { mouse.point.y = GAME_HEIGHT; }

	//マウスの押しているボタンを取得
	mouse.InputValue = GetMouseInput();

	//左ボタンを押しているかチェック(TRUEは0以外なので、2も4もTRUE)
	if ((mouse.InputValue & MOUSE_INPUT_LEFT) == MOUSE_INPUT_LEFT) { mouse.button[MOUSE_INPUT_LEFT]++; }		//押している
	if ((mouse.InputValue & MOUSE_INPUT_LEFT) != MOUSE_INPUT_LEFT) { mouse.button[MOUSE_INPUT_LEFT] = 0; }		//押していない

	//中ボタンを押しているかチェック(TRUEは0以外なので、2も4もTRUE)
	if ((mouse.InputValue & MOUSE_INPUT_MIDDLE) == MOUSE_INPUT_MIDDLE) { mouse.button[MOUSE_INPUT_MIDDLE]++; }	//押している
	if ((mouse.InputValue & MOUSE_INPUT_MIDDLE) != MOUSE_INPUT_MIDDLE) { mouse.button[MOUSE_INPUT_MIDDLE] = 0; }//押していない

	//右ボタンを押しているかチェック(TRUEは0以外なので、2も4もTRUE)
	if ((mouse.InputValue & MOUSE_INPUT_RIGHT) == MOUSE_INPUT_RIGHT) { mouse.button[MOUSE_INPUT_RIGHT]++; }		//押している
	if ((mouse.InputValue & MOUSE_INPUT_RIGHT) != MOUSE_INPUT_RIGHT) { mouse.button[MOUSE_INPUT_RIGHT] = 0; }	//押していない

	//ホイールの回転量を取得
	mouse.WheelValue = GetMouseWheelRotVol();

	return;
}

/// <summary>
/// ボタンを押しているか、マウスコードで判断する
/// </summary>
/// <param name="MOUSE_INPUT_">MOUSE_INPUT_</param>
/// <returns>ボタンを押しているならTRUE</returns>
BOOL MouseDown(int MOUSE_INPUT_)
{
	//マウスコードのボタンを押している時
	if (mouse.button[MOUSE_INPUT_] != 0)
	{
		return TRUE;	//ボタンを押している
	}
	else
	{
		return FALSE;	//ボタンを押していない
	}
}

/// <summary>
/// ボタンを押し上げたか、マウスコード判断する
/// </summary>
/// <param name="MOUSE_INPUT_">MOUSE_INPUT_</param>
/// <returns>ボタンを押しあげているならTRUE</returns>
BOOL MouseUp(int MOUSE_INPUT_)
{
	if (mouse.oldbutton[MOUSE_INPUT_] >= 1	//直前は押していて
		&& mouse.button[MOUSE_INPUT_] == 0)	//今は押していないとき
	{
		return TRUE;	//ボタンを押し上げている
	}
	else
	{
		return FALSE;	//ボタンを押し上げていない
	}
}

/// <summary>
/// ボタンを押し上げたか、マウスコードで判断する
/// </summary>
/// <param name="MOUSE_INPUT_">MOUSE_INPUT_</param>
/// <param name="milliTime">ボタンを押し続けている時間(ミリ秒)</param>
/// <returns>押し続けていたらTRUE</returns>
BOOL MouseDownKeep(int MOUSE_INPUT_, int milliTime)
{
	float MilliSec = 1000.0f;	//１秒は1000ミリ秒

	//押し続ける時間=秒数×FPS値
	//例）60FPSのゲームで、1秒間押し続けるなら、1秒×60FPS
	int UpdateTime = (milliTime / MilliSec) * fps.Value;

	if (mouse.button[MOUSE_INPUT_] > UpdateTime)
	{
		return TRUE;	//押し続けている
	}
	else
	{
		return FALSE;	//押し続けていない
	}
}

/// <summary>
/// マウスをクリックしたか、マウスコードで判断する
/// </summary>
/// <param name="MOUSE_INPUT_">MOUSE_INPUT_</param>
/// <returns>クリックしたらTRUE</returns>
BOOL MouseClick(int MOUSE_INPUT_)
{
	if (mouse.oldbutton[MOUSE_INPUT_] >= 1		//直前は押していて
		&& mouse.button[MOUSE_INPUT_] == 0)		//今は押していないとき
	{
		return TRUE;	//マウスをクリックした（押し続けても、１回のみ発生）
	}
	else
	{
		return FALSE;	//マウスをクリックしていないか、押し続けている
	}
}

/// <summary>
/// マウスの情報を描画する
/// </summary>
VOID MouseDraw(VOID)
{
	if (GAME_DEBUG == TRUE)	//デバッグモードなら
	{
		//マウスの座標を描画
		DrawFormatString(0, GAME_HEIGHT - 60, GetColor(0, 255, 0), "MOUSE[X:%4d/Y:%4d]", mouse.point.x, mouse.point.y);
	}

	return;
}

/// <summary>
/// マウスが矩形領域をクリックしたか
/// </summary>
/// <param name="rect">矩形領域</param>
/// <param name="MOUSE_INPUT_">マウスコード</param>
BOOL MouseRectClick(RECT rect, int MOUSE_INPUT_)
{
	//点と四角の当たり判定
	if (CheckColliPointToRect(mouse.point, rect) == TRUE)
	{
		//マウスをクリックしているか
		return MouseClick(MOUSE_INPUT_);
	}

	return FALSE;
}

/// <summary>
/// マウスが円をクリックしたか
/// </summary>
/// <param name="circle">円領域</param>
/// <param name="MOUSE_INPUT_">マウスコード</param>
BOOL MouseMaruClick(MARU circle, int MOUSE_INPUT_)
{
	//点と円の当たり判定
	if (CheckColliPointToMaru(mouse.point, circle) == TRUE)
	{
		//マウスをクリックしているか
		return MouseClick(MOUSE_INPUT_);
	}

	return FALSE;
}