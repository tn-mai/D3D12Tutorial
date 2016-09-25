/**
* @file PixelShader.hlsl
*/

Texture2D t1 : register(t0);
SamplerState s1 : register(s0);

/**
* ピクセルシェーダへ入力するフォーマットの定義.
* 対になる頂点シェーダでも同じ定義を使用すること.
*/
struct PSInput {
	float4 pos : SV_POSITION;
	float4 col : COLOR;
	float2 texCoord : TEXCOORD;
};

/**
* 緑色を返すだけのピクセルシェーダ.
*/
float4 main(PSInput input) : SV_TARGET
{
	return t1.Sample(s1, input.texCoord) * input.col;
}