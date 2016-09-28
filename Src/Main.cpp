/**
* @file Main.cpp
*/
#include "stdafx.h"
#include "Texture.h"
#include <wrl/client.h>
#include <algorithm>
#include <fstream>
#include <string>

using Microsoft::WRL::ComPtr;

/**
* Vertex�\���̂̃��C�A�E�g.
*/
D3D12_INPUT_ELEMENT_DESC vertexLayout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

/**
* ���_�f�[�^�^.
*/
struct Vertex {
	constexpr Vertex(float x, float y, float z, float r, float g, float b, float a, float u, float v) : pos(x, y, z), col(r, g, b, a), texCoord(u, v) {}
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT4 col;
	DirectX::XMFLOAT2 texCoord;
};

/**
* ���_�f�[�^�z��.
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
* �C���f�b�N�X�f�[�^�z��.
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
* �t�H���g�`��p���_�f�[�^���C�A�E�g.
*/
D3D12_INPUT_ELEMENT_DESC fontVertexLayout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

/**
* �t�H���g�`��p���_�f�[�^�^.
*/
struct FontVertex {
	constexpr FontVertex(float x, float y, float r, float g, float b, float a, float u, float v) : pos(x, y), col(r, g, b, a), texCoord(u, v) {}
	DirectX::XMFLOAT2 pos;
	DirectX::XMFLOAT4 col;
	DirectX::XMFLOAT2 texCoord;
};

/**
* �t�H���g�L�����N�^�f�[�^�^.
*/
struct FontChar {
	int id; ///< �����̃��j�R�[�h.
	DirectX::XMFLOAT2 uv; ///< �e�N�X�`����̍�����W.
	DirectX::XMFLOAT2 tsize; ///< �e�N�X�`����̏c���T�C�Y.
	DirectX::XMFLOAT2 ssize; ///< �X�N���[�����W��̏c���T�C�Y.
	DirectX::XMFLOAT2 offset; ///< �J�[�\���ʒu������ۂ̕\���ʒu�ւ̃I�t�Z�b�g.
	float xadvance; ///< ���̃L�����N�^�̕\���J�n�ʒu�ւ̉E�����̃I�t�Z�b�g.
};

/**
* �t�H���g�̃J�[�j���O���^.
*/
struct FontKerning
{
	int first; ///< �����̕����̃��j�R�[�h.
	int second; ///< �E���̕����̃��j�R�[�h.
	float amount; ///< �E���̕�����X�\�����W�ɉ��Z����ړ���.
};

/**
* �t�H���g���^.
*/
struct Font {
	std::wstring name;
	std::wstring fontImage;
	int size;
	float lineHeight;
	float baseHeight;
	int textureWidth;
	int textureHeight;
	std::vector<FontChar> charList;
	std::vector<FontKerning> kerningList;
	ComPtr<ID3D12Resource> textureBuffer;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandle;
	float leftPadding;
	float topPadding;
	float rightPadding;
	float bottomPadding;

	float GetKerning(wchar_t first, wchar_t second) const
	{
		auto result = std::find_if(
			kerningList.begin(),
			kerningList.end(),
			[first, second](const FontKerning& k) { return k.first == first && k.second == second; }
		);
		if (result != kerningList.end()) {
			return result->amount;
		}
		return 0.0f;
	}

	const FontChar* GetChar(wchar_t c) const
	{
		auto result = std::find_if(
			charList.begin(),
			charList.end(),
			[c](const FontChar& fontChar) { return fontChar.id == c; }
		);
		if (result != charList.end()) {
			return &*result;
		}
		return nullptr;
	}
};

/**
* �萔�o�b�t�@�^.
*
* DirectX 12�ł́A�萔�o�b�t�@�̃T�C�Y��256�o�C�g�P�ʂɂ��邱�Ƃ��v������Ă���.
*/
struct ConstantBuffer {
	DirectX::XMFLOAT4X4 wvpMatrix;
};
const size_t AlignedConstantBufferSize = (sizeof(ConstantBuffer) + 255) & ~255UL;

/**
* �I�u�W�F�N�g�p�f�[�^.
*/
struct ObjectState {
	DirectX::XMFLOAT4X4 matWorld;
	DirectX::XMFLOAT4X4 matRot;
	DirectX::XMFLOAT4 pos;
};

