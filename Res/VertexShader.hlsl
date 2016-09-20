/**
* @file VertexShader.hlsl
*/

/**
* ���_�V�F�[�_�֓��͂���t�H�[�}�b�g�̒�`.
* D3D12_INPUT_ELEMENT_DESC�̎w��Ɠ������Ȃ�悤�ɂ���.
*/
struct VSInput {
	float3 pos : POSITION;
	float4 col : COLOR;
};

/**
* �s�N�Z���V�F�[�_�֓��͂���t�H�[�}�b�g�̒�`.
* �΂ɂȂ�s�N�Z���V�F�[�_�ł�������`���g�p���邱��.
*/
struct PSInput {
	float4 pos : SV_POSITION;
	float4 col : COLOR;
};

/**
* �萔�o�b�t�@.
*/
cbuffer ConstantBuffer : register(b0)
{
	float4 color;
}

/**
* ���͂��ꂽ���W�����̂܂܏o�͂��钸�_�V�F�[�_.
*/
PSInput main(VSInput input)
{
	PSInput result;
	result.pos = float4(input.pos, 1.0f);
	result.col = input.col * color;
	return result;
}