/**
* @file Engine.cpp
*/
#include "Engine.h"
#include "d3dx12.h"
#include "AnimationData.h"
#include <DirectXMath.h>

using Microsoft::WRL::ComPtr;

/**
* �A�b�v���[�h�p�R�}���h���X�g����������.
*/
ResourceLoader ResourceLoader::Open(Microsoft::WRL::ComPtr<ID3D12Device> device)
{
	ResourceLoader rl;

	HRESULT hr;
	hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(rl.commandAllocator.GetAddressOf()));
	if (FAILED(hr)) {
		return rl;
	}
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, rl.commandAllocator.Get(), nullptr, IID_PPV_ARGS(rl.commandList.GetAddressOf()));
	if (FAILED(hr)) {
		return rl;
	}
	rl.device = device;
	return rl;
}

/**
* �f�[�^��GPU�ɃA�b�v���[�h����
*
* @param buffer �A�b�v���[�h��̃��\�[�X�����i�[����I�u�W�F�N�g.
* @param desc �쐬���郊�\�[�X�̏ڍ׏��̃A�h���X.
* @param data �A�b�v���[�h����f�[�^�̃A�h���X.
* @param dataSize �A�b�v���[�h����f�[�^�̃o�C�g��.
* @param rowPitch �A�b�v���[�h����̂�2, 3�����f�[�^�̏ꍇ�A���̕��ɂ�����o�C�g��. 1�����f�[�^�̏ꍇ��dataSize�Ɠ����l���w�肷��.
* @param slicePitch �A�b�v���[�h����̂�3�����f�[�^�̏ꍇ�A�e���ʂ̃o�C�g��. 2�����f�[�^�̏ꍇ��rowPitch�A1�����f�[�^�̏ꍇ��dataSize�Ɠ����l���w�肷��. 
* @param finishState �A�b�v���[�h��������buffer�̏��. ���\�[�X�o���A�𒣂邽�߂Ɏg�p�����.
*
* @retval true �A�b�v���[�h����.
*              buffer��GetGPUAddress()�ɂ���āA�A�b�v���[�h��̃A�h���X�𓾂邱�Ƃ��o����.
*              ���ۂɃA�b�v���[�h���s���ɂ́A�R�}���h���X�g���R�}���h�L���[�ɓ���Ď��s����K�v�����邱�Ƃɒ���.
* @retval false �A�b�v���[�h���s.
*               buffer�͕s���S�ȏ�Ԃɂ���. ���₩�ɔj�����邱��.
*/
bool ResourceLoader::Upload(Microsoft::WRL::ComPtr<ID3D12Resource>& buffer, const D3D12_RESOURCE_DESC* desc, const void* data, size_t dataSize, int rowPitch, int slicePitch, D3D12_RESOURCE_STATES finishState)
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
	buffer->SetName(L"ResourceLoader");

	ComPtr<ID3D12Resource> uploadBuffer;
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
	uploadBuffer->SetName(L"ResourceLoader(Uplaod Heap)");
	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData = data;
	vertexData.RowPitch = rowPitch;
	vertexData.SlicePitch = slicePitch;
	if (UpdateSubresources<1>(commandList.Get(), buffer.Get(), uploadBuffer.Get(), 0, 0, 1, &vertexData) == 0) {
		return false;
	}
	commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, finishState));
	uploadBufferList.push_back(uploadBuffer);
	return true;
}

/**
* �A�b�v���[�h�p�̃R�}���h���X�g�����.
*/
bool ResourceLoader::Close()
{
	return SUCCEEDED(commandList->Close());
}

/**
* �A�b�v���[�h�p�R�}���h���X�g���擾����.
*/
ID3D12GraphicsCommandList* ResourceLoader::GetCommandList()
{
	return commandList.Get();
}

