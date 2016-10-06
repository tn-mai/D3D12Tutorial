/**
* @file Font.cpp
*/
#include "Font.h"
#include "Engine.h"
#include <algorithm>
#include <fstream>

using Microsoft::WRL::ComPtr;

bool LoadShader(const wchar_t* filename, const char* target, ComPtr<ID3DBlob>& blob);

/**
* フォント描画用頂点データレイアウト.
*/
D3D12_INPUT_ELEMENT_DESC fontVertexLayout[] = {
	{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

/**
* フォント描画用頂点データ型.
*/
struct FontVertex {
	constexpr FontVertex(float x, float y, float r, float g, float b, float a, float u, float v) : pos(x, y), col(r, g, b, a), texCoord(u, v) {}
	DirectX::XMFLOAT2 pos;
	DirectX::XMFLOAT4 col;
	DirectX::XMFLOAT2 texCoord;
};

float Font::GetKerning(wchar_t first, wchar_t second) const
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

const FontChar* Font::GetChar(wchar_t c) const
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
std::wstring GetValue(std::wifstream& fs)
{
	std::wstring tmp;
	std::getline(fs, tmp, L'=');
	wchar_t c = fs.get();
	if (c == L'"') {
		std::getline(fs, tmp, L'"');
	}
	else {
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
* フォント定義ファイルを読み込む.
*
* @param font フォント定義オブジェクト.
* @param filename フォント定義ファイル名.
* @param screenWidth 表示スクリーンの横ピクセル数.
* @param screenHeight 表示スクリーンの縦ピクセル数.
*
* @retval treu 読み込み成功.
* @retval false 読み込み失敗.
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

FontRenderer::FontRenderer() :
	frameIndex(0),
	numCharacters(0),
	fontInfo(nullptr)
{
}

bool FontRenderer::Init(Microsoft::WRL::ComPtr<ID3D12Device> device, ResourceLoader& resourceLoader)
{
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
	if (!LoadShader(L"Res/FontVertexShader.hlsl", "vs_5_0", fontVertexShaderBlob)) {
		return false;
	}
	if (!LoadShader(L"Res/FontPixelShader.hlsl", "ps_5_0", fontPixelShaderBlob)) {
		return false;
	}
	D3D12_GRAPHICS_PIPELINE_STATE_DESC fontPsoDesc = {};
	fontPsoDesc.pRootSignature = rootSignature.Get();
	fontPsoDesc.VS.pShaderBytecode = fontVertexShaderBlob->GetBufferPointer();
	fontPsoDesc.VS.BytecodeLength = fontVertexShaderBlob->GetBufferSize();
	fontPsoDesc.PS.pShaderBytecode = fontPixelShaderBlob->GetBufferPointer();
	fontPsoDesc.PS.BytecodeLength = fontPixelShaderBlob->GetBufferSize();
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
	hr = device->CreateGraphicsPipelineState(&fontPsoDesc, IID_PPV_ARGS(fontPSO.GetAddressOf()));
	if (FAILED(hr)) {
		return false;
	}

	for (int i = 0; i < frameBufferCount; ++i) {
		hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator[i].GetAddressOf()));
		if (FAILED(hr)) {
			return false;
		}
	}
	hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator[0].Get(), nullptr, IID_PPV_ARGS(commandList.GetAddressOf()));
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
			&CD3DX12_RESOURCE_DESC::Buffer(maxFontCharacters * sizeof(FontVertex)),
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(fontVertexBuffer[i].GetAddressOf())
		);
		if (FAILED(hr)) {
			return false;
		}
		fontVertexBuffer[i]->SetName(L"Font Vertex Buffer");
		CD3DX12_RANGE range(0, 0);
		hr = fontVertexBuffer[i]->Map(0, &range, &fontVertexBufferGPUAddress[i]);
		if (FAILED(hr)) {
			return false;
		}
		fontVertexBufferView[i].BufferLocation = fontVertexBuffer[i]->GetGPUVirtualAddress();
		fontVertexBufferView[i].StrideInBytes = sizeof(FontVertex);
		fontVertexBufferView[i].SizeInBytes = maxFontCharacters * sizeof(FontVertex);
	}
	std::vector<DWORD> fontIndexList;
	fontIndexList.resize(maxFontCharacters * 6);
	for (int i = 0; i < maxFontCharacters; ++i) {
		fontIndexList[i * 6 + 0] = i * 4 + 0;
		fontIndexList[i * 6 + 1] = i * 4 + 1;
		fontIndexList[i * 6 + 2] = i * 4 + 2;
		fontIndexList[i * 6 + 3] = i * 4 + 0;
		fontIndexList[i * 6 + 4] = i * 4 + 3;
		fontIndexList[i * 6 + 5] = i * 4 + 1;
	}
	const size_t fontIndexListSize = maxFontCharacters * 6 * sizeof(DWORD);
	if (!resourceLoader.Upload(fontIndexBuffer, &CD3DX12_RESOURCE_DESC::Buffer(fontIndexListSize), fontIndexList.data(), fontIndexListSize, fontIndexListSize, fontIndexListSize, D3D12_RESOURCE_STATE_INDEX_BUFFER)) {
		return false;
	}
	fontIndexBufferView.BufferLocation = fontIndexBuffer->GetGPUVirtualAddress();
	fontIndexBufferView.SizeInBytes = fontIndexListSize;
	fontIndexBufferView.Format = DXGI_FORMAT_R32_UINT;

	return true;
}

