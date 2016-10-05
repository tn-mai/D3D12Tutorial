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
		Loader();
		~Loader() {}

		bool LoadFromFile(const wchar_t* filename, std::vector<uint8_t>& imageData, D3D12_RESOURCE_DESC& desc, int& bytesPerRow);

	private:
		Microsoft::WRL::ComPtr<IWICImagingFactory> factory;
	};

	/**
	* �e�N�X�`���Ǘ��N���X.
	*/
	class Manager
	{
	public:
		typedef Microsoft::WRL::ComPtr<ID3D12Resource> ResourcePtr;
		static const size_t maxTextureCount = 10;

		Manager();
		bool Initialize(Microsoft::WRL::ComPtr<ID3D12Device> device);
		bool LoadFromFile(Microsoft::WRL::ComPtr<ID3D12Device> device, const wchar_t* filename);
		void Unload(const wchar_t* filename);
		const ID3D12CommandList* GetCommandList() const;
		void ClearUploadBuffer();
		D3D12_GPU_DESCRIPTOR_HANDLE GetTextureHandle(const wchar_t* filename) const;

	private:
		struct TextureInfo {
			ResourcePtr buffer;
			D3D12_GPU_DESCRIPTOR_HANDLE srvHandle;
		};
		std::map<std::wstring, TextureInfo> textureList;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvDescriptorHeap;
		std::vector<int> freeDescriptorList;
		int srvDescriptorSize;
		Loader loader;
		std::vector<ResourcePtr> uploadBufferList;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList;
	};

} // namespace Texture
