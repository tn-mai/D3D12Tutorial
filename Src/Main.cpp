/**
* @file Main.cpp
*/
#include "stdafx.h"
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

struct Direct3DStuff {
	static const int frameBufferCount = 3;
	int witdh;
	int height;
	bool fullScreen;
	bool running;
	bool initialized;
	HWND hwnd;

	ComPtr<ID3D12Device> device;
	ComPtr<IDXGISwapChain3> swapChain;
	ComPtr<ID3D12CommandQueue> commandQueue;
	ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	ComPtr<ID3D12Resource> renderTargetList[frameBufferCount];
	ComPtr<ID3D12CommandAllocator> commandAllocator[frameBufferCount];
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12Fence> fence;
	HANDLE fenceEvent;
	UINT64 masterFenceValue;
	UINT64 fenceValueForFrameBuffer[frameBufferCount];
	int frameIndex;
	int rtvDescriptorSize;
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool Initialize(Direct3DStuff&);
void Finalize(Direct3DStuff&);
void Update(Direct3DStuff&);
void Render(Direct3DStuff&);
void WaitForPreviousFrame(Direct3DStuff&);

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

	Direct3DStuff d3dStuff;
	d3dStuff.witdh = width;
	d3dStuff.height = height;
	d3dStuff.fullScreen = fullScreen;
	d3dStuff.hwnd = hwnd;
	d3dStuff.running = true;
	d3dStuff.initialized = false;
	d3dStuff.masterFenceValue = 0;
	d3dStuff.fenceEvent = nullptr;
	if (!Initialize(d3dStuff)) {
		MessageBox(nullptr, L"DirectX�̏������Ɏ��s", L"�G���[", MB_OK | MB_ICONERROR);
		Finalize(d3dStuff);
		return 0;
	}

	MSG msg = { 0 };
	while (d3dStuff.running) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			if (msg.message == WM_QUIT) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}

		// TODO: �����ɃA�v���P�[�V�����̃R�[�h������.
		Update(d3dStuff);
		Render(d3dStuff);
	}

	Finalize(d3dStuff);

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