/**
* Direct3D�ŕK�v�ȃI�u�W�F�N�g���܂Ƃ߂�����.
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

	static const size_t maxFontCharacters = 1024;
	Font fontInfo;
	ComPtr<ID3DBlob> fontVertexShaderBlob;
	ComPtr<ID3DBlob> fontPixelShaderBlob;
	ComPtr<ID3D12PipelineState> fontPSO;
	ComPtr<ID3D12Resource> fontVertexBuffer[frameBufferCount];
	D3D12_VERTEX_BUFFER_VIEW fontVertexBufferView[frameBufferCount];
	ComPtr<ID3D12Resource> fontIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW fontIndexBufferView;
	void* fontVertexBufferGPUAddress[frameBufferCount];
};

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
bool Initialize(Direct3DStuff&);
void Finalize(Direct3DStuff&);
void Update(Direct3DStuff&);
void Render(Direct3DStuff&);
void WaitForPreviousFrame(Direct3DStuff&);
bool LoadFont(Font& font, const wchar_t* filename, float screenWidth, float screenHeight);
void DrawFont(Direct3DStuff&, const std::wstring& text, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 scale = { 1.0f, 1.0f }, DirectX::XMFLOAT4 col = { 1.0f, 1.0f, 1.0f, 1.0f });

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

	// D3D12��Ή��n�[�h�E�F�A�ł�WARP�f�o�C�X���g�����ƂɂȂ邪�AWARP�f�o�C�X�̓t���X�N���[�����[�h�ɑΉ����Ă��Ȃ�.
	// ���̂��߁A�X�N���[���S�̂𕢂��悤�ȉ��E�^�C�g���o�[�Ȃ��̃E�B���h�E����邱�Ƃő�p����.
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
* �V�F�[�_��ǂݍ���.
*
* @param filename �V�F�[�_�t�@�C����.
* @param target   �ΏۂƂ���V�F�[�_�o�[�W����.
* @param blob     �ǂݍ��񂾃V�F�[�_���i�[����Blob�I�u�W�F�N�g.
*
* @retval true �ǂݍ��ݐ���.
* @retval false �ǂݍ��ݎ��s.
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
* �f�[�^��GPU�ɃA�b�v���[�h����
*
* @param buffer �A�b�v���[�h��̃��\�[�X�����i�[����I�u�W�F�N�g.
* @param uploadBuffer �A�b�v���[�h�p�̒��ԃo�b�t�@�̏����i�[����I�u�W�F�N�g.
*                     �A�b�v���[�h�����܂ŕێ����Ȃ���΂Ȃ�Ȃ�.
* @param d3dStuff Direct3D�Ǘ��I�u�W�F�N�g.
*                 ���̊֐���Direct3DStuff���ێ�����f�o�C�X�ƃR�}���h���X�g���g�p���āA�f�[�^�̃A�b�v���[�h���s��.
* @param desc �쐬���郊�\�[�X�̏ڍ׏��̃A�h���X.
* @param data �A�b�v���[�h����f�[�^�̃A�h���X.
* @param dataSize �A�b�v���[�h����f�[�^�̃o�C�g��.
*
* @retval true �A�b�v���[�h����.
*              buffer��GetGPUAddress()�ɂ���āA�A�b�v���[�h��̃A�h���X�𓾂邱�Ƃ��o����.
*              ���ۂɃA�b�v���[�h���s���ɂ́A�R�}���h���X�g���R�}���h�L���[�ɓ���Ď��s����K�v�����邱�Ƃɒ���.
* @retval false �A�b�v���[�h���s.
*               buffer�͕s���S�ȏ�Ԃɂ���. ���₩�ɔj�����邱��.
*/
bool UploadToGpuMemory(ComPtr<ID3D12Resource>& buffer, ComPtr<ID3D12Resource>& uploadBuffer, Direct3DStuff& d3dStuff, const D3D12_RESOURCE_DESC* desc, const void* data, size_t dataSize, int rowPitch, int slicePitch, D3D12_RESOURCE_STATES finishState, const wchar_t* bufferName = nullptr)
{
	HRESULT hr = d3dStuff.device->CreateCommittedResource(
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
	hr = d3dStuff.device->CreateCommittedResource(
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
	if (UpdateSubresources<1>(d3dStuff.commandList.Get(), buffer.Get(), uploadBuffer.Get(), 0, 0, 1, &vertexData) == 0) {
		return false;
	}
	d3dStuff.commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, finishState));
	return true;
}

