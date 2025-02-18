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

cbuffer ConstantBuffer : register(b0)
{
    //���C�g�̌�����ݒ�
    float3 LightDir;
}

cbuffer ConstantBuffer : register(b1)
{
    //�J�����̌����Ă��������ݒ�
    float3 CameraTarget;
}

cbuffer ConstantBuffer : register(b2)
{
    //�J�����̈ʒu��ݒ�
    float3 CameraPos;
}

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
    float3 lightDir = LightDir;
    lightDir = normalize(lightDir);

    //�@�����𐳋K��
    float3 n = normalize(input.VNormal);

    //���̋��������C�g�����Ɩ@�������̓��ςŋ��߂�
    float bright = dot(lightDir, n);
    
    //���̋�����i�K�ɕ�����
    bright = round(bright * 4) / 4;
    
    //�����l��ݒ�
    bright = max(bright, 0.4f);

    //���Ƃ̐F�Ɍ��̋�����������
    ret.col.rgb *= bright;
    
    //�J�����̌����Ă�����������߂�
    float3 cameraDir = normalize(CameraTarget - CameraPos);
    
    //�@���̌����ƃJ�����̌����Ń������C�g�̋��������߂�
    float rim = dot(n, cameraDir);
    
    //�������C�g�̋�����-0.25~0.25�̊Ԃ̏ꍇ
    if (rim > -0.25f && rim < 0.25f)
    {
        ret.col.rbg = 0.05f;
    }
    
    return ret;
}