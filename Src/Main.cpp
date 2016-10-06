/**
* @file Main.cpp
*/
#include "stdafx.h"
#include "Texture.h"
#include "Sprite.h"
#include "Engine.h"
#include <algorithm>
#include <fstream>
#include <string>

using Microsoft::WRL::ComPtr;

/// テクスチャ名.
const wchar_t textureName[] = L"Res/rock_s.png";
const wchar_t spriteTextureName[] = L"Res/playerunit.png";

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

	Engine engine;

	ComPtr<ID3D12PipelineState> pso;
	ComPtr<ID3DBlob> signatureBlob;
	ComPtr<ID3D12RootSignature> rootSignature;

	ComPtr<ID3D12Resource> vertexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	ComPtr<ID3D12Resource> indexBuffer;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;

	ComPtr<ID3DBlob> vertexShaderBlob;
	ComPtr<ID3DBlob> pixelShaderBlob;

	ConstantBuffer cbPerObject;

	DirectX::XMFLOAT4X4 matProjection;
	DirectX::XMFLOAT4X4 matView;
	DirectX::XMFLOAT4 cameraPos;
	DirectX::XMFLOAT4 cameraTarget;
	DirectX::XMFLOAT4 cameraUp;

	ObjectState objectState[objectCount];
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool Initialize(Direct3DStuff&, int, int, bool, HWND);
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
	if (!Initialize(d3dStuff, width, height, fullScreen, hwnd)) {
		MessageBox(nullptr, L"DirectXの初期化に失敗", L"エラー", MB_OK | MB_ICONERROR);
		Finalize(d3dStuff);
		return 0;
	}

	MSG msg = { 0 };
	while (d3dStuff.engine.IsRunning()) {
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
* Direct3Dの初期化.
*
* @retval true 初期化成功.
* @retval false 初期化失敗.
*/
bool Init3D(Direct3DStuff& d3dStuff, int width, int height, bool fullscreen, HWND hwnd)
{
	HRESULT hr;

	if (!d3dStuff.engine.Initialize(hwnd, width, height, fullscreen)) {
		return false;
	}

	DirectX::XMStoreFloat4x4(&d3dStuff.cbPerObject.wvpMatrix, DirectX::XMMatrixIdentity());
	for (int i = 0; i < d3dStuff.frameBufferCount; ++i) {
		uint8_t* p = d3dStuff.engine.GetConstantBufferAddress(i);
		memcpy(p, &d3dStuff.cbPerObject, sizeof(d3dStuff.cbPerObject));
		memcpy(p + AlignedConstantBufferSize, &d3dStuff.cbPerObject, sizeof(d3dStuff.cbPerObject));
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
		hr = d3dStuff.engine.GetDevice()->CreateRootSignature(0, d3dStuff.signatureBlob->GetBufferPointer(), d3dStuff.signatureBlob->GetBufferSize(), IID_PPV_ARGS(d3dStuff.rootSignature.GetAddressOf()));
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
	if (d3dStuff.engine.IsWarpDevice()) {
		psoDesc.Flags = D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG;
	}
	hr = d3dStuff.engine.GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(d3dStuff.pso.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}

	ResourceLoader resourceLoader = ResourceLoader::Open(d3dStuff.engine.GetDevice());

	// 頂点バッファを作成.
	if (!resourceLoader.Upload(d3dStuff.vertexBuffer, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertexList)), vertexList, sizeof(vertexList), sizeof(vertexList), sizeof(vertexList), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER)) {
		return false;
	}
	d3dStuff.vertexBufferView.BufferLocation = d3dStuff.vertexBuffer->GetGPUVirtualAddress();
	d3dStuff.vertexBufferView.StrideInBytes = sizeof(Vertex);
	d3dStuff.vertexBufferView.SizeInBytes = sizeof(vertexList);

	// インデックスバッファを作成.
	if (!resourceLoader.Upload(d3dStuff.indexBuffer, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(indexList)), indexList, sizeof(indexList), sizeof(indexList), sizeof(indexList), D3D12_RESOURCE_STATE_INDEX_BUFFER)) {
		return false;
	}
	d3dStuff.indexBufferView.BufferLocation = d3dStuff.indexBuffer->GetGPUVirtualAddress();
	d3dStuff.indexBufferView.SizeInBytes = sizeof(indexList);
	d3dStuff.indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	// テクスチャを読み込む.
	if (!d3dStuff.engine.LoadTexture(resourceLoader, textureName)) {
		return false;
	}
	if (!d3dStuff.engine.LoadTexture(resourceLoader, spriteTextureName)) {
		return false;
	}

	// ここまでに積まれたコマンドを実行.
	if (!resourceLoader.Close()) {
		return false;
	}
	ID3D12CommandList* commandListArray[] = { resourceLoader.GetCommandList() };
	if (!d3dStuff.engine.ExecuteCommandList(_countof(commandListArray), commandListArray)) {
		return false;
	}

	DirectX::XMStoreFloat4x4(
		&d3dStuff.matProjection,
		DirectX::XMMatrixPerspectiveFovLH(
			45.0f * 3.14f / 180.0f,
			static_cast<float>(d3dStuff.engine.GetWidth()) / static_cast<float>(d3dStuff.engine.GetHeight()),
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

	return true;
}

/**
* DirectXの初期化.
*
* @retval true 初期化成功.
* @retval false 初期化失敗.
*/
bool Initialize(Direct3DStuff& d3dStuff, int w, int h, bool fs, HWND hw)
{
	CoInitialize(nullptr);
	Texture::Loader::Initialize();
	if (!Init3D(d3dStuff, w, h, fs, hw)) {
		return false;
	}
	return true;
}

/**
* DirectXの破棄.
*/
void Finalize(Direct3DStuff& d3dStuff)
{
	d3dStuff.engine.Finalize();
	Texture::Loader::Destroy();
	CoUninitialize();
}

/**
* レンダリングパイプラインの更新.
*/
void Render(Direct3DStuff& d3dStuff)
{
	d3dStuff.engine.BeginRender(d3dStuff.pso.Get());

	// 頂点を描画.
	ID3D12GraphicsCommandList* commandList = d3dStuff.engine.GetCommandList();
	commandList->SetGraphicsRootSignature(d3dStuff.rootSignature.Get());
	ID3D12DescriptorHeap* heapList[] = { d3dStuff.engine.GetDescriptorHeap() };
	commandList->SetDescriptorHeaps(_countof(heapList), heapList);
	commandList->SetGraphicsRootDescriptorTable(1, d3dStuff.engine.GetTextureHandle(textureName));
	commandList->RSSetViewports(1, &d3dStuff.engine.GetViewport());
	commandList->RSSetScissorRects(1, &d3dStuff.engine.GetScissorRect());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &d3dStuff.vertexBufferView);
	commandList->IASetIndexBuffer(&d3dStuff.indexBufferView);
	D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = d3dStuff.engine.GetCurrentCBVAddress();
	for (int i = 0; i < d3dStuff.objectCount; ++i) {
		commandList->SetGraphicsRootConstantBufferView(0, gpuAddress);
		commandList->DrawIndexedInstanced(_countof(indexList), 1, 0, 0, 0);
		gpuAddress += AlignedConstantBufferSize;
	}
	HRESULT hr = commandList->Close();
	if (FAILED(hr)) {
		d3dStuff.engine.StopRunning();
	}

	d3dStuff.engine.ClearSprite();
	const SpriteCell sc = {
		DirectX::XMFLOAT2(0.0f, 0.0f),
		DirectX::XMFLOAT2(0.125f, 0.125f),
		DirectX::XMFLOAT2(32.0f / d3dStuff.engine.GetWidth(), 32.0f / d3dStuff.engine.GetHeight())
	};
	d3dStuff.engine.AddSprite(sc, d3dStuff.engine.GetTextureHandle(spriteTextureName), DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f));

	// フォントを描画.
	d3dStuff.engine.ClearAllText();
	d3dStuff.engine.AddText(L"FontTest", DirectX::XMFLOAT2(0.02f, 0.01f), DirectX::XMFLOAT2(2.0f, 2.0f));

	d3dStuff.engine.EndRender(0, nullptr);
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
			d3dStuff.engine.GetCurrentCBVHeap(),
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
			static_cast<uint8_t*>(d3dStuff.engine.GetCurrentCBVHeap()) + AlignedConstantBufferSize,
			&d3dStuff.cbPerObject,
			sizeof(d3dStuff.cbPerObject)
		);
	}
}