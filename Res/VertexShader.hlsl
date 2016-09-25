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
* �萔�o�b�t�@.
*/
cbuffer ConstantBuffer : register(b0)
{
	float4x4 matWVP;
}

/**
* ���͂��ꂽ���W�����̂܂܏o�͂��钸�_�V�F�[�_.
*/
PSInput main(VSInput input)
{
	PSInput result;
	result.pos = mul(float4(input.pos, 1.0f), matWVP);
	result.col = input.col;
	result.texCoord = input.texCoord;
	return result;
}