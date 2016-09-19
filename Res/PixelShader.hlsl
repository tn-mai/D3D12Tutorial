/**
* @file PixelShader.hlsl
*/

/**
* ピクセルシェーダへ入力するフォーマットの定義.
* 対になる頂点シェーダでも同じ定義を使用すること.
*/
struct PSInput {
	float4 pos : SV_POSITION;
	float4 col : COLOR;
};

/**
* 緑色を返すだけのピクセルシェーダ.
*/
float4 main(PSInput input) : SV_TARGET
{
	return input.col;
}