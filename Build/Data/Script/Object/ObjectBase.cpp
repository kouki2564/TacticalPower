#include "ObjectBase.h"
#include <cassert>

void ObjectBase::SetUpObject(std::string name, int handle, VECTOR pos, VECTOR dir, VECTOR scale)
{
	DrawingManager::Instance().RegisterModel(name, handle,pos,dir,scale);
}

void ObjectBase::FixPos()
{
	int x = m_pos.x * 100;
	int y = m_pos.y * 100;
	int z = m_pos.z * 100;

	if ((float)x + 0.5f <= m_pos.x)
	{
		x++;
	}
	if ((float)y + 0.5f <= m_pos.y)
	{
		y++;
	}
	if ((float)z + 0.5f <= m_pos.z)
	{
		z++;
	}

	m_pos = VGet((float)(x * 0.01f), (float)(y * 0.01f), (float)(z * 0.01f));
}