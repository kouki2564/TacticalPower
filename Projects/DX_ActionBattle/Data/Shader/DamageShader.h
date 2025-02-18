#pragma once
#include<memory>
#include<DxLib.h>


class Model;

class DamageShader
{
public:
	DamageShader(std::shared_ptr<Model> pModel);
	virtual ~DamageShader();

	void Update();	//�X�V����

	void DrawFirst1Frame();	//���f���^�C�v��1Frame�̎��̕`��
	void DrawFirst4Frame(); //���f���^�C�v��4Frame�̎��̕`��

	void DrawEnd();	//�`��I��

	void UseShader();//�V�F�[�_�[���g��

	//�V�F�[�_�[���g���Ă��邩�ǂ���
	bool GetUseShader() const { return m_isUseShader; }

private:
	int m_pixelShaderHandle;		//�s�N�Z���V�F�[�_�[�̃n���h��
	int m_vertexShader1FrameHandle;		//���_�V�F�[�_�[�̃n���h��
	int m_vertexShader4FrameHandle;		//���_�V�F�[�_�[�̃n���h��

	std::shared_ptr<Model> m_pModel;//���f��

	int m_graphHandle;		//�摜�n���h��
	
	int m_cbufferHandleTime;	//�萔�o�b�t�@�n���h��
	int m_cbufferHandleLightDir;	//�萔�o�b�t�@�n���h��
	int m_cbufferHandleCameraTarget;	//�萔�o�b�t�@�n���h��
	int m_cbufferHandleCameraPos;	//�萔�o�b�t�@�n���h��

	float* m_cbufferTime;	//�萔�o�b�t�@�������邽�߂̃R�s�[
	VECTOR* m_cbufferAddressLightDir;	//�萔�o�b�t�@�������邽�߂̃R�s�[
	VECTOR* m_cbufferAddressCameraTarget;	//�萔�o�b�t�@�������邽�߂̃R�s�[
	VECTOR* m_cbufferAddressCameraPos;	//�萔�o�b�t�@�������邽�߂̃R�s�[

	float m_colDensity;		//�F�̔Z��

	bool m_isUseShader;		//�V�F�[�_�[���g�����ǂ���
};

