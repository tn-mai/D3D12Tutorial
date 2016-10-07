/**
* @file Sprite.cpp
*/
#include "Sprite.h"
#include "Engine.h"
#include <algorithm>
#include <fstream>

using Microsoft::WRL::ComPtr;

bool LoadShader(const wchar_t* filename, const char* target, ComPtr<ID3DBlob>& blob);

namespace /* unnamed */ {

/**
* スプライト描画用頂点データレイアウト.
*/
D3D12_INPUT_ELEMENT_DESC vertexLayout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

/**
* スプライト描画用頂点データ型.
*/
struct Vertex {
	constexpr Vertex(float x, float y, float r, float g, float b, float a, float u, float v) : pos(x, y), col(r, g, b, a), texCoord(u, v) {}
	DirectX::XMFLOAT2 pos;
	DirectX::XMFLOAT4 col;
	DirectX::XMFLOAT2 texCoord;
};

/**
* ひとつのスプライトデータを頂点バッファに設定.
*/
void AddVertex(const RenderingInfo& info, Vertex* v)
{
	// 0-3
	// |\|
	// 2-1
	DirectX::XMFLOAT2 topLeftScreenPos(info.pos.x * 2.0f - 1.0f, (1.0f - info.pos.y) * 2.0f - 1.0f);
	DirectX::XMFLOAT2 curPos(topLeftScreenPos);
	DirectX::XMFLOAT2 halfSize(info.cell.ssize.x * 0.5f * info.scale.x, info.cell.ssize.y * 0.5f * info.scale.y);

	v[0].pos.x = curPos.x - halfSize.x;
	v[0].pos.y = curPos.y + halfSize.y;
	v[0].texCoord = info.cell.uv;
	v[0].col = info.color;
	v[1].pos.x = curPos.x + halfSize.x;
	v[1].pos.y = curPos.y - halfSize.y;
	v[1].texCoord.x = info.cell.uv.x + info.cell.tsize.x;
	v[1].texCoord.y = info.cell.uv.y + info.cell.tsize.y;
	v[1].col = info.color;
	v[2].pos.x = curPos.x - halfSize.x;
	v[2].pos.y = curPos.y - halfSize.y;
	v[2].texCoord.x = info.cell.uv.x;
	v[2].texCoord.y = info.cell.uv.y + info.cell.tsize.y;
	v[2].col = info.color;
	v[3].pos.x = curPos.x + halfSize.x;
	v[3].pos.y = curPos.y + halfSize.y;
	v[3].texCoord.x = info.cell.uv.x + info.cell.tsize.x;
	v[3].texCoord.y = info.cell.uv.y;
	v[3].col = info.color;
}

} // unnamed namedpace

SpriteRenderer::SpriteRenderer() :
	maxSpriteCount(0),
	frameBufferCount(0)
{
}

bool SpriteRenderer::Init(Microsoft::WRL::ComPtr<ID3D12Device> device, ResourceLoader& resourceLoader, int numFrameBuffer, int maxSprite)
{
	maxSpriteCount = maxSprite;
	frameBufferCount = numFrameBuffer;
	frameResourceList.resize(numFrameBuffer);
	renderingInfoList.reserve(maxSprite);

	HRESULT hr;
	{
		D3D12_DESCRIPTOR_RANGE descRange[1] = {};
		descRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descRange[0].NumDescriptors = 1;
		descRange[0].BaseShaderRegister = 0;
		descRange[0].RegisterSpace = 0;
		descRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
		D3D12_ROOT_PARAMETER rootParameterList[1];
		rootParameterList[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameterList[0].DescriptorTable.NumDescriptorRanges = _countof(descRange);
		rootParameterList[0].DescriptorTable.pDescriptorRanges = descRange;
		rootParameterList[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
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
		hr = D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, nullptr);
		if (FAILED(hr)) {
			return false;
		}
		hr = device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(rootSignature.GetAddressOf()));
		if (FAILED(hr)) {
			return false;
		}
	}

	// フォント描画用PSOを作成.
	if (!LoadShader(L"Res/SpriteVertexShader.hlsl", "vs_5_0", vertexShaderBlob)) {
		return false;
	}
	if (!LoadShader(L"Res/SpritePixelShader.hlsl", "ps_5_0", pixelShaderBlob)) {
		return false;
	}
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = rootSignature.Get();
	psoDesc.VS.pShaderBytecode = vertexShaderBlob->GetBufferPointer();
	psoDesc.VS.BytecodeLength = vertexShaderBlob->GetBufferSize();
	psoDesc.PS.pShaderBytecode = pixelShaderBlob->GetBufferPointer();
	psoDesc.PS.BytecodeLength = pixelShaderBlob->GetBufferSize();
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = 0xffffffff;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.InputLayout.pInputElementDescs = vertexLayout;
	psoDesc.InputLayout.NumElements = _countof(vertexLayout);
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.BlendState.AlphaToCoverageEnable = FALSE;
	psoDesc.BlendState.IndependentBlendEnable = FALSE;
	psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
	psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(pso.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}

	for (int i = 0; i < frameBufferCount; ++i) {
		hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(frameResourceList[i].commandAllocator.GetAddressOf()));
		if (FAILED(hr)) {
			return false;
		}
	}
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, frameResourceList[0].commandAllocator.Get(), nullptr, IID_PPV_ARGS(commandList.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}
	hr = commandList->Close();
	if (FAILED(hr)) {
		return false;
	}

	for (int i = 0; i < frameBufferCount; ++i) {
		hr = device->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),
			D3D12_HEAP_FLAG_NONE,
			&CD3DX12_RESOURCE_DESC::Buffer(maxSpriteCount * sizeof(Vertex)),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(frameResourceList[i].vertexBuffer.GetAddressOf())
		);
		if (FAILED(hr)) {
			return false;
		}
		frameResourceList[i].vertexBuffer->SetName(L"Sprite Vertex Buffer");
		CD3DX12_RANGE range(0, 0);
		hr = frameResourceList[i].vertexBuffer->Map(0, &range, &frameResourceList[i].vertexBufferGPUAddress);
		if (FAILED(hr)) {
			return false;
		}
		frameResourceList[i].vertexBufferView.BufferLocation = frameResourceList[i].vertexBuffer->GetGPUVirtualAddress();
		frameResourceList[i].vertexBufferView.StrideInBytes = sizeof(Vertex);
		frameResourceList[i].vertexBufferView.SizeInBytes = static_cast<UINT>(maxSpriteCount * sizeof(Vertex));
	}
	ComPtr<ID3D12Resource> iBUploadHeap;
	std::vector<DWORD> indexList;
	indexList.resize(maxSpriteCount * 6);
	for (int i = 0; i < maxSpriteCount; ++i) {
		indexList[i * 6 + 0] = i * 4 + 0;
		indexList[i * 6 + 1] = i * 4 + 1;
		indexList[i * 6 + 2] = i * 4 + 2;
		indexList[i * 6 + 3] = i * 4 + 0;
		indexList[i * 6 + 4] = i * 4 + 3;
		indexList[i * 6 + 5] = i * 4 + 1;
	}
	const int indexListSize = static_cast<int>(maxSpriteCount * 6 * sizeof(DWORD));
	if (!resourceLoader.Upload(indexBuffer, &CD3DX12_RESOURCE_DESC::Buffer(indexListSize), indexList.data(), indexListSize, indexListSize, indexListSize, D3D12_RESOURCE_STATE_INDEX_BUFFER)) {
		return false;
	}
	indexBufferView.BufferLocation = indexBuffer->GetGPUVirtualAddress();
	indexBufferView.SizeInBytes = indexListSize;
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	return true;
}