/**
* Direct3D�̏�����.
*
* @retval true ����������.
* @retval false ���������s.
*/
bool Init3D(Direct3DStuff& d3dStuff)
{
	HRESULT hr;

	IDXGIFactory4* dxgiFactory;
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(hr)) {
		return false;
	}

	// �@�\���x��11�𖞂����n�[�h�E�F�A�A�_�v�^���������A���̃f�o�C�X�C���^�[�t�F�C�X���擾����.
	IDXGIAdapter1* dxgiAdapter;
	int adapterIndex = 0;
	bool adapterFound = false;
	while (dxgiFactory->EnumAdapters1(adapterIndex, &dxgiAdapter) != DXGI_ERROR_NOT_FOUND) {
		DXGI_ADAPTER_DESC1 desc;
		dxgiAdapter->GetDesc1(&desc);
		if (!(desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)) {
			hr = D3D12CreateDevice(dxgiAdapter, D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr);
			if (SUCCEEDED(hr)) {
				adapterFound = true;
				break;
			}
		}
		++adapterIndex;
	}
	if (!adapterFound) {
		// �@�\���x��11�𖞂����n�[�h�E�F�A��������Ȃ��ꍇ�AWARP�f�o�C�X�̍쐬�����݂�.
		hr = dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter));
		if (FAILED(hr)) {
			return false;
		}
	}
	hr = D3D12CreateDevice(dxgiAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(d3dStuff.device.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}

	// �R�}���h�L���[���쐬.
	// �ʏ�A�R�}���h�L���[�̓f�o�C�X���Ƃ�1�����쐬����.
	D3D12_COMMAND_QUEUE_DESC cqDesc = {};
	hr = d3dStuff.device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(d3dStuff.commandQueue.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}

	// �X���b�v�`�F�[�����쐬.
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width = d3dStuff.witdh;
	scDesc.Height = d3dStuff.height;
	scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.SampleDesc.Count = 1;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.BufferCount = d3dStuff.frameBufferCount;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	ComPtr<IDXGISwapChain1> tmpSwapChain;
	hr = dxgiFactory->CreateSwapChainForHwnd(d3dStuff.commandQueue.Get(), d3dStuff.hwnd, &scDesc, nullptr, nullptr, tmpSwapChain.GetAddressOf());
	if (FAILED(hr)) {
		return false;
	}
	tmpSwapChain.As(&d3dStuff.swapChain);
	d3dStuff.frameIndex = d3dStuff.swapChain->GetCurrentBackBufferIndex();

	// RTV�p�̃f�X�N���v�^�q�[�v�y�уf�X�N���v�^���쐬.
	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDesc.NumDescriptors = d3dStuff.frameBufferCount;
	rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = d3dStuff.device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(d3dStuff.rtvDescriptorHeap.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}
	d3dStuff.rtvDescriptorSize = d3dStuff.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3dStuff.rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (int i = 0; i < d3dStuff.frameBufferCount; ++i) {
		hr = d3dStuff.swapChain->GetBuffer(i, IID_PPV_ARGS(d3dStuff.renderTargetList[i].GetAddressOf()));
		if (FAILED(hr)) {
			return false;
		}
		d3dStuff.device->CreateRenderTargetView(d3dStuff.renderTargetList[i].Get(), nullptr, rtvHandle);
		rtvHandle.ptr += d3dStuff.rtvDescriptorSize;
	}

	// �R�}���h�A���P�[�^���쐬.
	// �R�}���h�A���P�[�^�͕`�撆��GPU�����s����e�R�}���h��ێ�����.
	// GPU���`�撆�̃R�}���h��j�������ꍇ�AGPU�̓���͖���`�ɂȂ��Ă��܂�. ���̂��߁A�`�撆�̓R�}���h��ێ���������K�v������.
	// �����ł́A�e�o�b�N�o�b�t�@�ɂЂƂÂR�}���h�A���P�[�^����������.
	// ����ɂ���āA����o�b�N�o�b�t�@���`�撆�ł��A���̃o�b�N�o�b�t�@�̂��߂̃R�}���h�𐶐��E�j���ł���悤�ɂ���.
	for (int i = 0; i < d3dStuff.frameBufferCount; ++i) {
		hr = d3dStuff.device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(d3dStuff.commandAllocator[i].GetAddressOf()));
		if (FAILED(hr)) {
			return false;
		}
	}

	// �R�}���h���X�g���쐬.
	// �R�}���h�A���P�[�^�ƈقȂ�A�R�}���h���X�g�̓R�}���h�L���[�����s���ꂽ���ƂȂ炢�ł����Z�b�g�ł���.
	// �o�b�N�o�b�t�@���Ɏ��K�v���Ȃ�����1�������΂悢.
	// ���̂����A�`�悵�����o�b�N�o�b�t�@���ς�閈�ɁA�Ή�����R�}���h�A���P�[�^��ݒ肵�����K�v������.
	hr = d3dStuff.device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, d3dStuff.commandAllocator[0].Get(), nullptr, IID_PPV_ARGS(d3dStuff.commandList.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}
	d3dStuff.commandList->Close();

	// �t�F���X�ƃt�F���X�C�x���g���쐬.
	// DirectX 12�ł́AGPU�̕`��I�������o���邽�߂Ƀt�F���X�Ƃ������̂��g��.
	// �t�F���X��OS�̃C�x���g���֘A�t���邱�Ƃ��ł���悤�ɐ݌v����Ă���.
	// �����ŁAOS�̃C�x���g���쐬���A�t�F���X���R�}���h���X�g�֐ݒ肷��Ƃ��Ɋ֘A�t����.
	hr = d3dStuff.device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(d3dStuff.fence.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}
	d3dStuff.fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!d3dStuff.fenceEvent) {
		return false;
	}
	++d3dStuff.masterFenceValue;
	for (int i = 0; i < d3dStuff.frameBufferCount; ++i) {
		d3dStuff.fenceValueForFrameBuffer[i] = 0;
	}

	d3dStuff.initialized = true;

	return true;
}

