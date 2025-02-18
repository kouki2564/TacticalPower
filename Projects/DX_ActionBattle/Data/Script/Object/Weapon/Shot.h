﻿#pragma once
#include "DxLib.h"
#include "Collider.h"

class Shot
{
public:
	Shot() {}
	~Shot() {}

	void SetUp(int id, int handle, VECTOR startPos, VECTOR targetPos, int time, VECTOR baseDir, VECTOR shotDir, float shotSize);

	void Update();

	void UpdateTargetPos(VECTOR targetPos);

	VECTOR GetPos() { return m_pos; }

	bool GetIsApp() { return m_isApp; }

private:

	int m_handle;
	VECTOR m_pos;
	VECTOR m_targetPos;
	VECTOR m_shotDir;

	VECTOR m_firstDirVec;
	VECTOR m_baseDir;

	VECTOR m_startPos;

	float m_scale;

	int m_id = -1;

	int m_time = 0;
	int m_maxTime = 0;
	bool m_isApp = true;

	bool m_isTargetStart = false;
	bool m_isTargetEnd = false;
};