void SpriteRenderer::ClearRenderingInfo()
{
	renderingInfoList.clear();
}

void SpriteRenderer::AddRenderingInfo(const SpriteCell& cell, D3D12_GPU_DESCRIPTOR_HANDLE srvHandle, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT2 scale, float rot, DirectX::XMFLOAT4 color)
{
	renderingInfoList.emplace_back(cell, srvHandle, pos, scale, rot, color);
}

/**
* スプライトを描画.
*
* @param info 描画するスプライト情報.
*/
bool SpriteRenderer::Draw(int frameIndex, const D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandle, const D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle, const D3D12_VIEWPORT* viewport, const D3D12_RECT* scissorRect)
{
	FrameResource& fr = frameResourceList[frameIndex];
	HRESULT hr = fr.commandAllocator->Reset();
	if (FAILED(hr)) {
		return false;
	}
	hr = commandList->Reset(fr.commandAllocator.Get(), pso.Get());
	if (FAILED(hr)) {
		return false;
	}

	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &fr.vertexBufferView);
	commandList->IASetIndexBuffer(&indexBufferView);
	commandList->OMSetRenderTargets(1, rtvHandle, FALSE, dsvHandle);
	commandList->RSSetViewports(1, viewport);
	commandList->RSSetScissorRects(1, scissorRect);

	std::stable_sort(renderingInfoList.begin(), renderingInfoList.end(), [](const RenderingInfo& lhs, const RenderingInfo& rhs) { return lhs.pos.z > rhs.pos.z; });

	D3D12_GPU_DESCRIPTOR_HANDLE srvHandle = {};
	int numGroupSprites = 0;
	int vertexLocation = 0;
	Vertex* v = static_cast<Vertex*>(fr.vertexBufferGPUAddress);
	const Vertex* const vEnd = v + maxSpriteCount * 4;
	for (const RenderingInfo& info : renderingInfoList) {
		if (srvHandle.ptr != info.srvHandle.ptr) {
			if (srvHandle.ptr) {
				commandList->DrawIndexedInstanced(numGroupSprites * 6, 1, 0, vertexLocation, 0);
				vertexLocation += numGroupSprites * 6;
				numGroupSprites = 0;
			}
			commandList->SetGraphicsRootDescriptorTable(0, info.srvHandle);
			srvHandle = info.srvHandle;
		}
		AddVertex(info, v);
		++numGroupSprites;
		v += 4;
		if (v >= vEnd) {
			break;
		}
	}
	if (srvHandle.ptr) {
		commandList->DrawIndexedInstanced(numGroupSprites * 6, 1, 0, vertexLocation, 0);
	}

	return SUCCEEDED(commandList->Close());
}

/**
* コマンドリストを取得する.
*/
ID3D12GraphicsCommandList* SpriteRenderer::GetCommandList()
{
	return commandList.Get();
}