bool DescriptorHeapManager::Initialize(Microsoft::WRL::ComPtr<ID3D12Device> dev, int size)
{
	device = dev;
	D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
	srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvHeapDesc.NumDescriptors = size;
	srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	HRESULT hr = device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(descriptorHeap.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}
	descriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	freeDescriptorList.reserve(size);
	for (int i = size - 1; i >= 0; --i) {
		freeDescriptorList.push_back(i);
	}
	return true;
}

ID3D12DescriptorHeap* DescriptorHeapManager::Get()
{
	return descriptorHeap.Get();
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapManager::GetFreeSRVHandle(Microsoft::WRL::ComPtr<ID3D12Resource> res, DXGI_FORMAT format)
{
	if (freeDescriptorList.empty()) {
		return{ 0 };
	}
	const int descriptorIndex = freeDescriptorList.back();
	freeDescriptorList.pop_back();

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	device->CreateShaderResourceView(res.Get(), &srvDesc, CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeap->GetCPUDescriptorHandleForHeapStart(), descriptorIndex, descriptorSize));

	return CD3DX12_GPU_DESCRIPTOR_HANDLE(descriptorHeap->GetGPUDescriptorHandleForHeapStart(), descriptorIndex, descriptorSize);
}

void DescriptorHeapManager::ReleaseHandle(D3D12_GPU_DESCRIPTOR_HANDLE handle)
{
	const int descriptorIndex = static_cast<int>(handle.ptr - descriptorHeap->GetGPUDescriptorHandleForHeapStart().ptr) / descriptorSize;
	freeDescriptorList.push_back(descriptorIndex);
}

Engine::Engine() :
	running(true),
	initialized(false),
	warp(false),
	masterFenceValue(0),
	fenceEvent(nullptr)
{
}

