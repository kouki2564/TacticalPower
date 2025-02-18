#pragma once
#include "DxLib.h"

struct LightSet
{
	int handle = 0;

	VECTOR dir = VGet(0.0f, 0.0f, 0.0f);

	COLOR_F difColor = { 0.0f,0.0f,0.0f,1.0f };
	COLOR_F spcColor = { 0.0f,0.0f,0.0f,1.0f };
	COLOR_F ambColor = { 0.0f,0.0f,0.0f,1.0f };
};

class Light
{
public:
	Light();
	virtual ~Light();

	void Init(VECTOR cameraPos, VECTOR frontPos);
	void Update(VECTOR cameraPos, VECTOR frontPos);

private:
	LightSet m_floorDirLight;
	LightSet m_cameraDirLight;
};