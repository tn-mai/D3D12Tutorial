/**
* @file PixelShader.hlsl
*/

/**
* �s�N�Z���V�F�[�_�֓��͂���t�H�[�}�b�g�̒�`.
* �΂ɂȂ钸�_�V�F�[�_�ł�������`���g�p���邱��.
*/
struct PSInput {
	float4 pos : SV_POSITION;
	float4 col : COLOR;
};

/**
* �ΐF��Ԃ������̃s�N�Z���V�F�[�_.
*/
float4 main(PSInput input) : SV_TARGET
{
	return input.col;
}