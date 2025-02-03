struct PS_INPUT
{
    float4 Diffuse : COLOR0;        //�f�B�t���[�Y�J���[
    float4 Specular : COLOR1;       //�X�y�L�����J���[
    float4 TexCoords0_1 : TEXCOORD0;//xy:�e�N�X�`�����W zw:�T�u�e�N�X�`�����W
    float3 VPosition : TEXCOORD1;   //���W( �r���[��� )
    float3 VNormal : TEXCOORD2;     //�@��( �r���[��� )

    float1 Fog : TEXCOORD5;         //�t�H�O�p�����[�^( x )

    float4 Position : SV_POSITION;  //���W( �v���W�F�N�V������� )
    //���C�g�r���[�X�N���[����Ԃł̍��W��ǉ�
    float4 posInLVP : TEXCOORD3;    //���C�g�r���[�X�N���[����Ԃł̃s�N�Z���̍��W
};

SamplerState smp : register(s0);    //�f�B�t���[�Y�}�b�v�e�N�X�`��
Texture2D tex : register(t0);       //�f�B�t���[�Y�}�b�v�e�N�X�`��

Texture2D shadowMapSmp : register(t1); //�V���h�E�}�b�v�e�N�X�`��

//�萔�o�b�t�@(���C�g�̕���)
cbuffer ConstantBuffer : register(b0)
{
    float3 LightDir;
}

//�萔�o�b�t�@(�J�����̃^�[�Q�b�g)
cbuffer ConstantBuffer : register(b1)
{
    float3 CameraTarget;
}

//�萔�o�b�t�@(�J�����̈ʒu)
cbuffer ConstantBuffer : register(b2)
{
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

    //���̋��������C�g�����Ɩ@�������̓��ςŋ��߂�(�t�B�[���h�ɂ͏������߂Ɍ��𓖂Ă�)
    float bright = dot(lightDir, n) * 1.5f;
     
    //���̋�����i�K�ɕ�����
    bright = round(bright * 4) / 4;
    
    //�����l��ݒ�
    bright = max(bright, 0.6f);
    
    //����l��ݒ�
    bright = min(bright, 1.0f);
    
    //�e�̏���
    float2 uv = input.posInLVP.xy / input.posInLVP.w;
    uv = uv * float2(0.5f, 0.5f);
    uv += 0.5f;
    
    //�e�����]��h������
    uv.y *= -1;
    uv.y += 1.0f;
    
    //�e�̏���
    float4 shadowMap = float4(0.0f, 0.0f, 0.0f,1.0f);
    
    //�e�̃T���v�����O���s��
    shadowMap = shadowMapSmp.Sample(smp, uv);
    
    //�e���𔽉f�������F��Ԃ�
    ret.col.rgb = ret.col.rgb * shadowMap.rgb * bright;
    
    return ret;
}