/**
* @file FontVertexShader.hlsl
*/

/**
* ���_�V�F�[�_�֓��͂���t�H�[�}�b�g�̒�`.
* D3D12_INPUT_ELEMENT_DESC�̎w��Ɠ������Ȃ�悤�ɂ���.
*/
struct VSInput {
	float2 pos : POSITION;
	float4 col : COLOR;
	float2 texCoord : TEXCOORD;
};

/**
* �s�N�Z���V�F�[�_�֓��͂���t�H�[�}�b�g�̒�`.
* �΂ɂȂ�s�N�Z���V�F�[�_�ł�������`���g�p���邱��.
*/
struct PSInput {
	float4 pos : SV_POSITION;
	float4 col : COLOR;
	float2 texCoord : TEXCOORD;
};

/**
* ���͂��ꂽ���W�����̂܂܏o�͂��钸�_�V�F�[�_.
*/
PSInput main(VSInput input)
{
	PSInput result;
	result.pos = float4(input.pos, 0.0f, 1.0f);
	result.col = input.col;
	result.texCoord = input.texCoord;
	return result;
}