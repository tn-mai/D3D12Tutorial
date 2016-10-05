/**
* @file Engine.h
*/
#ifndef TUTORIAL_SRC_ENGINE_H_
#define TUTORIAL_SRC_ENGINE_H_
#include "Texture.h"
#include "Sprite.h"
#include "Entity.h"
#include "Font.h"

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
	Engine();
	bool Initialize(HWND hwnd, int width, int height);
	ID3D12Device* GetDevice() { return device.Get(); }
	IDXGISwapChain3* GetSwapChain() { return swapChain.Get(); }
	ID3D12CommandQueue* GetCommandQueue() { return commandQueue.Get(); }
	bool IsWarpDevice() const { return warp; }

	bool LoadTexture(const wchar_t* filename) { return textureManager.LoadFromFile(device, filename); }
	D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(const wchar_t* filename) const { return textureManager.GetTextureHandle(filename); }
	DXGI_FORMAT GetTextureFormat(const wchar_t* filename) const { return textureManager.GetTextureFormat(filename); }
	ID3D12GraphicsCommandList* GetTextureCommandList() { return textureManager.GetCommandList(); }
	ID3D12DescriptorHeap* GetDescriptorHeap() { return cbvSrvUavDescriptorHeap.Get(); }
	D3D12_GPU_DESCRIPTOR_HANDLE CreateSRVHandle(Microsoft::WRL::ComPtr<ID3D12Resource> res, DXGI_FORMAT format) { return cbvSrvUavDescriptorHeap.GetFreeSRVHandle(res, format); }

private:
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> swapChain;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue;
	bool warp;

	DescriptorHeapManager cbvSrvUavDescriptorHeap;

	//FontRenderer fontRendrer;
	//SpriteRenderer spriteRenderer;
	//EntityList entytyList;
	Texture::Manager  textureManager;
};

#endif // TUTORIAL_SRC_ENGINE_H_
