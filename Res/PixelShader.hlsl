/**
* @file PixelShader.hlsl
*/

Texture2D t1 : register(t0);
SamplerState s1 : register(s0);

/**
* �s�N�Z���V�F�[�_�֓��͂���t�H�[�}�b�g�̒�`.
* �΂ɂȂ钸�_�V�F�[�_�ł�������`���g�p���邱��.
*/
struct PSInput {
	float4 pos : SV_POSITION;
	float4 col : COLOR;
	float2 texCoord : TEXCOORD;
};

/**
* �ΐF��Ԃ������̃s�N�Z���V�F�[�_.
*/
float4 main(PSInput input) : SV_TARGET
{
	return t1.Sample(s1, input.texCoord) * input.col;
}