/**
* フォントを描画.
*
* @param d3dStuff Direct3Dオブジェクト.
* @param text 描画する文字列.
* @param pos 描画位置のスクリーン座標.
* @param scale 描画するフォントの大きさ.
* @param color 描画するフォントの色.
*
* @retval treu 読み込み成功.
* @retval false 読み込み失敗.
*/
void FontRenderer::Draw(const std::wstring& text, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 scale, DirectX::XMFLOAT4 color)
{
	DirectX::XMFLOAT2 topLeftScreenPos(pos.x * 2.0f - 1.0f, (1.0f - pos.y) * 2.0f - 1.0f);
	DirectX::XMFLOAT2 curPos(topLeftScreenPos);
	DirectX::XMFLOAT2 padding((fontInfo->leftPadding + fontInfo->rightPadding) * 0.5f, (fontInfo->topPadding + fontInfo->bottomPadding) * 0.0f);

	FontVertex* v = static_cast<FontVertex*>(fontVertexBufferGPUAddress[frameIndex]);
	wchar_t lastChar = -1;
	for (int i = 0; i < text.size(); ++i) {
		if (numCharacters >= maxFontCharacters) {
			break;
		}
		const wchar_t c = text[i];
		const FontChar* fc = fontInfo->GetChar(c);
		if (!fc) {
			continue;
		}
		float kerning = 0.0f;
		if (i) {
			kerning = fontInfo->GetKerning(lastChar, c);
		}
		// 0-3
		// |\|
		// 2-1
		v[0].pos.x = curPos.x + (fc->offset.x + kerning) * scale.x;
		v[0].pos.y = curPos.y - fc->offset.y * scale.y;
		v[0].texCoord = fc->uv;
		v[0].col = color;
		v[1].pos.x = curPos.x + (fc->offset.x + fc->ssize.x + kerning) * scale.x;
		v[1].pos.y = curPos.y - (fc->offset.y + fc->ssize.y) * scale.y;
		v[1].texCoord.x = fc->uv.x + fc->tsize.x;
		v[1].texCoord.y = fc->uv.y + fc->tsize.y;
		v[1].col = color;
		v[2].pos.x = curPos.x + (fc->offset.x + kerning) * scale.x;
		v[2].pos.y = curPos.y - (fc->offset.y + fc->ssize.y) * scale.y;
		v[2].texCoord.x = fc->uv.x;
		v[2].texCoord.y = fc->uv.y + fc->tsize.y;
		v[2].col = color;
		v[3].pos.x = curPos.x + (fc->offset.x + fc->ssize.x + kerning) * scale.x;
		v[3].pos.y = curPos.y - fc->offset.y * scale.y;
		v[3].texCoord.x = fc->uv.x + fc->tsize.x;
		v[3].texCoord.y = fc->uv.y;
		v[3].col = color;

		v += 4;
		curPos.x += (fc->xadvance - padding.x) * scale.x;
		++numCharacters;
		lastChar = c;
	}
}

/**
* 描画に使用するフォントを指定する.
*/
bool FontRenderer::Begin(const Font* p, int fi, const D3D12_CPU_DESCRIPTOR_HANDLE* rtvHandle, const D3D12_CPU_DESCRIPTOR_HANDLE* dsvHandle, const D3D12_VIEWPORT* viewport, const D3D12_RECT* scissorRect)
{
	fontInfo = p;
	frameIndex = fi;
	numCharacters = 0;

	HRESULT hr = commandAllocator[frameIndex]->Reset();
	if (FAILED(hr)) {
		return false;
	}
	hr = commandList->Reset(commandAllocator[frameIndex].Get(), fontPSO.Get());
	if (FAILED(hr)) {
		return false;
	}

	commandList->SetGraphicsRootSignature(rootSignature.Get());
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &fontVertexBufferView[frameIndex]);
	commandList->IASetIndexBuffer(&fontIndexBufferView);
	commandList->SetGraphicsRootDescriptorTable(0, fontInfo->srvHandle);
	commandList->OMSetRenderTargets(1, rtvHandle, FALSE, dsvHandle);
	commandList->RSSetViewports(1, viewport);
	commandList->RSSetScissorRects(1, scissorRect);

	return true;
}

/**
* 現在のフレームの描画を終了する.
*/
bool FontRenderer::End()
{
	commandList->DrawIndexedInstanced(numCharacters * 6, 1, 0, 0, 0);
	return SUCCEEDED(commandList->Close());
}

/**
* フォント用コマンドリストを取得する.
*/
ID3D12GraphicsCommandList* FontRenderer::GetCommandList()
{
	return commandList.Get();
}
