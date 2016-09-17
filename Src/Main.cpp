/**
* @file Main.cpp
*/
#include "stdafx.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

/**
* �A�v���P�[�V�����̃G���g���|�C���g.
*
* @param instance     OS����n�����A�v���P�[�V�����̃C���X�^���X�n���h��.
*                     �A�C�R����r�b�g�}�b�v��ǂݍ��ނȂǁAWindows�̓���̋@�\���Ăяo���ۂɕK�v�ƂȂ�.
* @param prevInstance 32�r�b�gWindows�Ȍ�͏��0���n�����(16�r�b�gWindows����̈�Y).
* @param cmdLine      �R�}���h���C������.
* @param cmdShow      �E�B���h�E�̍ŏ����A�ő剻�A�ʏ�\�����w�肷�邽�߂̃t���O.
*
* @return �A�v���P�[�V�����I�����ɕԂ����X�e�[�^�X�R�[�h.
*         CLI�A�v���P�[�V�����ɂ����āA�������ʂ�G���[�̒ʒm�Ɏg���邱�Ƃ�����.
*         GUI�A�v���P�[�V�����ł͂��܂�g�p����Ȃ�.
*         ���Ɏg�p���Ȃ��ꍇ��0��Ԃ��Ă����Ƃ悢(����I���������l�Ƃ��Ĉ�ʓI�Ɏg���Ă��邽��).
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
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION); // �^�X�N�}�l�[�W�����ŕ\�������A�C�R��.
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_MENUTEXT);
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = windowName;
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION); // �^�C�g���o�[�̍��[�ɕ\�������A�C�R��.
	if (!RegisterClassEx(&wc)) {
		MessageBox(nullptr, L"�E�B���h�E�N���X�̓o�^�Ɏ��s", L"�G���[", MB_OK | MB_ICONERROR);
		return 0;
	}
	HWND hwnd = CreateWindowEx(0, windowName, windowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, nullptr, nullptr, instance, nullptr);
	if (!hwnd) {
		MessageBox(nullptr, L"�E�B���h�E�̍쐬�Ɏ��s", L"�G���[", MB_OK | MB_ICONERROR);
		return 0;
	}
	// �t���X�N���[�����[�h�̏ꍇ�A�t���X�N���[���\���ł͕s�v�ȃE�B���h�E�X�^�C������菜��.
	// WS_OVERLAPPED���w�肷��ƃ^�C�g���o�[�ƃE�B���h�E���E���������悤�ɂȂ�.
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

		// TODO: �����ɃA�v���P�[�V�����̃R�[�h������.
	}

	return 0;
}

/**
* �E�B���h�E�v���V�[�W��.
*
* OS����̃��b�Z�[�W����������.
*
* @param hwnd   ���b�Z�[�W�̈���̃E�B���h�E�̃n���h��.
*               ����ɂ���āA�����̃C���X�^���X���쐬���ꂽ�ꍇ�ł��A�ǂ̃E�B���h�E���ɑ���ꂽ���b�Z�[�W�Ȃ̂���������.
* @param msg    �����Ă������b�Z�[�W.
* @param wParam ���b�Z�[�W�̒ǉ��p�����[�^����1.
* @param lParam ���b�Z�[�W�̒ǉ��p�����[�^����2.
*
* @return ���b�Z�[�W�̏�������.
*         ���b�Z�[�W�̎�ނɂ���ĕԂ��ׂ��l�����܂��Ă���.
*         �قƂ�ǂ̃��b�Z�[�W�͏��0��Ԃ����Ƃ�v������. �������A�Ⴆ��WM_CREATE��0�ȊO�ɂ�-1��Ԃ����Ƃ��ł���.
*         WM_CREATE��CreateWindowEx�֐��̏������ɃE�B���h�E�v���V�[�W���֑�����.
*         �����ŃE�B���h�E�v���V�[�W����-1��Ԃ��ƁAOS�̓E�B���h�E�̍쐬�𒆎~���ACreateWindowEx()�֐���nullptr��
*         �Ԃ�.
*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			if (MessageBox(nullptr, L"�I�����܂����H", L"�I��", MB_YESNO | MB_ICONQUESTION) == IDYES) {
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