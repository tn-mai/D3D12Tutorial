/**
* @file Main.cpp
*/
#include "stdafx.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

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
int WINAPI wWinMain(HINSTANCE instance, HINSTANCE prevInstance, PWSTR cmdLine, int cmdShow)
{
	const wchar_t windowName[] = L"D3D12 Tutorial";
	const wchar_t windowTitle[] = L"D3D12 Tutorial";
	bool fullScreen = false;
	int width = 800;
	int height = 600;

	if (fullScreen) {
		HMONITOR hm = MonitorFromWindow(nullptr, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = { sizeof(MONITORINFO) };
		GetMonitorInfo(hm, &mi);
		width = mi.rcMonitor.right - mi.rcMonitor.left;
		height = mi.rcMonitor.bottom - mi.rcMonitor.top;
	}
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = instance;
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION); // タスクマネージャ等で表示されるアイコン.
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_MENUTEXT);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = windowName;
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION); // タイトルバーの左端に表示されるアイコン.
	if (!RegisterClassEx(&wc)) {
		MessageBox(nullptr, L"ウィンドウクラスの登録に失敗", L"エラー", MB_OK | MB_ICONERROR);
		return 0;
	}
	HWND hwnd = CreateWindowEx(0, windowName, windowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, instance, nullptr);
	if (!hwnd) {
		MessageBox(nullptr, L"ウィンドウの作成に失敗", L"エラー", MB_OK | MB_ICONERROR);
		return 0;
	}
	// フルスクリーンモードの場合、フルスクリーン表示では不要なウィンドウスタイルを取り除く.
	// WS_OVERLAPPEDを指定するとタイトルバーとウィンドウ境界だけを持つようになる.
	if (fullScreen) {
		SetWindowLong(hwnd, GWL_STYLE, WS_OVERLAPPED);
	}

	ShowWindow(hwnd, cmdShow);
	UpdateWindow(hwnd);

	MSG msg = { 0 };
	for (;;) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		// TODO: ここにアプリケーションのコードを書く.
	}

	return 0;
}

/**
* ウィンドウプロシージャ.
*
* OSからのメッセージを処理する.
*
* @param hwnd   メッセージの宛先のウィンドウのハンドル.
*               これによって、複数のインスタンスが作成された場合でも、どのウィンドウ宛に送られたメッセージなのかが分かる.
* @param msg    送られてきたメッセージ.
* @param wParam メッセージの追加パラメータその1.
* @param lParam メッセージの追加パラメータその2.
*
* @return メッセージの処理結果.
*         メッセージの種類によって返すべき値が決まっている.
*         ほとんどのメッセージは常に0を返すことを要求する. しかし、例えばWM_CREATEは0以外にも-1を返すことができる.
*         WM_CREATEはCreateWindowEx関数の処理中にウィンドウプロシージャへ送られる.
*         ここでウィンドウプロシージャが-1を返すと、OSはウィンドウの作成を中止し、CreateWindowEx()関数はnullptrを
*         返す.
*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			if (MessageBox(nullptr, L"終了しますか？", L"終了", MB_YESNO | MB_ICONQUESTION) == IDYES) {
				DestroyWindow(hwnd);
			}
		}
		return 0;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}