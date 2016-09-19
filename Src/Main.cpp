/**
* @file Main.cpp
*/
#include "stdafx.h"
#include <wrl/client.h>

using Microsoft::WRL::ComPtr;

struct Direct3DStuff {
	static const int frameBufferCount = 3;
	int width;
	int height;
	bool fullScreen;
	bool running;
	bool warp;
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

	ComPtr<ID3D12PipelineState> pso;
	ComPtr<ID3DBlob> signatureBlob;
	ComPtr<ID3D12RootSignature> rootSignature;
	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;
	ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	ComPtr<ID3DBlob> vertexShaderBlob;
	ComPtr<ID3DBlob> pixelShaderBlob;
};

/**
* Vertex構造体のレイアウト.
*/
D3D12_INPUT_ELEMENT_DESC vertexLayout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

/**
* 頂点データ型.
*/
struct Vertex {
	DirectX::XMFLOAT3 pos;
};

/**
* 頂点データ配列.
*/
Vertex vertexList[] = {
	{ { 0.0f, 0.5f, 0.5f } },
	{ { 0.5f, -0.5f, 0.5f } },
	{ { -0.5f, -0.5f, 0.5f } },
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool Initialize(Direct3DStuff&);
void Finalize(Direct3DStuff&);
void Update(Direct3DStuff&);
void Render(Direct3DStuff&);
void WaitForPreviousFrame(Direct3DStuff&);

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

	Direct3DStuff d3dStuff;
	d3dStuff.width = width;
	d3dStuff.height = height;
	d3dStuff.fullScreen = fullScreen;
	d3dStuff.hwnd = hwnd;
	d3dStuff.running = true;
	d3dStuff.initialized = false;
	d3dStuff.warp = false;
	d3dStuff.masterFenceValue = 0;
	d3dStuff.fenceEvent = nullptr;
	if (!Initialize(d3dStuff)) {
		MessageBox(nullptr, L"DirectXの初期化に失敗", L"エラー", MB_OK | MB_ICONERROR);
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

		// TODO: ここにアプリケーションのコードを書く.
		Update(d3dStuff);
		Render(d3dStuff);
	}

	Finalize(d3dStuff);

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

/**
* Direct3Dの初期化.
*
* @retval true 初期化成功.
* @retval false 初期化失敗.
*/
bool Init3D(Direct3DStuff& d3dStuff)
{
	HRESULT hr;

	IDXGIFactory4* dxgiFactory;
	hr = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(hr)) {
		return false;
	}

	// 機能レベル11を満たすハードウェアアダプタを検索し、そのデバイスインターフェイスを取得する.
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
		// 機能レベル11を満たすハードウェアが見つからない場合、WARPデバイスの作成を試みる.
		hr = dxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&dxgiAdapter));
		if (FAILED(hr)) {
			return false;
		}
		d3dStuff.warp = true;
	}
	hr = D3D12CreateDevice(dxgiAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(d3dStuff.device.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}

	// コマンドキューを作成.
	// 通常、コマンドキューはデバイスごとに1つだけ作成する.
	D3D12_COMMAND_QUEUE_DESC cqDesc = {};
	hr = d3dStuff.device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(d3dStuff.commandQueue.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}

	// スワップチェーンを作成.
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width = d3dStuff.width;
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

	// RTV用のデスクリプタヒープ及びデスクリプタを作成.
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

	// コマンドアロケータを作成.
	// コマンドアロケータは描画中にGPUが実行する各コマンドを保持する.
	// GPUが描画中のコマンドを破棄した場合、GPUの動作は未定義になってしまう. そのため、描画中はコマンドを保持し続ける必要がある.
	// ここでは、各バックバッファにひとつづつコマンドアロケータを持たせる.
	// これによって、あるバックバッファが描画中でも、他のバックバッファのためのコマンドを生成・破棄できるようにする.
	for (int i = 0; i < d3dStuff.frameBufferCount; ++i) {
		hr = d3dStuff.device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(d3dStuff.commandAllocator[i].GetAddressOf()));
		if (FAILED(hr)) {
			return false;
		}
	}

	// コマンドリストを作成.
	// コマンドアロケータと異なり、コマンドリストはコマンドキューが実行されたあとならいつでもリセットできる.
	// バックバッファ毎に持つ必要がないため1つだけ作ればよい.
	// そのかわり、描画したいバックバッファが変わる毎に、対応するコマンドアロケータを設定し直す必要がある.
	// 生成された直後のコマンドリストはリセットが呼び出された直後と同じ状態になっているため、すぐにコマンドを送り込むことが出来る.
	hr = d3dStuff.device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, d3dStuff.commandAllocator[0].Get(), nullptr, IID_PPV_ARGS(d3dStuff.commandList.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}

	// フェンスとフェンスイベントを作成.
	// DirectX 12では、GPUの描画終了を検出するためにフェンスというものを使う.
	// フェンスはOSのイベントを関連付けることができるように設計されている.
	// そこで、OSのイベントを作成し、フェンスをコマンドリストへ設定するときに関連付ける.
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

	// ルートシグネチャを作成.
	D3D12_ROOT_SIGNATURE_DESC rsDesc = { 0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT };
	hr = D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &d3dStuff.signatureBlob, nullptr);
	if (FAILED(hr)) {
		return false;
	}
	hr = d3dStuff.device->CreateRootSignature(0, d3dStuff.signatureBlob->GetBufferPointer(), d3dStuff.signatureBlob->GetBufferSize(), IID_PPV_ARGS(d3dStuff.rootSignature.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}

	// 頂点シェーダを作成.
	ComPtr<ID3DBlob> errorBuffer;
	hr = D3DCompileFromFile(L"Res/VertexShader.hlsl", nullptr, nullptr, "main", "vs_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, d3dStuff.vertexShaderBlob.GetAddressOf(), errorBuffer.GetAddressOf());
	if (FAILED(hr)) {
		if (errorBuffer) {
			OutputDebugStringA(static_cast<char*>(errorBuffer->GetBufferPointer()));
		}
		return false;
	}

	// ピクセルシェーダを作成.
	hr = D3DCompileFromFile(L"Res/PixelShader.hlsl", nullptr, nullptr, "main", "ps_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, d3dStuff.pixelShaderBlob.GetAddressOf(), errorBuffer.GetAddressOf());
	if (FAILED(hr)) {
		if (errorBuffer) {
			OutputDebugStringA(static_cast<char*>(errorBuffer->GetBufferPointer()));
		}
		return false;
	}

	// パイプラインステートオブジェクト(PSO)を作成.
	// PSOは、レンダリングパイプラインの状態を素早く、一括して変更できるように導入された.
	// PSOによって、多くのステートに対してそれぞれ状態変更コマンドを送らずとも、単にPSOを切り替えるコマンドを送るだけで済む.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = d3dStuff.rootSignature.Get();
	psoDesc.VS.pShaderBytecode = d3dStuff.vertexShaderBlob->GetBufferPointer();
	psoDesc.VS.BytecodeLength = d3dStuff.vertexShaderBlob->GetBufferSize();
	psoDesc.PS.pShaderBytecode = d3dStuff.pixelShaderBlob->GetBufferPointer();
	psoDesc.PS.BytecodeLength = d3dStuff.pixelShaderBlob->GetBufferSize();
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.InputLayout.pInputElementDescs = vertexLayout;
	psoDesc.InputLayout.NumElements = sizeof(vertexLayout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	if (d3dStuff.warp) {
		psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG;
	}
	hr = d3dStuff.device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(d3dStuff.pso.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}

	// 頂点バッファを作成.
	hr = d3dStuff.device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertexList)),
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(d3dStuff.vertexBuffer.GetAddressOf())
	);
	if (FAILED(hr)) {
		return false;
	}
	d3dStuff.vertexBuffer->SetName(L"Vertex Buffer");

	// 頂点バッファに頂点データ配列を転送.
	ComPtr<ID3D12Resource> vbUploadHeap;
	hr = d3dStuff.device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertexList)),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(vbUploadHeap.GetAddressOf())
	);
	if (FAILED(hr)) {
		return false;
	}
	vbUploadHeap->SetName(L"Vertex Buffer Uplaod Heap");
	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData = vertexList;
	vertexData.RowPitch = sizeof(vertexList);
	vertexData.SlicePitch = sizeof(vertexList);
	if (UpdateSubresources<1>(d3dStuff.commandList.Get(), d3dStuff.vertexBuffer.Get(), vbUploadHeap.Get(), 0, 0, 1, &vertexData) == 0) {
		return false;
	}
	d3dStuff.commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(d3dStuff.vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER));
	d3dStuff.vertexBufferView.BufferLocation = d3dStuff.vertexBuffer->GetGPUVirtualAddress();
	d3dStuff.vertexBufferView.StrideInBytes = sizeof(Vertex);
	d3dStuff.vertexBufferView.SizeInBytes = sizeof(vertexList);

	hr = d3dStuff.commandList->Close();
	if (FAILED(hr)) {
		return false;
	}
	ID3D12CommandList* commandListArray[] = { d3dStuff.commandList.Get() };
	d3dStuff.commandQueue->ExecuteCommandLists(_countof(commandListArray), commandListArray);
	hr = d3dStuff.commandQueue->Signal(d3dStuff.fence.Get(), d3dStuff.masterFenceValue);
	if (FAILED(hr)) {
		return false;
	}
	d3dStuff.fence->SetEventOnCompletion(d3dStuff.masterFenceValue, d3dStuff.fenceEvent);
	WaitForSingleObject(d3dStuff.fenceEvent, INFINITE);
	++d3dStuff.masterFenceValue;

	d3dStuff.viewport.TopLeftX = 0;
	d3dStuff.viewport.TopLeftY = 0;
	d3dStuff.viewport.Width = static_cast<float>(d3dStuff.width);
	d3dStuff.viewport.Height = static_cast<float>(d3dStuff.height);
	d3dStuff.viewport.MinDepth = 0.0f;
	d3dStuff.viewport.MaxDepth = 1.0f;

	d3dStuff.scissorRect.left = 0;
	d3dStuff.scissorRect.top = 0;
	d3dStuff.scissorRect.right = d3dStuff.width;
	d3dStuff.scissorRect.bottom = d3dStuff.height;

	d3dStuff.initialized = true;

	return true;
}