std::wstring GetValue(std::wifstream& fs)
{
	std::wstring tmp;
	std::getline(fs, tmp, L'=');
	wchar_t c = fs.get();
	if (c == L'"') {
		std::getline(fs, tmp, L'"');
	} else {
		tmp.clear();
		tmp.push_back(c);
		while (fs) {
			c = fs.get();
			if (c == L' ' || c == L'\n') {
				break;
			}
			tmp.push_back(c);
		}
	}
	return tmp;
}

int GetValueI(std::wifstream& fs)
{
	std::wstring tmp = GetValue(fs);
	return std::stoi(tmp);
}
float GetValueF(std::wifstream& fs)
{
	return static_cast<float>(GetValueI(fs));
}

/**
* �t�H���g��`�t�@�C����ǂݍ���.
*
* @param font �t�H���g��`�I�u�W�F�N�g.
* @param filename �t�H���g��`�t�@�C����.
* @param screenWidth �\���X�N���[���̉��s�N�Z����.
* @param screenHeight �\���X�N���[���̏c�s�N�Z����.
*
* @retval treu �ǂݍ��ݐ���.
* @retval false �ǂݍ��ݎ��s.
*/
bool LoadFont(Font& font, const wchar_t* filename, float screenWidth, float screenHeight)
{
	std::wifstream fs;
	fs.open(filename);

	std::wstring tmp;

	fs >> tmp;

	font.name = GetValue(fs);
	font.size = GetValueI(fs);

	fs >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp >> tmp;

	tmp = GetValue(fs);
	size_t startpos = tmp.find(L",") + 1;
	font.topPadding = static_cast<float>(std::stoi(tmp.substr(0, startpos))) / screenHeight;
	tmp = tmp.substr(startpos, tmp.size() - startpos);
	startpos = tmp.find(L",") + 1;
	font.rightPadding = static_cast<float>(std::stoi(tmp.substr(0, startpos))) / screenWidth;
	tmp = tmp.substr(startpos, tmp.size() - startpos);
	startpos = tmp.find(L",") + 1;
	font.bottomPadding = static_cast<float>(std::stoi(tmp.substr(0, startpos))) / screenHeight;
	font.leftPadding = static_cast<float>(std::stoi(tmp.substr(startpos, tmp.size() - startpos))) / screenWidth;

	fs >> tmp >> tmp;

	font.lineHeight = GetValueF(fs) / screenHeight;
	font.baseHeight = GetValueF(fs) / screenHeight;
	font.textureWidth = GetValueI(fs);
	font.textureHeight = GetValueI(fs);

	fs >> tmp >> tmp >> tmp >> tmp;

	font.fontImage = GetValue(fs);

	fs >> tmp;

	const int numCharacters = GetValueI(fs);
	font.charList.reserve(numCharacters);
	for (int i = 0; i < numCharacters; ++i) {
		FontChar c;
		fs >> tmp;
		c.id = GetValueI(fs);
		c.uv.x = GetValueF(fs) / font.textureWidth;
		c.uv.y = GetValueF(fs) / font.textureHeight;
		const float w = GetValueF(fs);
		c.ssize.x = w / screenWidth;
		c.tsize.x = w / font.textureWidth;
		const float h = GetValueF(fs);
		c.ssize.y = h / screenWidth;
		c.tsize.y = h / font.textureHeight;
		c.offset.x = GetValueF(fs) / screenWidth;
		c.offset.y = GetValueF(fs) / screenHeight;
		c.xadvance = GetValueF(fs) / screenWidth;

		fs >> tmp >> tmp;

		font.charList.push_back(c);
	}

	fs >> tmp;

	const int numKernings = GetValueI(fs);
	font.kerningList.reserve(numKernings);
	for (int i = 0; i < numKernings; ++i) {
		FontKerning k;
		fs >> tmp;
		k.first = GetValueI(fs);
		k.second = GetValueI(fs);
		k.amount = GetValueF(fs) / screenWidth;
		font.kerningList.push_back(k);
	}

	return true;
}

