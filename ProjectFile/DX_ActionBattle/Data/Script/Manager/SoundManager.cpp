#include "SoundManager.h"

SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
}

void SoundManager::registerSound(std::string name, int handle, int vol, bool isRoop)
{
	Sound sound;
	sound.handle = handle;
	sound.vol = vol;
	sound.isRoop = isRoop;
	m_sounds.insert(std::make_pair(name, sound));
}

void SoundManager::OnePlay(std::string name)
{
	
}

void SoundManager::loopPlay(std::string name)
{

}

void SoundManager::Stop(std::string name)
{

}

void SoundManager::StopAll()
{

}
