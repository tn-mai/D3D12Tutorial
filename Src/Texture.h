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
* �e�N�X�`���֘A�̋@�\��l���i�[���閼�O���.
*/
namespace Texture {

	/**
	* �e�N�X�`���ǂݍ��݃N���X.
	*
	* �R���X�g���N�^�ŏ���������A�f�X�g���N�^�Ŕj�������.
	* �������Ɏ��s�����ꍇ�͗�O����������.
	* �������ɐ��������̂��ALoadFromFile()�֐��Ńe�N�X�`�����o�C�g��Ƃ��ēǂݍ���.
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
	* �e�N�X�`���Ǘ��N���X.
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