/**
* DirectX�̏�����.
*
* @retval true ����������.
* @retval false ���������s.
*/
bool Initialize(Direct3DStuff& d3dStuff)
{
	if (!Init3D(d3dStuff)) {
		return false;
	}
	return true;
}

/**
* DirectX�̔j��.
*/
void Finalize(Direct3DStuff& d3dStuff)
{
	if (d3dStuff.initialized) {
		for (int i = 0; i < d3dStuff.frameBufferCount; ++i) {
			d3dStuff.frameIndex = i;
			WaitForPreviousFrame(d3dStuff);
		}

		BOOL fs = FALSE;
		if (d3dStuff.swapChain->GetFullscreenState(&fs, nullptr)) {
			d3dStuff.swapChain->SetFullscreenState(false, nullptr);
		}
	}

	if (d3dStuff.fenceEvent) {
		CloseHandle(d3dStuff.fenceEvent);
	}
}

/**
* ���O�̃t���[���̕`�抮����҂�.
*/
void WaitForPreviousFrame(Direct3DStuff& d3dStuff)
{
	const UINT64 lastCompletedFence = d3dStuff.fence->GetCompletedValue();
	d3dStuff.frameIndex = d3dStuff.swapChain->GetCurrentBackBufferIndex();
	if (d3dStuff.fenceValueForFrameBuffer[d3dStuff.frameIndex] > lastCompletedFence) {
		HRESULT hr = d3dStuff.fence->SetEventOnCompletion(d3dStuff.fenceValueForFrameBuffer[d3dStuff.frameIndex], d3dStuff.fenceEvent);
		if (FAILED(hr)) {
			d3dStuff.running = false;
		}
		WaitForSingleObject(d3dStuff.fenceEvent, INFINITE);
	}
}

/**
* �����_�����O�p�C�v���C���̍X�V.
*/
void Render(Direct3DStuff& d3dStuff)
{
	WaitForPreviousFrame(d3dStuff);

	// �R�}���h���X�g�y�уR�}���h�A���P�[�^�����Z�b�g.
	HRESULT hr;
	hr = d3dStuff.commandAllocator[d3dStuff.frameIndex]->Reset();
	if (FAILED(hr)) {
		d3dStuff.running = false;
	}
	hr = d3dStuff.commandList->Reset(d3dStuff.commandAllocator[d3dStuff.frameIndex].Get(), nullptr);
	if (FAILED(hr)) {
		d3dStuff.running = false;
	}

	// �R�}���h��ς�ł���.
	d3dStuff.commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(d3dStuff.renderTargetList[d3dStuff.frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3dStuff.rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += d3dStuff.frameIndex * d3dStuff.rtvDescriptorSize;
	d3dStuff.commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	static const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	d3dStuff.commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	d3dStuff.commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(d3dStuff.renderTargetList[d3dStuff.frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	hr = d3dStuff.commandList->Close();
	if (FAILED(hr)) {
		d3dStuff.running = false;
	}

	// �`��J�n.
	ID3D12CommandList* commandListArray[] = { d3dStuff.commandList.Get() };
	d3dStuff.commandQueue->ExecuteCommandLists(_countof(commandListArray), commandListArray);
	hr = d3dStuff.swapChain->Present(1, 0);
	if (FAILED(hr)) {
		d3dStuff.running = false;
	}

	d3dStuff.fenceValueForFrameBuffer[d3dStuff.frameIndex] = d3dStuff.masterFenceValue;
	hr = d3dStuff.commandQueue->Signal(d3dStuff.fence.Get(), d3dStuff.masterFenceValue);
	if (FAILED(hr)) {
		d3dStuff.running = false;
	}
	++d3dStuff.masterFenceValue;
}

/**
* �V�[���̍X�V.
*/
void Update(Direct3DStuff&)
{
}