/**
* @file Main.cpp
*/
#include "stdafx.h"
#include "Texture.h"
#include "Font.h"
#include <algorithm>
#include <fstream>
#include <string>

using Microsoft::WRL::ComPtr;

/**
* Vertex構造体のレイアウト.
*/
D3D12_INPUT_ELEMENT_DESC vertexLayout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

/**
* 頂点データ型.
*/
struct Vertex {
	constexpr Vertex(float x, float y, float z, float r, float g, float b, float a, float u, float v) : pos(x, y, z), col(r, g, b, a), texCoord(u, v) {}
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT4 col;
	DirectX::XMFLOAT2 texCoord;
};

/**
* 頂点データ配列.
*/
Vertex vertexList[] = {
	// front face
	{ -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
	{ 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f , 1.0f, 1.0f },
	{ -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f },
	{ 0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f , 1.0f, 0.0f },

	// right side face
	{ 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
	{ 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f },
	{ 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f },
	{ 0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f },

	// left side face
	{ -0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
	{ -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f },
	{ -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f },
	{ -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f },

	// back face
	{ 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f , 0.0f, 0.0f },
	{ -0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f },
	{ 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f, 1.0f , 0.0f, 1.0f },
	{ -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f },

	// top face
	{ -0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f },
	{ 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 1.0f, 1.0f , 1.0f, 1.0f },
	{ 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f , 0.0f, 1.0f },
	{ -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f },

	// bottom face
	{ 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f, 1.0f , 0.0f, 0.0f },
	{ -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f },
	{ 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f , 0.0f, 1.0f },
	{ -0.5f, -0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f },
};

/**
* インデックスデータ配列.
*/
UINT indexList[] = {
	0, 1, 2,
	0, 3, 1,
	4, 5, 6,
	4, 7, 5,
	8, 9, 10,
	8, 11, 9,
	12, 13, 14,
	12, 15, 13,
	16, 17, 18,
	16, 19, 17,
	20, 21, 22,
	20, 23, 21,
};

/**
* 定数バッファ型.
*
* DirectX 12では、定数バッファのサイズを256バイト単位にすることが要求されている.
*/
struct ConstantBuffer {
	DirectX::XMFLOAT4X4 wvpMatrix;
};
const size_t AlignedConstantBufferSize = (sizeof(ConstantBuffer) + 255) & ~255UL;

/**
* オブジェクト用データ.
*/
struct ObjectState {
	DirectX::XMFLOAT4X4 matWorld;
	DirectX::XMFLOAT4X4 matRot;
	DirectX::XMFLOAT4 pos;
};

/**
* Direct3Dで必要なオブジェクトをまとめたもの.
*/
struct Direct3DStuff {
	static const int frameBufferCount = 3;
	static const int objectCount = 2;

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
	ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;
	ComPtr<ID3D12Resource> depthStencilbuffer;
	ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
	ComPtr<ID3D12Resource> cbvUploadHeapList[frameBufferCount];
	void* cbvHeapBegin[frameBufferCount];
	ComPtr<ID3D12GraphicsCommandList> prologueCommandList;
	ComPtr<ID3D12GraphicsCommandList> epilogueCommandList;
	ComPtr<ID3D12CommandAllocator> commandAllocator[frameBufferCount];
	ComPtr<ID3D12GraphicsCommandList> commandList;
	ComPtr<ID3D12Fence> fence;
	HANDLE fenceEvent;
	UINT64 masterFenceValue;
	UINT64 fenceValueForFrameBuffer[frameBufferCount];
	int frameIndex;
	int rtvDescriptorSize;
	int srvDescriptorSize;

	ComPtr<ID3D12PipelineState> pso;
	ComPtr<ID3DBlob> signatureBlob;
	ComPtr<ID3D12RootSignature> rootSignature;
	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;

	ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	ComPtr<ID3D12Resource> indexBuffer;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	ComPtr<ID3DBlob> vertexShaderBlob;
	ComPtr<ID3DBlob> pixelShaderBlob;

	ComPtr<ID3D12Resource> textureBuffer;
	ComPtr<ID3D12Resource> textureBufferUploadHeap;

	ConstantBuffer cbPerObject;

	DirectX::XMFLOAT4X4 matProjection;
	DirectX::XMFLOAT4X4 matView;
	DirectX::XMFLOAT4 cameraPos;
	DirectX::XMFLOAT4 cameraTarget;
	DirectX::XMFLOAT4 cameraUp;

	ObjectState objectState[objectCount];

	Font fontInfo;
	FontRenderer fontRenderer;
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

	// D3D12非対応ハードウェアではWARPデバイスを使うことになるが、WARPデバイスはフルスクリーンモードに対応していない.
	// そのため、スクリーン全体を覆うような縁・タイトルバーなしのウィンドウを作ることで代用する.
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
* シェーダを読み込む.
*
* @param filename シェーダファイル名.
* @param target   対象とするシェーダバージョン.
* @param blob     読み込んだシェーダを格納するBlobオブジェクト.
*
* @retval true 読み込み成功.
* @retval false 読み込み失敗.
*/
bool LoadShader(const wchar_t* filename, const char* target, ComPtr<ID3DBlob>& blob)
{
	ComPtr<ID3DBlob> errorBuffer;
	HRESULT hr = D3DCompileFromFile(filename, nullptr, nullptr, "main", target, D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, blob.GetAddressOf(), errorBuffer.GetAddressOf());
	if (FAILED(hr)) {
		if (errorBuffer) {
			OutputDebugStringA(static_cast<char*>(errorBuffer->GetBufferPointer()));
		}
		return false;
	}
	return true;
}

/**
* データをGPUにアップロードする
*
* @param buffer アップロード先のリソース情報を格納するオブジェクト.
* @param uploadBuffer アップロード用の中間バッファの情報を格納するオブジェクト.
*                     アップロード完了まで保持しなければならない.
* @param d3dStuff Direct3D管理オブジェクト.
*                 この関数はDirect3DStuffが保持するデバイスとコマンドリストを使用して、データのアップロードを行う.
* @param desc 作成するリソースの詳細情報のアドレス.
* @param data アップロードするデータのアドレス.
* @param dataSize アップロードするデータのバイト数.
*
* @retval true アップロード成功.
*              bufferのGetGPUAddress()によって、アップロード先のアドレスを得ることが出来る.
*              実際にアップロードを行うには、コマンドリストをコマンドキューに入れて実行する必要があることに注意.
* @retval false アップロード失敗.
*               bufferは不完全な状態にある. 速やかに破棄すること.
*/
bool UploadToGpuMemory(ComPtr<ID3D12Resource>& buffer, ComPtr<ID3D12Resource>& uploadBuffer, ComPtr<ID3D12Device>& device, ComPtr<ID3D12GraphicsCommandList>& commandList, const D3D12_RESOURCE_DESC* desc, const void* data, size_t dataSize, int rowPitch, int slicePitch, D3D12_RESOURCE_STATES finishState, const wchar_t* bufferName = nullptr)
{
	HRESULT hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		desc,
		D3D12_RESOURCE_STATE_COPY_DEST,
		nullptr,
		IID_PPV_ARGS(buffer.GetAddressOf())
	);
	if (FAILED(hr)) {
		return false;
	}
	if (bufferName) {
		buffer->SetName(bufferName);
	}
	hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer(dataSize),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(uploadBuffer.GetAddressOf())
	);
	if (FAILED(hr)) {
		return false;
	}
	uploadBuffer->SetName((std::wstring(bufferName) + L" Uplaod Heap").c_str());
	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData = data;
	vertexData.RowPitch = rowPitch;
	vertexData.SlicePitch = slicePitch;
	if (UpdateSubresources<1>(commandList.Get(), buffer.Get(), uploadBuffer.Get(), 0, 0, 1, &vertexData) == 0) {
		return false;
	}
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, finishState));
	return true;
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

#if !defined(NDEBUG)
	// Enable the D3D12 debug layer.
	{
		ComPtr<ID3D12Debug> debugController;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
			debugController->EnableDebugLayer();
		}
	}