bool Engine::Initialize(HWND hw, int w, int h, bool fs)
{
	HRESULT hr;

	width = w;
	height = h;
	fullScreen = fs;
	hwnd = hw;

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
		warp = true;
	}
	hr = D3D12CreateDevice(dxgiAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(device.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}

	// �R�}���h�L���[���쐬.
	// �ʏ�A�R�}���h�L���[�̓f�o�C�X���Ƃ�1�����쐬����.
	D3D12_COMMAND_QUEUE_DESC cqDesc = {};
	hr = device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(commandQueue.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}

	// �X���b�v�`�F�[�����쐬.
	const int frameBufferCount = 3;
	DXGI_SWAP_CHAIN_DESC1 scDesc = {};
	scDesc.Width = width;
	scDesc.Height = height;
	scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.SampleDesc.Count = 1;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.BufferCount = frameBufferCount;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	{
		ComPtr<IDXGISwapChain1> tmpSwapChain;
		hr = dxgiFactory->CreateSwapChainForHwnd(commandQueue.Get(), hwnd, &scDesc, nullptr, nullptr, tmpSwapChain.GetAddressOf());
		if (FAILED(hr)) {
			return false;
		}
		tmpSwapChain.As(&swapChain);
	}
	frameIndex = swapChain->GetCurrentBackBufferIndex();

	cbvSrvUavDescriptorHeap.Initialize(device, 100);

	// RTV�p�̃f�X�N���v�^�q�[�v�y�уf�X�N���v�^���쐬.
	D3D12_DESCRIPTOR_HEAP_DESC rtvDesc = {};
	rtvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvDesc.NumDescriptors = frameBufferCount;
	rtvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = device->CreateDescriptorHeap(&rtvDesc, IID_PPV_ARGS(rtvDescriptorHeap.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}
	rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	for (int i = 0; i < frameBufferCount; ++i) {
		hr = swapChain->GetBuffer(i, IID_PPV_ARGS(renderTargetList[i].GetAddressOf()));
		if (FAILED(hr)) {
			return false;
		}
		device->CreateRenderTargetView(renderTargetList[i].Get(), nullptr, rtvHandle);
		rtvHandle.ptr += rtvDescriptorSize;
	}

	// DS�p�̃f�X�N���v�^�q�[�v�y�уf�X�N���v�^���쐬.
	D3D12_DESCRIPTOR_HEAP_DESC dsvDesc = {};
	dsvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvDesc.NumDescriptors = 1;
	dsvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	hr = device->CreateDescriptorHeap(&dsvDesc, IID_PPV_ARGS(dsvDescriptorHeap.GetAddressOf()));
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
	hr = device->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL),
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&dsClearValue,
		IID_PPV_ARGS(depthStencilbuffer.GetAddressOf())
	);
	if (FAILED(hr)) {
		return false;
	}
	device->CreateDepthStencilView(depthStencilbuffer.Get(), &depthStencilDesc, dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

	// �萔�o�b�t�@���쐬.
	// D3D12�̎d�l�ɂ��A���\�[�X�q�[�v��4MB(�}���`�T���v���e�N�X�`���̏ꍇ)�A���邢��64KB(���̑��̏ꍇ)�ɃA���C�������.
	// �萔�o�b�t�@�́u���̑��v�Ȃ̂ŁA64KB�P�ʂŊm�ۂ���̂��ł��������悢.
	// �������A����͊m�ۂɌ��������ƂŁA�萔�o�b�t�@�̓ǂݏo���͂��������ׂ����A256�o�C�g�A���C�����v�������.
	// ���̂��߁ASetGraphicsRootConstantBufferView()�ɓn���A�h���X�́A�萔�o�b�t�@�̐擪����256�o�C�g�P�ʂɂ���K�v������.
	// https://msdn.microsoft.com/en-us/library/windows/desktop/dn899216(v=vs.85).aspx
	for (int i = 0; i < frameBufferCount; ++i) {
		hr = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(64 * 1024),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(cbvUploadHeapList[i].GetAddressOf())
		);
		if (FAILED(hr)) {
			return false;
		}
		cbvUploadHeapList[i]->SetName(L"CBV Upload Heap");
		D3D12_RANGE  cbvRange = { 0, 0 };
		hr = cbvUploadHeapList[i]->Map(0, &cbvRange, &cbvHeapBegin[i]);
		if (FAILED(hr)) {
			return false;
		}
	}

	// �R�}���h�A���P�[�^���쐬.
	// �R�}���h�A���P�[�^�͕`�撆��GPU�����s����e�R�}���h��ێ�����.
	// GPU���`�撆�̃R�}���h��j�������ꍇ�AGPU�̓���͖���`�ɂȂ��Ă��܂�. ���̂��߁A�`�撆�̓R�}���h��ێ���������K�v������.
	// �����ł́A�e�o�b�N�o�b�t�@�ɂЂƂÂR�}���h�A���P�[�^����������.
	// ����ɂ���āA����o�b�N�o�b�t�@���`�撆�ł��A���̃o�b�N�o�b�t�@�̂��߂̃R�}���h�𐶐��E�j���ł���悤�ɂ���.
	for (int i = 0; i < frameBufferCount; ++i) {
		hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator[i].GetAddressOf()));
		if (FAILED(hr)) {
			return false;
		}
	}

	// �R�}���h���X�g���쐬.
	// �R�}���h�A���P�[�^�ƈقȂ�A�R�}���h���X�g�̓R�}���h�L���[�����s���ꂽ���ƂȂ炢�ł����Z�b�g�ł���.
	// �o�b�N�o�b�t�@���Ɏ��K�v���Ȃ�����1�������΂悢.
	// ���̂����A�`�悵�����o�b�N�o�b�t�@���ς�閈�ɁA�Ή�����R�}���h�A���P�[�^��ݒ肵�����K�v������.
	// �������ꂽ����̃R�}���h���X�g�̓��Z�b�g���Ăяo���ꂽ����Ɠ�����ԂɂȂ��Ă��邽�߁A�����ɃR�}���h�𑗂荞�ނ��Ƃ��o����.
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator[0].Get(), nullptr, IID_PPV_ARGS(prologueCommandList.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}
	if (FAILED(prologueCommandList->Close())) {
		return false;
	}
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator[0].Get(), nullptr, IID_PPV_ARGS(epilogueCommandList.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}
	if (FAILED(epilogueCommandList->Close())) {
		return false;
	}
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator[0].Get(), nullptr, IID_PPV_ARGS(commandList.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}
	if (FAILED(commandList->Close())) {
		return false;
	}

	// �t�F���X�ƃt�F���X�C�x���g���쐬.
	// DirectX 12�ł́AGPU�̕`��I�������o���邽�߂Ƀt�F���X�Ƃ������̂��g��.
	// �t�F���X��OS�̃C�x���g���֘A�t���邱�Ƃ��ł���悤�ɐ݌v����Ă���.
	// �����ŁAOS�̃C�x���g���쐬���A�t�F���X���R�}���h���X�g�֐ݒ肷��Ƃ��Ɋ֘A�t����.
	hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(fence.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}
	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!fenceEvent) {
		return false;
	}
	++masterFenceValue;
	for (int i = 0; i < frameBufferCount; ++i) {
		fenceValueForFrameBuffer[i] = 0;
	}

	if (!textureManager.Initialize(&cbvSrvUavDescriptorHeap)) {
		return false;
	}

	// �t�H���g��ǂݍ���.
	if (!LoadFont(fontInfo, L"Res/ArialBlack.fnt", static_cast<float>(width), static_cast<float>(height))) {
		return false;
	}
	ResourceLoader resourceLoader = ResourceLoader::Open(device);
	const std::wstring fontTextureName(std::wstring(L"Res/") + fontInfo.fontImage);
	if (!LoadTexture(resourceLoader, fontTextureName.c_str())) {
		return false;
	}
	fontInfo.srvHandle = GetTextureHandle(fontTextureName.c_str());
	if (!fontRenderer.Init(device, resourceLoader)) {
		return false;
	}

	if (!spriteRenderer.Init(device, resourceLoader, frameBufferCount, 1024)) {
		return false;
	}

	if (!resourceLoader.Close()) {
		return false;
	}
	ID3D12CommandList* commandListArray[] = { resourceLoader.GetCommandList() };
	if (!ExecuteCommandList(_countof(commandListArray), commandListArray)) {
		return false;
	}

	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	scissorRect.left = 0;
	scissorRect.top = 0;
	scissorRect.right = width;
	scissorRect.bottom = height;

	animationData.Init();

	initialized = true;
	return true;
}

