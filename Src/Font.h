/**
* @file Font.h
*/
#pragma once
#include "stdafx.h"
#include <string>
#include <memory>
#include <vector>

struct Direct3DStuff;
class ResourceLoader;

/**
* フォントキャラクタデータ型.
*/
struct FontChar {
	int id; ///< 文字のユニコード.
	DirectX::XMFLOAT2 uv; ///< テクスチャ上の左上座標.
	DirectX::XMFLOAT2 tsize; ///< テクスチャ上の縦横サイズ.
	DirectX::XMFLOAT2 ssize; ///< スクリーン座標上の縦横サイズ.
	DirectX::XMFLOAT2 offset; ///< カーソル位置から実際の表示位置へのオフセット.
	float xadvance; ///< 次のキャラクタの表示開始位置への右方向のオフセット.
};

/**
* フォントのカーニング情報型.
*/
struct FontKerning
{
	int first; ///< 左側の文字のユニコード.
	int second; ///< 右側の文字のユニコード.
	float amount; ///< 右側の文字のX表示座標に加算する移動量.
};

/**
* フォント情報型.
*/
struct Font
{
	std::wstring name;
	std::wstring fontImage;
	int size;
	float lineHeight;
	float baseHeight;
	int textureWidth;
	int textureHeight;
	std::vector<FontChar> charList;
	std::vector<FontKerning> kerningList;
	Microsoft::WRL::ComPtr<ID3D12Resource> textureBuffer;
	D3D12_GPU_DESCRIPTOR_HANDLE srvHandle;
	float leftPadding;
	float topPadding;
	float rightPadding;
	float bottomPadding;

	float GetKerning(wchar_t first, wchar_t second) const;
	const FontChar* GetChar(wchar_t c) const;
};

class FontRenderer
{
public:
	FontRenderer();
	bool Init(Microsoft::WRL::ComPtr<ID3D12Device>, ResourceLoader&);
	bool Begin(const Font*, int, const D3D12_CPU_DESCRIPTOR_HANDLE*, const D3D12_CPU_DESCRIPTOR_HANDLE*, const D3D12_VIEWPORT*, const D3D12_RECT*);
	void Draw(const std::wstring&, DirectX::XMFLOAT2 pos, DirectX::XMFLOAT2 scale = { 1.0f, 1.0f }, DirectX::XMFLOAT4 color = { 1.0f, 1.0f, 1.0f, 1.0f });
	bool End();
	ID3D12GraphicsCommandList* GetCommandList();

private:
	static const size_t maxFontCharacters = 1024;
	static const int frameBufferCount = 3;

	int frameIndex;
	int numCharacters;
	const Font* fontInfo;

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;

	Microsoft::WRL::ComPtr<ID3DBlob> fontVertexShaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> fontPixelShaderBlob;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> fontPSO;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator[frameBufferCount];
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;

	Microsoft::WRL::ComPtr<ID3D12Resource> fontVertexBuffer[frameBufferCount];
	D3D12_VERTEX_BUFFER_VIEW fontVertexBufferView[frameBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> fontIndexBuffer;
	D3D12_INDEX_BUFFER_VIEW fontIndexBufferView;
	void* fontVertexBufferGPUAddress[frameBufferCount];
};

bool LoadFont(Font& font, const wchar_t* filename, float screenWidth, float screenHeight);
