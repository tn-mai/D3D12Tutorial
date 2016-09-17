/**
* @file Main.cpp
*/
#include "stdafx.h"

/**
* アプリケーションのエントリポイント.
*
* @param instance     OSから渡されるアプリケーションのインスタンスハンドル.
*                     アイコンやビットマップを読み込むなど、Windowsの特定の機能を呼び出す際に必要となる.
* @param prevInstance 32ビットWindows以後は常に0が渡される(16ビットWindows時代の遺産).
* @param cmdLine      コマンドライン引数.
* @param cmdShow      ウィンドウの最小化、最大化、通常表示を指定するためのフラグ.
*
* @return アプリケーション終了時に返されるステータスコード.
*         CLIアプリケーションにおいて、処理結果やエラーの通知に使われることが多い.
*         GUIアプリケーションではあまり使用されない.
*         特に使用しない場合は0を返しておくとよい(正常終了を示す値として一般的に使われているため).
*/
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prevInstance, PWSTR cmdLine, int cmdShow) {
	return 0;
}