bool Engine::Update()
{
	const float delta = 1.0f / 60.0f;
	entityList.Update(delta);
	return true;
}

void Engine::Finalize()
{
	for (int i = 0; i < frameBufferCount; ++i) {
		frameIndex = i;
		WaitForPreviousFrame();
	}

	BOOL fs = FALSE;
	if (swapChain->GetFullscreenState(&fs, nullptr)) {
		swapChain->SetFullscreenState(false, nullptr);
	}

	if (fenceEvent) {
		CloseHandle(fenceEvent);
	}
}

/**
* ���O�̃t���[���̕`�抮����҂�.
*/
void Engine::WaitForPreviousFrame()
{
	const UINT64 lastCompletedFence = fence->GetCompletedValue();
	frameIndex = swapChain->GetCurrentBackBufferIndex();
	if (fenceValueForFrameBuffer[frameIndex] > lastCompletedFence) {
		HRESULT hr = fence->SetEventOnCompletion(fenceValueForFrameBuffer[frameIndex], fenceEvent);
		if (FAILED(hr)) {
			running = false;
		}
		WaitForSingleObject(fenceEvent, INFINITE);
	}
}

bool Engine::BeginRender(ID3D12PipelineState* pso)
{
	WaitForPreviousFrame();

	// �R�}���h���X�g�y�уR�}���h�A���P�[�^�����Z�b�g.
	HRESULT hr;
	hr = commandAllocator[frameIndex]->Reset();
	if (FAILED(hr)) {
		running = false;
	}

	hr = prologueCommandList->Reset(commandAllocator[frameIndex].Get(), pso);
	if (FAILED(hr)) {
		running = false;
	}
	prologueCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargetList[frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	hr = prologueCommandList->Close();
	if (FAILED(hr)) {
		running = false;
	}

	hr = epilogueCommandList->Reset(commandAllocator[frameIndex].Get(), pso);
	if (FAILED(hr)) {
		running = false;
	}
	epilogueCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargetList[frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));
	hr = epilogueCommandList->Close();
	if (FAILED(hr)) {
		running = false;
	}

	hr = commandList->Reset(commandAllocator[frameIndex].Get(), pso);
	if (FAILED(hr)) {
		running = false;
	}

	// �R�}���h��ς�ł���.
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	rtvHandle.ptr += frameIndex * rtvDescriptorSize;
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
	static const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	return true;
}

