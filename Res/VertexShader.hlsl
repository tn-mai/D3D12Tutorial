/**
* @file VertexShader.hlsl
*/

/**
* ���͂��ꂽ���W�����̂܂܏o�͂��钸�_�V�F�[�_.
*/
float4 main(float3 pos : POSITION) : SV_POSITION
{
	return float4(pos, 1.0f);
}