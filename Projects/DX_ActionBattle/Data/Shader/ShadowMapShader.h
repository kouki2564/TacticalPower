#pragma once
#include<DxLib.h>

class ShadowMapShader
{
public:
	ShadowMapShader();
	~ShadowMapShader();
	
	//�V���h�E�}�b�v�ւ̏������݊J�n
	void StartWrite(VECTOR targetPos);
	
	//�V���h�E�}�b�v�̃J�������猩���^�[�Q�b�g��ݒ�
	void SetShadowMapCameraTarget(VECTOR targetPos);

	void SetShader(int shaderType);
	void SetShaderField(VECTOR targetPos);

	//�V���h�E�}�b�v�ւ̏������ݏI���
	void EndWrite();



private:
	//�r���[�v���W�F�N�V�����s�����邽�߂̊֐�
	MATRIX ViewProjectionMatrix(VECTOR targetPos);
private:
	int m_vertexShader1FrameHandle;				//���_�V�F�[�_�[1�t���[���n���h��
	int m_vertexShader4FrameHandle;				//���_�V�F�[�_�[4�t���[���n���h��
	int m_vertexShader8FrameHandle;				//���_�V�F�[�_�[8�t���[���n���h��
	int m_vertexShaderNormal4FrameHandle;		//���_�V�F�[�_�[4�t���[���@���}�b�v����n���h��
	int m_vertexShaderNormal8FrameHandle;		//���_�V�F�[�_�[8�t���[���@���}�b�v����n���h��
	
	//���_�V�F�[�_�[�̃V���h�E�}�b�v���t�B�[���h�ɒ���t���邽�߂̒��_�V�F�[�_�[
	int m_vertexShaderShadowMapField;		
	int m_pixelShaderHandle;		//�s�N�Z���V�F�[�_�̃n���h��
	int m_pixelShaderFieldHandle;	//�s�N�Z���V�F�[�_�[�̃t�B�[���h�ɓK�p������n���h��

	int m_cbufferHandleLightDir;	//�萔�o�b�t�@�n���h��
	int m_cbufferHandleCameraTarget;	//�萔�o�b�t�@�n���h��
	int m_cbufferHandleCameraPos;	//�萔�o�b�t�@�n���h��
	int m_cbufferHandleViewProjectionMat;	//�萔�o�b�t�@�n���h��

	VECTOR* m_cbufferAddressLightDir;	//�萔�o�b�t�@�������邽�߂̃R�s�[
	VECTOR* m_cbufferAddressCameraTarget;	//�萔�o�b�t�@�������邽�߂̃R�s�[
	VECTOR* m_cbufferAddressCameraPos;	//�萔�o�b�t�@�������邽�߂̃R�s�[

	//�r���[�v���W�F�N�V�����s��
	//�V���h�E�}�b�v�e�N�X�`�����t�B�[���h���f��
	//�ɒ���t����ۂɍ��W�̕ϊ����s�����߂ɕK�v
	MATRIX* m_viewProjectionMat;
	int m_shadowMap = -1;	//�V���h�E�}�b�v�p�̕ϐ�
};

