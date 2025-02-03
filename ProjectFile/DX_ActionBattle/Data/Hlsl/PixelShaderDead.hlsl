cbuffer ConstantBuffer : register(b0)
{
    //���邳��ς��邽�߂̕ϐ�
    float brightTime;
}

struct PS_INPUT
{
    float4 Diffuse : COLOR0; // �f�B�t���[�Y�J���[
    float4 Specular : COLOR1; // �X�y�L�����J���[
    float4 TexCoords0_1 : TEXCOORD0; // xy:�e�N�X�`�����W zw:�T�u�e�N�X�`�����W
    float3 VPosition : TEXCOORD1; // ���W( �r���[��� )
    float3 VNormal : TEXCOORD2; // �@��( �r���[��� )

    float1 Fog : TEXCOORD5; // �t�H�O�p�����[�^( x )

    float4 Position : SV_POSITION; // ���W( �v���W�F�N�V������� )
};

SamplerState smp : register(s0); // �f�B�t���[�Y�}�b�v�e�N�X�`��
Texture2D tex : register(t0); // �f�B�t���[�Y�}�b�v�e�N�X�`��


struct PS_OUTPUT
{
    float4 col : SV_TARGET0;
    float4 depth : SV_TARGET1;
    float4 norm : SV_TARGET2;
};

PS_OUTPUT main(PS_INPUT input)
{
    PS_OUTPUT ret;
    ret.depth = input.Position.zwyx;
    ret.col = input.Diffuse * tex.Sample(smp, input.TexCoords0_1.xy);
    ret.norm = float4(input.VNormal, 1.0f);
    
    //���C�g�̌�����ݒ�
    float3 lightDir = { 0.5f, 2.0f, 0.5f };
    lightDir = normalize(lightDir);

    //�@�����𐳋K��
    input.VNormal = normalize(input.VNormal);

    //���̋��������C�g�����Ɩ@�������̓��ςŋ��߂�
    float bright = dot(lightDir, input.VNormal);

   //���̋�����i�K�ɕ�����
    bright = round(bright * 4) / 4;
    
    //�����l��ݒ�
    bright = max(bright, 0.4f);

    //���Ƃ̐F�Ɍ��̋�����������
    ret.col.rgb *= bright;
    
    //���邳��ς���
    ret.col.rgb = ret.col.rgb * brightTime;
   
    return ret;
}