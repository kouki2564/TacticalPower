#pragma once
#include<memory>
#include<DxLib.h>

class Model;

class DeadShader
{
public:
	DeadShader();
	virtual ~DeadShader();

	void Update();	//�X�V����

	void OnShader();//�V�F�[�_�[���g��
	void SetShader(int shaderType);	//�`�揈��
	void EndShader();

	//�V�F�[�_�[���g���Ă��邩�ǂ���
	bool GetUseShader() const { return m_isUseShader; }

private:
	int m_vertexShader1FrameHandle;				//���_�V�F�[�_�[1�t���[���n���h��
	int m_vertexShader4FrameHandle;				//���_�V�F�[�_�[4�t���[���n���h��
	int m_vertexShader8FrameHandle;				//���_�V�F�[�_�[8�t���[���n���h��
	int m_vertexShaderNormal4FrameHandle;		//���_�V�F�[�_�[4�t���[���@���}�b�v����n���h��
	int m_vertexShaderNormal8FrameHandle;		//���_�V�F�[�_�[8�t���[���@���}�b�v����n���h��
	int m_pixelShaderHandle;		//�s�N�Z���V�F�[�_�̃n���h��

	int m_cbufferHandle;	//�萔�o�b�t�@�n���h��

	float* m_cbufferAddress;	//�萔�o�b�t�@�������邽�߂̃R�s�[

	float m_colDensity;		//�F�̔Z��

	bool m_isUseShader;		//�V�F�[�_�[���g�����ǂ���
};

