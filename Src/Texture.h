/**
* @file Texture.h
*/
#pragma once
#include <stdint.h>
#include <d3d12.h>
#include <wincodec.h>
#include <wrl/event.h>
#include <vector>

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
		Loader();
		~Loader() {}

		bool LoadFromFile(const wchar_t* filename, std::vector<uint8_t>& imageData, D3D12_RESOURCE_DESC& desc, int& bytesPerRow);

	private:
		Microsoft::WRL::ComPtr<IWICImagingFactory> factory;
	};

} // namespace Texture