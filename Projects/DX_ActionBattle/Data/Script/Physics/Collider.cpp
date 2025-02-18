#include "Collider.h"

Collider::Collider()
{
	form = ColliderForm::NONE;
	pos[0] = VZero();
	pos[1] = VZero();
	prefer = -1;
	radius = 0;
	// width = 0;
	// depth = 0;
	centerPos = VZero();
	mapHandle = -1;
}

void Collider::SetSphere(VECTOR _setPos, int _prefer, float _radius)
{
	form = ColliderForm::SPHERE;
	pos[0] = _setPos;
	prefer = _prefer;
	radius = _radius;
	centerPos = pos[0];
}

void Collider::SetCapsule(VECTOR _setPos1,VECTOR _setPos2, int _prefer, float _radius)
{
	form = ColliderForm::CAPSULE;
	pos[0] = _setPos1;
	pos[1] = _setPos2;
	prefer = _prefer;
	radius = _radius;
	centerPos = VScale(VSub(pos[1],pos[0]), 0.5f);
}

void Collider::SetMap(int _mapHandle, VECTOR _scale)
{
	form = ColliderForm::MAP;
	pos[0] = VZero();
	mapHandle = _mapHandle;
	prefer = 0;
	MV1SetupCollInfo(mapHandle, -1);
}

//void Collider::SetWALL(VECTOR pos, int prefer, float width, float height, float depth)
//{
//	m_form = ColliderForm::WALL;
//	m_pos[0] = pos;
//	m_prefer = prefer;
//	m_width = width;
//	m_height = height;
//	m_depth = depth;
//}

void Collider::ColliderUpdate(VECTOR _moveVec)
{
	pos[0] = VAdd(pos[0], _moveVec);
	pos[1] = VAdd(pos[1], _moveVec);
	centerPos = VAdd(pos[0], VScale(VSub(pos[1], pos[0]), 0.5f));
}