#endif
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

	// DS用のデスクリプタヒープ及びデスクリプタを作成.
	D3D12_DESCRIPTOR_HEAP_DESC dsvDesc = {};
	dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDesc.NumDescriptors = 1;
	dsvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = d3dStuff.device->CreateDescriptorHeap(&dsvDesc, IID_PPV_ARGS(d3dStuff.dsvDescriptorHeap.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}
	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;
	D3D12_CLEAR_VALUE dsClearValue = {};
	dsClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	dsClearValue.DepthStencil.Depth = 1.0f;
	dsClearValue.DepthStencil.Stencil = 0;
	hr = d3dStuff.device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, d3dStuff.width, d3dStuff.height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&dsClearValue,
		IID_PPV_ARGS(d3dStuff.depthStencilbuffer.GetAddressOf())
	);
	if (FAILED(hr)) {
		return false;
	}
	d3dStuff.device->CreateDepthStencilView(d3dStuff.depthStencilbuffer.Get(), &depthStencilDesc, d3dStuff.dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// 定数バッファを作成.
	DirectX::XMStoreFloat4x4(&d3dStuff.cbPerObject.wvpMatrix, DirectX::XMMatrixIdentity());
	for (int i = 0; i < d3dStuff.frameBufferCount; ++i) {
		hr = d3dStuff.device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(AlignedConstantBufferSize * d3dStuff.objectCount),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(d3dStuff.cbvUploadHeapList[i].GetAddressOf())
		);
		if (FAILED(hr)) {
			return false;
		}
		d3dStuff.cbvUploadHeapList[i]->SetName(L"CBV Upload Heap");
		D3D12_RANGE  cbvRange = { 0, 0 };
		hr = d3dStuff.cbvUploadHeapList[i]->Map(0, &cbvRange, &d3dStuff.cbvHeapBegin[i]);
		if (FAILED(hr)) {
			return false;
		}
		memcpy(d3dStuff.cbvHeapBegin[i], &d3dStuff.cbPerObject, sizeof(d3dStuff.cbPerObject));
		memcpy(static_cast<uint8_t*>(d3dStuff.cbvHeapBegin[i]) + AlignedConstantBufferSize, &d3dStuff.cbPerObject, sizeof(d3dStuff.cbPerObject));
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
	hr = d3dStuff.device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, d3dStuff.commandAllocator[0].Get(), nullptr, IID_PPV_ARGS(d3dStuff.prologueCommandList.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}
	hr = d3dStuff.prologueCommandList->Close();
	if (FAILED(hr)) {
		d3dStuff.running = false;
	}
	hr = d3dStuff.device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, d3dStuff.commandAllocator[0].Get(), nullptr, IID_PPV_ARGS(d3dStuff.epilogueCommandList.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}
	hr = d3dStuff.epilogueCommandList->Close();
	if (FAILED(hr)) {
		d3dStuff.running = false;
	}
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
	// ルートパラメータのShaderVisibilityは適切に設定する必要がある.
	// ルートシグネチャが正しく設定されていない場合でも、シグネチャの作成には成功することがある.
	// しかしその場合、PSO作成時にエラーが発生する.
	{
		D3D12_DESCRIPTOR_RANGE descRange[1] = {};
		descRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descRange[0].NumDescriptors = 1;
		descRange[0].BaseShaderRegister = 0;
		descRange[0].RegisterSpace = 0;
		descRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		D3D12_ROOT_PARAMETER rootParameterList[2];
		rootParameterList[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameterList[0].Descriptor.RegisterSpace = 0;
		rootParameterList[0].Descriptor.ShaderRegister = 0;
		rootParameterList[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		rootParameterList[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameterList[1].DescriptorTable.NumDescriptorRanges = _countof(descRange);
		rootParameterList[1].DescriptorTable.pDescriptorRanges = descRange;
		rootParameterList[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		D3D12_STATIC_SAMPLER_DESC sampler[1] = {};
		sampler[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		sampler[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
		sampler[0].MipLODBias = 0;
		sampler[0].MaxAnisotropy = 0;
		sampler[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		sampler[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
		sampler[0].MinLOD = 0.0f;
		sampler[0].MaxLOD = D3D12_FLOAT32_MAX;
		sampler[0].ShaderRegister = 0;
		sampler[0].RegisterSpace = 0;
		sampler[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		D3D12_ROOT_SIGNATURE_DESC rsDesc = {
			_countof(rootParameterList),
			rootParameterList,
			_countof(sampler),
			sampler,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
			D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS
		};
		hr = D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &d3dStuff.signatureBlob, nullptr);
		if (FAILED(hr)) {
			return false;
		}
		hr = d3dStuff.device->CreateRootSignature(0, d3dStuff.signatureBlob->GetBufferPointer(), d3dStuff.signatureBlob->GetBufferSize(), IID_PPV_ARGS(d3dStuff.rootSignature.GetAddressOf()));
		if (FAILED(hr)) {
			return false;
		}
	}

	// 頂点シェーダを作成.
	if (!LoadShader(L"Res/VertexShader.hlsl", "vs_5_0", d3dStuff.vertexShaderBlob)) {
		return false;
	}
	// ピクセルシェーダを作成.
	if (!LoadShader(L"Res/PixelShader.hlsl", "ps_5_0", d3dStuff.pixelShaderBlob)) {
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
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
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
	ComPtr<ID3D12Resource> vbUploadHeap;
	if (!UploadToGpuMemory(d3dStuff.vertexBuffer, vbUploadHeap, d3dStuff.device, d3dStuff.commandList, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertexList)), vertexList, sizeof(vertexList), sizeof(vertexList), sizeof(vertexList), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, L"Vertex Buffer")) {
		return false;
	}
	d3dStuff.vertexBufferView.BufferLocation = d3dStuff.vertexBuffer->GetGPUVirtualAddress();
	d3dStuff.vertexBufferView.StrideInBytes = sizeof(Vertex);
	d3dStuff.vertexBufferView.SizeInBytes = sizeof(vertexList);

	// インデックスバッファを作成.
	ComPtr<ID3D12Resource> ibUploadHeap;
	if (!UploadToGpuMemory(d3dStuff.indexBuffer, ibUploadHeap, d3dStuff.device, d3dStuff.commandList, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(indexList)), indexList, sizeof(indexList), sizeof(indexList), sizeof(indexList), D3D12_RESOURCE_STATE_INDEX_BUFFER, L"Index Buffer")) {
		return false;
	}
	d3dStuff.indexBufferView.BufferLocation = d3dStuff.indexBuffer->GetGPUVirtualAddress();
	d3dStuff.indexBufferView.SizeInBytes = sizeof(indexList);
	d3dStuff.indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	// テクスチャを読み込む.
	Texture::Loader textureLoader;
	D3D12_RESOURCE_DESC textureDesc;
	int imageBytesPerRow;
	std::vector<uint8_t> imageData;
	if (!textureLoader.LoadFromFile(L"Res/rock_s.png", imageData, textureDesc, imageBytesPerRow)) {
		return false;
	}
	UINT64 textureHeapSize;
	d3dStuff.device->GetCopyableFootprints(&textureDesc, 0, 1, 0, nullptr, nullptr, nullptr, &textureHeapSize);
	ComPtr<ID3D12Resource> textureUploadHeap;
	if (!UploadToGpuMemory(d3dStuff.textureBuffer, textureUploadHeap, d3dStuff.device, d3dStuff.commandList, &textureDesc, imageData.data(), textureHeapSize, imageBytesPerRow, imageBytesPerRow * textureDesc.Height, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, L"Textre Buffer")) {
		return false;
	}

	D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
	cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvHeapDesc.NumDescriptors = 2;
	cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	hr = d3dStuff.device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(d3dStuff.srvDescriptorHeap.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = textureDesc.Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	d3dStuff.device->CreateShaderResourceView(d3dStuff.textureBuffer.Get(), &srvDesc, d3dStuff.srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// フォントを読み込む.
	if (!LoadFont(d3dStuff.fontInfo, L"Res/ArialBlack.fnt", static_cast<float>(d3dStuff.width), static_cast<float>(d3dStuff.height))) {
		return false;
	}
	D3D12_RESOURCE_DESC fontTextureDesc;
	int fontImageBytesPerRow;
	std::vector<uint8_t> fontImageData;
	if (!textureLoader.LoadFromFile((std::wstring(L"Res/") + d3dStuff.fontInfo.fontImage).c_str(), fontImageData, fontTextureDesc, fontImageBytesPerRow)) {
		return false;
	}
	UINT64 fontTextureHeapSize;
	d3dStuff.device->GetCopyableFootprints(&fontTextureDesc, 0, 1, 0, nullptr, nullptr, nullptr, &fontTextureHeapSize);
	ComPtr<ID3D12Resource> fontTextureUploadHeap;
	if (!UploadToGpuMemory(d3dStuff.fontInfo.textureBuffer, fontTextureUploadHeap, d3dStuff.device, d3dStuff.commandList, &fontTextureDesc, fontImageData.data(), fontTextureHeapSize, fontImageBytesPerRow, fontImageBytesPerRow * fontTextureDesc.Height, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, L"Font Textre Buffer")) {
		return false;
	}
	D3D12_SHADER_RESOURCE_VIEW_DESC fontSrvDesc = {};
	fontSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	fontSrvDesc.Format = fontTextureDesc.Format;
	fontSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	fontSrvDesc.Texture2D.MipLevels = 1;
	d3dStuff.srvDescriptorSize = d3dStuff.device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	d3dStuff.fontInfo.srvHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(d3dStuff.srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart(), 1, d3dStuff.srvDescriptorSize);
	CD3DX12_CPU_DESCRIPTOR_HANDLE srvHandle(d3dStuff.srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), 1, d3dStuff.srvDescriptorSize);
	d3dStuff.device->CreateShaderResourceView(d3dStuff.fontInfo.textureBuffer.Get(), &fontSrvDesc, srvHandle);

	// ここまでに積まれたコマンドを実行.
	hr = d3dStuff.commandList->Close();
	if (FAILED(hr)) {
		return false;
	}

	UploadBufferList uploadBufferList;
	if (!d3dStuff.fontRenderer.Init(d3dStuff.device, uploadBufferList)) {
		return false;
	}

	ID3D12CommandList* commandListArray[] = { d3dStuff.commandList.Get(), d3dStuff.fontRenderer.GetCommandList() };
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

	DirectX::XMStoreFloat4x4(
		&d3dStuff.matProjection,
		DirectX::XMMatrixPerspectiveFovLH(
			45.0f * 3.14f / 180.0f,
			static_cast<float>(d3dStuff.width) / static_cast<float>(d3dStuff.height),
			0.1f,
			1000.0f
		)
	);
	d3dStuff.cameraPos = DirectX::XMFLOAT4(0.0f, 2.0f, -4.0f, 0.0f);
	d3dStuff.cameraTarget = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	d3dStuff.cameraUp = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
	DirectX::XMStoreFloat4x4(
		&d3dStuff.matView,
		DirectX::XMMatrixLookAtLH(
			DirectX::XMLoadFloat4(&d3dStuff.cameraPos),
			DirectX::XMLoadFloat4(&d3dStuff.cameraTarget),
			DirectX::XMLoadFloat4(&d3dStuff.cameraUp)
		)
	);

	d3dStuff.objectState[0].pos = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	DirectX::XMStoreFloat4x4(&d3dStuff.objectState[0].matRot, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(
		&d3dStuff.objectState[0].matWorld,
		DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat4(&d3dStuff.objectState[0].pos))
	);

	d3dStuff.objectState[1].pos = DirectX::XMFLOAT4(1.5f, 0.0f, 0.0f, 0.0f);
	DirectX::XMStoreFloat4x4(&d3dStuff.objectState[1].matRot, DirectX::XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(
		&d3dStuff.objectState[1].matWorld,
		DirectX::XMMatrixTranslationFromVector(
			DirectX::XMVectorAdd(
				DirectX::XMLoadFloat4(&d3dStuff.objectState[1].pos),
				DirectX::XMLoadFloat4(&d3dStuff.objectState[0].pos)
			)
		)
	);

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
	CoInitialize(nullptr);
	Texture::Loader::Initialize();
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
	Texture::Loader::Destroy();
	CoUninitialize();
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

	hr = d3dStuff.prologueCommandList->Reset(d3dStuff.commandAllocator[d3dStuff.frameIndex].Get(), d3dStuff.pso.Get());
	if (FAILED(hr)) {
		d3dStuff.running = false;
	}
	d3dStuff.prologueCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(d3dStuff.renderTargetList[d3dStuff.frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	hr = d3dStuff.prologueCommandList->Close();
	if (FAILED(hr)) {
		d3dStuff.running = false;
	}

	hr = d3dStuff.epilogueCommandList->Reset(d3dStuff.commandAllocator[d3dStuff.frameIndex].Get(), d3dStuff.pso.Get());
	if (FAILED(hr)) {
		d3dStuff.running = false;
	}
	d3dStuff.epilogueCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(d3dStuff.renderTargetList[d3dStuff.frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	hr = d3dStuff.epilogueCommandList->Close();
	if (FAILED(hr)) {
		d3dStuff.running = false;
	}

	hr = d3dStuff.commandList->Reset(d3dStuff.commandAllocator[d3dStuff.frameIndex].Get(), d3dStuff.pso.Get());
	if (FAILED(hr)) {
		d3dStuff.running = false;
	}

	// コマンドを積んでいく.
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3dStuff.rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += d3dStuff.frameIndex * d3dStuff.rtvDescriptorSize;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = d3dStuff.dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dStuff.commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
	static const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	d3dStuff.commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	d3dStuff.commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// 頂点を描画.
	d3dStuff.commandList->SetGraphicsRootSignature(d3dStuff.rootSignature.Get());
	ID3D12DescriptorHeap* heapList[] = { d3dStuff.srvDescriptorHeap.Get() };
	d3dStuff.commandList->SetDescriptorHeaps(_countof(heapList), heapList);
	d3dStuff.commandList->SetGraphicsRootDescriptorTable(1, d3dStuff.srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
	d3dStuff.commandList->RSSetViewports(1, &d3dStuff.viewport);
	d3dStuff.commandList->RSSetScissorRects(1, &d3dStuff.scissorRect);
	d3dStuff.commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	d3dStuff.commandList->IASetVertexBuffers(0, 1, &d3dStuff.vertexBufferView);
	d3dStuff.commandList->IASetIndexBuffer(&d3dStuff.indexBufferView);

	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = d3dStuff.cbvUploadHeapList[d3dStuff.frameIndex]->GetGPUVirtualAddress();
	for (int i = 0; i < d3dStuff.objectCount; ++i) {
		d3dStuff.commandList->SetGraphicsRootConstantBufferView(0, gpuAddress);
		d3dStuff.commandList->DrawIndexedInstanced(_countof(indexList), 1, 0, 0, 0);
		gpuAddress += AlignedConstantBufferSize;
	}

	hr = d3dStuff.commandList->Close();
	if (FAILED(hr)) {
		d3dStuff.running = false;
	}

	if (!d3dStuff.fontRenderer.Begin(&d3dStuff.fontInfo, d3dStuff.frameIndex, &rtvHandle, &dsvHandle, &d3dStuff.viewport, &d3dStuff.scissorRect)) {
		d3dStuff.running = false;
	}
	d3dStuff.fontRenderer.Draw(L"FontTest", DirectX::XMFLOAT2(0.02f, 0.01f), DirectX::XMFLOAT2(2.0f, 2.0f));
	if (!d3dStuff.fontRenderer.End()) {
		d3dStuff.running = false;
	}

	// 描画開始.
	ID3D12CommandList* commandListArray[] = { d3dStuff.prologueCommandList.Get(), d3dStuff.commandList.Get(), d3dStuff.fontRenderer.GetCommandList(), d3dStuff.epilogueCommandList.Get() };
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
void Update(Direct3DStuff& d3dStuff)
{
	DirectX::XMMATRIX viewMat = DirectX::XMLoadFloat4x4(&d3dStuff.matView);
	DirectX::XMMATRIX projMat = DirectX::XMLoadFloat4x4(&d3dStuff.matProjection);

	{
		DirectX::XMMATRIX rotX = DirectX::XMMatrixRotationX(0.001f);
		DirectX::XMMATRIX rotY = DirectX::XMMatrixRotationY(0.002f);
		DirectX::XMMATRIX rotZ = DirectX::XMMatrixRotationZ(0.003f);
		DirectX::XMMATRIX rot = DirectX::XMLoadFloat4x4(&d3dStuff.objectState[0].matRot) * rotX * rotY * rotZ;
		DirectX::XMMATRIX trans = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat4(&d3dStuff.objectState[0].pos));
		DirectX::XMMATRIX world = rot * trans;
		DirectX::XMStoreFloat4x4(&d3dStuff.objectState[0].matRot, rot);
		DirectX::XMStoreFloat4x4(&d3dStuff.objectState[0].matWorld, world);

		DirectX::XMMATRIX wvpMat = DirectX::XMLoadFloat4x4(&d3dStuff.objectState[0].matWorld) * viewMat * projMat;
		DirectX::XMMATRIX transposed = XMMatrixTranspose(wvpMat);
		DirectX::XMStoreFloat4x4(&d3dStuff.cbPerObject.wvpMatrix, transposed);
		memcpy(
			static_cast<uint8_t*>(d3dStuff.cbvHeapBegin[d3dStuff.frameIndex]),
			&d3dStuff.cbPerObject,
			sizeof(d3dStuff.cbPerObject)
		);
	}
	{
		DirectX::XMMATRIX rotX = DirectX::XMMatrixRotationX(0.003f);
		DirectX::XMMATRIX rotY = DirectX::XMMatrixRotationY(0.002f);
		DirectX::XMMATRIX rotZ = DirectX::XMMatrixRotationZ(0.001f);
		DirectX::XMMATRIX rot = rotZ * DirectX::XMLoadFloat4x4(&d3dStuff.objectState[1].matRot) * (rotX * rotY);
		DirectX::XMMATRIX trans = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat4(&d3dStuff.objectState[1].pos));
		DirectX::XMMATRIX transParent = DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat4(&d3dStuff.objectState[0].pos));
		DirectX::XMMATRIX scale = DirectX::XMMatrixScaling(0.5f, 0.5f, 0.5f);
		DirectX::XMMATRIX world = scale * trans * rot * transParent;
		DirectX::XMStoreFloat4x4(&d3dStuff.objectState[1].matRot, rot);
		DirectX::XMStoreFloat4x4(&d3dStuff.objectState[1].matWorld, world);

		DirectX::XMMATRIX wvpMat = DirectX::XMLoadFloat4x4(&d3dStuff.objectState[1].matWorld) * viewMat * projMat;
		DirectX::XMMATRIX transposed = XMMatrixTranspose(wvpMat);
		DirectX::XMStoreFloat4x4(&d3dStuff.cbPerObject.wvpMatrix, transposed);
		memcpy(
			static_cast<uint8_t*>(d3dStuff.cbvHeapBegin[d3dStuff.frameIndex]) + AlignedConstantBufferSize,
			&d3dStuff.cbPerObject,
			sizeof(d3dStuff.cbPerObject)
		);
	}
}