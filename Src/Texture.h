/**
* @file Texture.h
*/
#pragma once
#include <stdint.h>
#include <d3d12.h>
#include <wincodec.h>
#include <wrl/event.h>
#include <vector>
#include <map>

class DescriptorHeapManager;
class ResourceLoader;

/**
* テクスチャ関連の機能や値を格納する名前空間.
*/
namespace Texture {

	/**
	* テクスチャ読み込みクラス.
	*
	* コンストラクタで初期化され、デストラクタで破棄される.
	* 初期化に失敗した場合は例外が投げられる.
	* 初期化に成功したのち、LoadFromFile()関数でテクスチャをバイト列として読み込む.
	*/
	class Loader
	{
	public:
		static void Initialize();
		static void Destroy();
		bool LoadFromFile(const wchar_t* filename, std::vector<uint8_t>& imageData, D3D12_RESOURCE_DESC& desc, int& bytesPerRow);

	private:
		static Microsoft::WRL::ComPtr<IWICImagingFactory> factory;
	};

	/**
	* テクスチャ管理クラス.
	*/
	class Manager
	{
	public:
		typedef Microsoft::WRL::ComPtr<ID3D12Resource> ResourcePtr;

		Manager();
		bool Initialize(DescriptorHeapManager* heap);
		bool LoadFromFile(ResourceLoader& resourceLoader, const wchar_t* filename);
		void Unload(const wchar_t* filename);
		D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(const wchar_t* filename) const;
		DXGI_FORMAT GetTextureFormat(const wchar_t* filename) const;

	private:
		struct TextureInfo {
			ResourcePtr buffer;
			DXGI_FORMAT format;
			D3D12_GPU_DESCRIPTOR_HANDLE srvHandle;
		};
		std::map<std::wstring, TextureInfo> textureList;
		Loader loader;
		DescriptorHeapManager* descriptorHeap;
	};

} // namespace Texture