/**
* DirectXの初期化.
*
* @retval true 初期化成功.
* @retval false 初期化失敗.
*/
bool Initialize(Direct3DStuff& d3dStuff)
{
	if (!Init3D(d3dStuff)) {
		return false;
	}
	return true;
}

/**
* DirectXの破棄.
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
* 直前のフレームの描画完了を待つ.
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
* レンダリングパイプラインの更新.
*/
void Render(Direct3DStuff& d3dStuff)
{
	WaitForPreviousFrame(d3dStuff);

	// コマンドリスト及びコマンドアロケータをリセット.
	HRESULT hr;
	hr = d3dStuff.commandAllocator[d3dStuff.frameIndex]->Reset();
	if (FAILED(hr)) {
		d3dStuff.running = false;
	}
	hr = d3dStuff.commandList->Reset(d3dStuff.commandAllocator[d3dStuff.frameIndex].Get(), d3dStuff.pso.Get());
	if (FAILED(hr)) {
		d3dStuff.running = false;
	}

	// コマンドを積んでいく.
	d3dStuff.commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(d3dStuff.renderTargetList[d3dStuff.frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3dStuff.rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += d3dStuff.frameIndex * d3dStuff.rtvDescriptorSize;
	d3dStuff.commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	static const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	d3dStuff.commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	d3dStuff.commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(d3dStuff.renderTargetList[d3dStuff.frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// 頂点を描画.
	d3dStuff.commandList->SetGraphicsRootSignature(d3dStuff.rootSignature.Get());
	d3dStuff.commandList->RSSetViewports(1, &d3dStuff.viewport);
	d3dStuff.commandList->RSSetScissorRects(1, &d3dStuff.scissorRect);
	d3dStuff.commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	d3dStuff.commandList->IASetVertexBuffers(0, 1, &d3dStuff.vertexBufferView);
	d3dStuff.commandList->DrawInstanced(3, 1, 0, 0);

	hr = d3dStuff.commandList->Close();
	if (FAILED(hr)) {
		d3dStuff.running = false;
	}

	// 描画開始.
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
* シーンの更新.
*/
void Update(Direct3DStuff&)
{
}