bool Engine::EndRender(size_t num, ID3D12CommandList** pp)
{
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = GetCurrentRTVHandle();
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = GetCurrentDSVHandle();
	if (!fontRenderer.Begin(&fontInfo, GetFrameIndex(), &rtvHandle, &dsvHandle, &viewport, &scissorRect)) {
		StopRunning();
	}
	for (const auto& e : textList) {
		fontRenderer.Draw(e.text, e.position, e.scale, e.color);
	}
	if (!fontRenderer.End()) {
		StopRunning();
	}

	spriteRenderer.ClearRenderingInfo();
	entityList.Draw(spriteRenderer);
	spriteRenderer.Draw(frameIndex, &rtvHandle, &dsvHandle, &viewport, &scissorRect);

	// �`��J�n.
	std::vector<ID3D12CommandList*> list;
	list.reserve(3 + num);
	list.push_back(prologueCommandList.Get());
	list.push_back(commandList.Get());
	for (size_t i = 0; i < num; ++i) {
		list.push_back(pp[i]);
	}
	list.push_back(spriteRenderer.GetCommandList());
	list.push_back(fontRenderer.GetCommandList());
	list.push_back(epilogueCommandList.Get());
	commandQueue->ExecuteCommandLists(static_cast<UINT>(list.size()), list.data());
	HRESULT hr = swapChain->Present(1, 0);
	if (FAILED(hr)) {
		StopRunning();
	}

	fenceValueForFrameBuffer[frameIndex] = masterFenceValue;
	hr = commandQueue->Signal(fence.Get(), masterFenceValue);
	if (FAILED(hr)) {
		StopRunning();
	}
	++masterFenceValue;

	return true;
}

bool Engine::ExecuteCommandList(size_t num, ID3D12CommandList** pp)
{
	std::vector<ID3D12CommandList*> list;
	list.reserve(1 + num);
	list.push_back(commandList.Get());
	for (size_t i = 0; i < num; ++i) {
		list.push_back(pp[i]);
	}
	commandQueue->ExecuteCommandLists(static_cast<UINT>(list.size()), list.data());
	HRESULT hr = commandQueue->Signal(fence.Get(), masterFenceValue);
	if (FAILED(hr)) {
		return false;
	}
	fence->SetEventOnCompletion(masterFenceValue, fenceEvent);
	WaitForSingleObject(fenceEvent, INFINITE);
	++masterFenceValue;

	return true;
}

void Engine::CreateEntity()
{
	auto p = entityList.CreateScriptEntity(
		DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f),
		animationData.GetData(0),
		textureManager.GetTextureHandle(L"Res/playerunit.png"),
		GetActionList()
	);
}

D3D12_CPU_DESCRIPTOR_HANDLE Engine::GetCurrentRTVHandle() const { return CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), frameIndex, rtvDescriptorSize); }
D3D12_CPU_DESCRIPTOR_HANDLE Engine::GetCurrentDSVHandle() const { return dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(); }
