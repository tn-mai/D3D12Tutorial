/**
* @file Sprite.h
*/
#pragma once
#include "stdafx.h"
#include <string>
#include <memory>
#include <vector>

class ResourceLoader;

/**
* スプライトデータ型.
*/
struct SpriteCell {
	DirectX::XMFLOAT2 uv; ///< テクスチャ上の左上座標.
	DirectX::XMFLOAT2 tsize; ///< テクスチャ上の縦横サイズ.
	DirectX::XMFLOAT2 ssize; ///< スクリーン座標上の縦横サイズ.
};

struct RenderingInfo
{
	RenderingInfo(const SpriteCell& c, D3D12_GPU_DESCRIPTOR_HANDLE srv, DirectX::XMFLOAT3 p, DirectX::XMFLOAT2 s, float r, DirectX::XMFLOAT4 col) :
		cell(c),
		srvHandle(srv),
		pos(p),
		scale(s),
		rot(r),
		color(col)
	{}

	SpriteCell cell;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandle;
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 scale;
	float rot;
	DirectX::XMFLOAT4 color;
};

class SpriteRenderer
{
public:
	SpriteRenderer();
	bool Init(Microsoft::WRL::ComPtr<ID3D12Device>, ResourceLoader&, int numFrameBuffer, int maxSprite);

	void ClearRenderingInfo();
	void AddRenderingInfo(const SpriteCell& cell, D3D12_GPU_DESCRIPTOR_HANDLE srvHandle, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT2 scale = { 1.0f, 1.0f }, float rot = 0.0f, DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f });
	bool Draw(int, ID3D12DescriptorHeap* descHeap, const D3D12_CPU_DESCRIPTOR_HANDLE*, const D3D12_CPU_DESCRIPTOR_HANDLE*, const D3D12_VIEWPORT*, const D3D12_RECT*);
	ID3D12GraphicsCommandList* GetCommandList();

private:
	size_t maxSpriteCount;
	int frameBufferCount;

	std::vector<RenderingInfo> renderingInfoList;

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;

	Microsoft::WRL::ComPtr<ID3DBlob> vertexShaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> pixelShaderBlob;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pso;

	struct FrameResource
	{
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
		void* vertexBufferGPUAddress;
	};

	std::vector<FrameResource> frameResourceList;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuffer;
	D3D12_INDEX_BUFFER_VIEW indexBufferView;
};
