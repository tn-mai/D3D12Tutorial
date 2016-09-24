/**
* @file Texture.h
*/
#pragma once
#include <stdint.h>
#include <d3d12.h>
#include <wincodec.h>
#include <wrl/event.h>
#include <vector>

namespace Texture {

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