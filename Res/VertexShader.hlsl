/**
* @file VertexShader.hlsl
*/

/**
* 頂点シェーダへ入力するフォーマットの定義.
* D3D12_INPUT_ELEMENT_DESCの指定と等しくなるようにする.
*/
struct VSInput {
	float3 pos : POSITION;
	float4 col : COLOR;
};

/**
* ピクセルシェーダへ入力するフォーマットの定義.
* 対になるピクセルシェーダでも同じ定義を使用すること.
*/
struct PSInput {
	float4 pos : SV_POSITION;
	float4 col : COLOR;
};

/**
* 定数バッファ.
*/
cbuffer ConstantBuffer : register(b0)
{
	float4 color;
}

/**
* 入力された座標をそのまま出力する頂点シェーダ.
*/
PSInput main(VSInput input)
{
	PSInput result;
	result.pos = float4(input.pos, 1.0f);
	result.col = input.col * color;
	return result;
}