/**
* �t�H���g��`��.
*
* @param d3dStuff Direct3D�I�u�W�F�N�g.
* @param text �`�悷�镶����.
* @param pos �`��ʒu�̃X�N���[�����W.
* @param scale �`�悷��t�H���g�̑傫��.
* @param  col �`�悷��t�H���g�̐F.
*
* @retval treu �ǂݍ��ݐ���.
* @retval false �ǂݍ��ݎ��s.
*/
void DrawFont(Direct3DStuff& d3dStuff, const std::wstring& text, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 scale, DirectX::XMFLOAT4 col)
{
	d3dStuff.commandList->SetPipelineState(d3dStuff.fontPSO.Get());
	d3dStuff.commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	d3dStuff.commandList->IASetVertexBuffers(0, 1, &d3dStuff.fontVertexBufferView[d3dStuff.frameIndex]);
	d3dStuff.commandList->IASetIndexBuffer(&d3dStuff.fontIndexBufferView);
	d3dStuff.commandList->SetGraphicsRootDescriptorTable(1, d3dStuff.fontInfo.srvHandle);

	int numCharacters = 0;
	DirectX::XMFLOAT2 topLeftScreenPos(pos.x * 2.0f - 1.0f, (1.0f - pos.y) * 2.0f - 1.0f);
	DirectX::XMFLOAT2 curPos(topLeftScreenPos);
	DirectX::XMFLOAT2 padding((d3dStuff.fontInfo.leftPadding + d3dStuff.fontInfo.rightPadding) * 0.5f, (d3dStuff.fontInfo.topPadding + d3dStuff.fontInfo.bottomPadding) * 0.0f);

	FontVertex* v = static_cast<FontVertex*>(d3dStuff.fontVertexBufferGPUAddress[d3dStuff.frameIndex]);
	wchar_t lastChar = -1;
	for (int i = 0; i < text.size(); ++i) {
		if (numCharacters >= d3dStuff.maxFontCharacters) {
			break;
		}
		const wchar_t c = text[i];
		const FontChar* fc = d3dStuff.fontInfo.GetChar(c);
		if (!fc) {
			continue;
		}
		float kerning = 0.0f;
		if (i) {
			kerning = d3dStuff.fontInfo.GetKerning(lastChar, c);
		}
		// 0-3
		// |\|
		// 2-1
		v[0].pos.x = curPos.x + (fc->offset.x + kerning) * scale.x;
		v[0].pos.y = curPos.y - fc->offset.y * scale.y;
		v[0].texCoord = fc->uv;
		v[0].col = col;
		v[1].pos.x = curPos.x + (fc->offset.x + fc->ssize.x + kerning) * scale.x;
		v[1].pos.y = curPos.y - (fc->offset.y + fc->ssize.y) * scale.y;
		v[1].texCoord.x = fc->uv.x + fc->tsize.x;
		v[1].texCoord.y = fc->uv.y + fc->tsize.y;
		v[1].col = col;
		v[2].pos.x = curPos.x + (fc->offset.x + kerning) * scale.x;
		v[2].pos.y = curPos.y - (fc->offset.y + fc->ssize.y) * scale.y;
		v[2].texCoord.x = fc->uv.x;
		v[2].texCoord.y = fc->uv.y + fc->tsize.y;
		v[2].col = col;
		v[3].pos.x = curPos.x + (fc->offset.x + fc->ssize.x + kerning) * scale.x;
		v[3].pos.y = curPos.y - fc->offset.y * scale.y;
		v[3].texCoord.x = fc->uv.x + fc->tsize.x;
		v[3].texCoord.y = fc->uv.y;
		v[3].col = col;

		v += 4;
		curPos.x += (fc->xadvance - padding.x) * scale.x;
		++numCharacters;
		lastChar = c;
	}
	d3dStuff.commandList->DrawIndexedInstanced(numCharacters * 6, 1, 0, 0, 0);
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
		d3dStuff.warp = true;
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

	// DS�p�̃f�X�N���v�^�q�[�v�y�уf�X�N���v�^���쐬.
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

	// �萔�o�b�t�@���쐬.
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
	// �������ꂽ����̃R�}���h���X�g�̓��Z�b�g���Ăяo���ꂽ����Ɠ�����ԂɂȂ��Ă��邽�߁A�����ɃR�}���h�𑗂荞�ނ��Ƃ��o����.
	hr = d3dStuff.device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, d3dStuff.commandAllocator[0].Get(), nullptr, IID_PPV_ARGS(d3dStuff.commandList.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}

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

	// ���[�g�V�O�l�`�����쐬.
	// ���[�g�p�����[�^��ShaderVisibility�͓K�؂ɐݒ肷��K�v������.
	// ���[�g�V�O�l�`�����������ݒ肳��Ă��Ȃ��ꍇ�ł��A�V�O�l�`���̍쐬�ɂ͐������邱�Ƃ�����.
	// ���������̏ꍇ�APSO�쐬���ɃG���[����������.
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

	// ���_�V�F�[�_���쐬.
	if (!LoadShader(L"Res/VertexShader.hlsl", "vs_5_0", d3dStuff.vertexShaderBlob)) {
		return false;
	}
	// �s�N�Z���V�F�[�_���쐬.
	if (!LoadShader(L"Res/PixelShader.hlsl", "ps_5_0", d3dStuff.pixelShaderBlob)) {
		return false;
	}

	// �p�C�v���C���X�e�[�g�I�u�W�F�N�g(PSO)���쐬.
	// PSO�́A�����_�����O�p�C�v���C���̏�Ԃ�f�����A�ꊇ���ĕύX�ł���悤�ɓ������ꂽ.
	// PSO�ɂ���āA�����̃X�e�[�g�ɑ΂��Ă��ꂼ���ԕύX�R�}���h�𑗂炸�Ƃ��A�P��PSO��؂�ւ���R�}���h�𑗂邾���ōς�.
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

	// ���_�o�b�t�@���쐬.
	ComPtr<ID3D12Resource> vbUploadHeap;
	if (!UploadToGpuMemory(d3dStuff.vertexBuffer, vbUploadHeap, d3dStuff, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertexList)), vertexList, sizeof(vertexList), sizeof(vertexList), sizeof(vertexList), D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, L"Vertex Buffer")) {
		return false;
	}
	d3dStuff.vertexBufferView.BufferLocation = d3dStuff.vertexBuffer->GetGPUVirtualAddress();
	d3dStuff.vertexBufferView.StrideInBytes = sizeof(Vertex);
	d3dStuff.vertexBufferView.SizeInBytes = sizeof(vertexList);

	// �C���f�b�N�X�o�b�t�@���쐬.
	ComPtr<ID3D12Resource> ibUploadHeap;
	if (!UploadToGpuMemory(d3dStuff.indexBuffer, ibUploadHeap, d3dStuff, &CD3DX12_RESOURCE_DESC::Buffer(sizeof(indexList)), indexList, sizeof(indexList), sizeof(indexList), sizeof(indexList), D3D12_RESOURCE_STATE_INDEX_BUFFER, L"Index Buffer")) {
		return false;
	}
	d3dStuff.indexBufferView.BufferLocation = d3dStuff.indexBuffer->GetGPUVirtualAddress();
	d3dStuff.indexBufferView.SizeInBytes = sizeof(indexList);
	d3dStuff.indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	// �e�N�X�`����ǂݍ���.
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
	if (!UploadToGpuMemory(d3dStuff.textureBuffer, textureUploadHeap, d3dStuff, &textureDesc, imageData.data(), textureHeapSize, imageBytesPerRow, imageBytesPerRow * textureDesc.Height, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, L"Textre Buffer")) {
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

	// �t�H���g�`��pPSO���쐬.
	if (!LoadShader(L"Res/FontVertexShader.hlsl", "vs_5_0", d3dStuff.fontVertexShaderBlob)) {
		return false;
	}
	if (!LoadShader(L"Res/FontPixelShader.hlsl", "ps_5_0", d3dStuff.fontPixelShaderBlob)) {
		return false;
	}
	D3D12_GRAPHICS_PIPELINE_STATE_DESC fontPsoDesc = {};
	fontPsoDesc.pRootSignature = d3dStuff.rootSignature.Get();
	fontPsoDesc.VS.pShaderBytecode = d3dStuff.fontVertexShaderBlob->GetBufferPointer();
	fontPsoDesc.VS.BytecodeLength = d3dStuff.fontVertexShaderBlob->GetBufferSize();
	fontPsoDesc.PS.pShaderBytecode = d3dStuff.fontPixelShaderBlob->GetBufferPointer();
	fontPsoDesc.PS.BytecodeLength = d3dStuff.fontPixelShaderBlob->GetBufferSize();
	fontPsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	fontPsoDesc.SampleMask = 0xffffffff;
	fontPsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	fontPsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	fontPsoDesc.DepthStencilState.DepthEnable = FALSE;
	fontPsoDesc.InputLayout.pInputElementDescs = fontVertexLayout;
	fontPsoDesc.InputLayout.NumElements = _countof(fontVertexLayout);
	fontPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	fontPsoDesc.NumRenderTargets = 1;
	fontPsoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	fontPsoDesc.SampleDesc.Count = 1;
	fontPsoDesc.BlendState.AlphaToCoverageEnable = FALSE;
	fontPsoDesc.BlendState.IndependentBlendEnable = FALSE;
	fontPsoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
	fontPsoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	fontPsoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	fontPsoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	fontPsoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	fontPsoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	fontPsoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	fontPsoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	hr = d3dStuff.device->CreateGraphicsPipelineState(&fontPsoDesc, IID_PPV_ARGS(d3dStuff.fontPSO.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}

	// �t�H���g��ǂݍ���.
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
	if (!UploadToGpuMemory(d3dStuff.fontInfo.textureBuffer, fontTextureUploadHeap, d3dStuff, &fontTextureDesc, fontImageData.data(), fontTextureHeapSize, fontImageBytesPerRow, fontImageBytesPerRow * fontTextureDesc.Height, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, L"Font Textre Buffer")) {
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
	for (int i = 0; i < d3dStuff.frameBufferCount; ++i) {
		hr = d3dStuff.device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(d3dStuff.maxFontCharacters * sizeof(FontVertex)),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(d3dStuff.fontVertexBuffer[i].GetAddressOf())
		);
		if (FAILED(hr)) {
			return false;
		}
		d3dStuff.fontVertexBuffer[i]->SetName(L"Font Vertex Buffer");
		CD3DX12_RANGE range(0, 0);
		hr = d3dStuff.fontVertexBuffer[i]->Map(0, &range, &d3dStuff.fontVertexBufferGPUAddress[i]);
		if (FAILED(hr)) {
			return false;
		}
		d3dStuff.fontVertexBufferView[i].BufferLocation = d3dStuff.fontVertexBuffer[i]->GetGPUVirtualAddress();
		d3dStuff.fontVertexBufferView[i].StrideInBytes = sizeof(FontVertex);
		d3dStuff.fontVertexBufferView[i].SizeInBytes = d3dStuff.maxFontCharacters * sizeof(FontVertex);
	}
	ComPtr<ID3D12Resource> fontIBUploadHeap;
	std::vector<DWORD> fontIndexList;
	fontIndexList.resize(d3dStuff.maxFontCharacters * 6);
	for (int i = 0; i < d3dStuff.maxFontCharacters; ++i) {
		fontIndexList[i * 6 + 0] = i * 4 + 0;
		fontIndexList[i * 6 + 1] = i * 4 + 1;
		fontIndexList[i * 6 + 2] = i * 4 + 2;
		fontIndexList[i * 6 + 3] = i * 4 + 0;
		fontIndexList[i * 6 + 4] = i * 4 + 3;
		fontIndexList[i * 6 + 5] = i * 4 + 1;
	}
	const size_t fontIndexListSize = d3dStuff.maxFontCharacters * 6 * sizeof(DWORD);
	if (!UploadToGpuMemory(d3dStuff.fontIndexBuffer, fontIBUploadHeap, d3dStuff, &CD3DX12_RESOURCE_DESC::Buffer(fontIndexListSize), fontIndexList.data(), fontIndexListSize, fontIndexListSize, fontIndexListSize, D3D12_RESOURCE_STATE_INDEX_BUFFER, L"Font Index Buffer")) {
		return false;
	}
	d3dStuff.fontIndexBufferView.BufferLocation = d3dStuff.fontIndexBuffer->GetGPUVirtualAddress();
	d3dStuff.fontIndexBufferView.SizeInBytes = fontIndexListSize;
	d3dStuff.fontIndexBufferView.Format = DXGI_FORMAT_R32_UINT;

	// �����܂łɐς܂ꂽ�R�}���h�����s.
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
* DirectX�̏�����.
*
* @retval true ����������.
* @retval false ���������s.
*/
bool Initialize(Direct3DStuff& d3dStuff)
{
	CoInitialize(nullptr);
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
	CoUninitialize();
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
	hr = d3dStuff.commandList->Reset(d3dStuff.commandAllocator[d3dStuff.frameIndex].Get(), d3dStuff.pso.Get());
	if (FAILED(hr)) {
		d3dStuff.running = false;
	}

	// �R�}���h��ς�ł���.
	d3dStuff.commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(d3dStuff.renderTargetList[d3dStuff.frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = d3dStuff.rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += d3dStuff.frameIndex * d3dStuff.rtvDescriptorSize;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = d3dStuff.dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	d3dStuff.commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
	static const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	d3dStuff.commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	d3dStuff.commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// ���_��`��.
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

	DrawFont(d3dStuff, L"FontTest", DirectX::XMFLOAT2(0.02f, 0.01f), DirectX::XMFLOAT2(2.0f, 2.0f));

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