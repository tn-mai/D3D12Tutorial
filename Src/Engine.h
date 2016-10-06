/**
* @file Engine.h
*/
#ifndef TUTORIAL_SRC_ENGINE_H_
#define TUTORIAL_SRC_ENGINE_H_
#include "Texture.h"
#include <wrl/client.h>
#include <vector>
#include <d3d12.h>
#include <dxgi1_4.h>
//#include "Sprite.h"
//#include "Entity.h"
//#include "Font.h"

typedef std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> UploadBufferList;

class ResourceLoader
{
public:
	static ResourceLoader Open(Microsoft::WRL::ComPtr<ID3D12Device>);
	bool Upload(Microsoft::WRL::ComPtr<ID3D12Resource>& buffer, const D3D12_RESOURCE_DESC* desc, const void* data, size_t dataSize, int rowPitch, int slicePitch, D3D12_RESOURCE_STATES finishState);
	bool Close();
	ID3D12GraphicsCommandList* GetCommandList();
	ID3D12Device* GetDevice() { return device.Get(); }
private:
	bool initialized;
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	UploadBufferList uploadBufferList;
};

class DescriptorHeapManager
{
public:
	bool Initialize(Microsoft::WRL::ComPtr<ID3D12Device>, int);
	ID3D12DescriptorHeap* Get();
	D3D12_GPU_DESCRIPTOR_HANDLE GetFreeSRVHandle(Microsoft::WRL::ComPtr<ID3D12Resource> res, DXGI_FORMAT format);
	void ReleaseHandle(D3D12_GPU_DESCRIPTOR_HANDLE);
	bool Empty() const { return freeDescriptorList.empty(); }
private:
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;
	std::vector<int> freeDescriptorList;
	int descriptorSize;
};

class Engine
{
public:
	static const int frameBufferCount = 3;

	Engine();
	bool Initialize(HWND hw, int w, int h, bool fs);
	void Finalize();
	void WaitForPreviousFrame();
	bool BeginRender(ID3D12PipelineState*);
	bool EndRender(size_t num, ID3D12CommandList** pp);
	bool ExecuteCommandList(size_t num, ID3D12CommandList** pp);
	void StopRunning() { running = false; }
	ID3D12Device* GetDevice() { return device.Get(); }
	IDXGISwapChain3* GetSwapChain() { return swapChain.Get(); }
	ID3D12CommandQueue* GetCommandQueue() { return commandQueue.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() { return commandList.Get(); }
	D3D12_GPU_VIRTUAL_ADDRESS GetCurrentCBVAddress() { return cbvUploadHeapList[frameIndex]->GetGPUVirtualAddress(); }
	void* GetCurrentCBVHeap() { return cbvHeapBegin[frameIndex]; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTVHandle() const;
	D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentDSVHandle() const;
	int GetWidth() const { return width; }
	int GetHeight() const { return height; }
	bool IsWarpDevice() const { return warp; }
	bool IsRunning() const { return running; }
	bool IsInitialized() const { return initialized; }
	int GetFrameIndex() const { return frameIndex; }

	bool LoadTexture(ResourceLoader& resourceLoader, const wchar_t* filename) { return textureManager.LoadFromFile(resourceLoader, filename); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(const wchar_t* filename) const { return textureManager.GetTextureHandle(filename); }
	DXGI_FORMAT GetTextureFormat(const wchar_t* filename) const { return textureManager.GetTextureFormat(filename); }
	ID3D12DescriptorHeap* GetDescriptorHeap() { return cbvSrvUavDescriptorHeap.Get(); }

	uint8_t* GetConstantBufferAddress(int n) { return static_cast<uint8_t*>(cbvHeapBegin[n]); }

private:
	int width;
	int height;
	bool fullScreen;
	bool running;
	bool warp;
	bool initialized;
	HWND hwnd;

	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> renderTargetList[frameBufferCount];
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilbuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource> cbvUploadHeapList[frameBufferCount];
	void* cbvHeapBegin[frameBufferCount];
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> prologueCommandList;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> epilogueCommandList;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator[frameBufferCount];
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence;
	HANDLE fenceEvent;
	UINT64 masterFenceValue;
	UINT64 fenceValueForFrameBuffer[frameBufferCount];
	int frameIndex;
	int rtvDescriptorSize;

	DescriptorHeapManager cbvSrvUavDescriptorHeap;

	//FontRenderer fontRendrer;
	//SpriteRenderer spriteRenderer;
	//EntityList entytyList;
	Texture::Manager  textureManager;
};

#endif // TUTORIAL_SRC_ENGINE_H_
