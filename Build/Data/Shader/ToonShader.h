#pragma once
#include <DxLib.h>

class ToonShader
{
public:
	ToonShader();
	virtual ~ToonShader();

	void Update();	//�X�V����

	void SetShader(int shaderType);
	void SetShaderField(int shaderType);
	void EndShader();

private:
	int m_vertexShader1FrameHandle;				//���_�V�F�[�_�[1�t���[���n���h��
	int m_vertexShader4FrameHandle;				//���_�V�F�[�_�[4�t���[���n���h��
	int m_vertexShader8FrameHandle;				//���_�V�F�[�_�[8�t���[���n���h��
	int m_vertexShaderNormal4FrameHandle;		//���_�V�F�[�_�[4�t���[���@���}�b�v����n���h��
	int m_vertexShaderNormal8FrameHandle;		//���_�V�F�[�_�[8�t���[���@���}�b�v����n���h��
	int m_pixelShaderHandle;		//�s�N�Z���V�F�[�_�̃n���h��
	int m_pixelShaderFieldHandle;	//�s�N�Z���V�F�[�_�[�̃t�B�[���h�ɓK�p������n���h��

	int m_graphHandle;		//�摜�n���h��

	int m_cbufferHandleLightDir;	//�萔�o�b�t�@�n���h��
	int m_cbufferHandleCameraTarget;	//�萔�o�b�t�@�n���h��
	int m_cbufferHandleCameraPos;	//�萔�o�b�t�@�n���h��

	VECTOR* m_cbufferAddressLightDir;	//�萔�o�b�t�@�������邽�߂̃R�s�[
	VECTOR* m_cbufferAddressCameraTarget;	//�萔�o�b�t�@�������邽�߂̃R�s�[
	VECTOR* m_cbufferAddressCameraPos;	//�萔�o�b�t�@�������邽�߂̃R�s�[
};

