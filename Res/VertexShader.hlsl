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
	float2 texCoord : TEXCOORD;
};

/**
* ピクセルシェーダへ入力するフォーマットの定義.
* 対になるピクセルシェーダでも同じ定義を使用すること.
*/
struct PSInput {
	float4 pos : SV_POSITION;
	float4 col : COLOR;
	float2 texCoord : TEXCOORD;
};

/**
* 定数バッファ.
*/
cbuffer ConstantBuffer : register(b0)
{
	float4x4 matWVP;
}

/**
* 入力された座標をそのまま出力する頂点シェーダ.
*/
PSInput main(VSInput input)
{
	PSInput result;
	result.pos = mul(float4(input.pos, 1.0f), matWVP);
	result.col = input.col;
	result.texCoord = input.texCoord;
	return result;
}