/**
* @file VertexShader.hlsl
*/

/**
* 入力された座標をそのまま出力する頂点シェーダ.
*/
float4 main(float3 pos : POSITION) : SV_POSITION
{
	return float4(pos, 1.0f);
}