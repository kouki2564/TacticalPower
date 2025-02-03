#pragma once
#include <string>
#include <map>

struct Sound
{
	// サウンドハンドル
	int handle;
	// 音量
	int vol;
	// ループ有無
	bool isRoop;
};

class SoundManager
{
public:
	SoundManager();
	~SoundManager();

	//インスタンスを返す
	static SoundManager& Instance() {
		static SoundManager instance;
		return instance;
	}

	void registerSound(std::string name, int handle, int vol, bool isRoop);

	void OnePlay(std::string name);

	void loopPlay(std::string name);

	void Stop(std::string name);

	void StopAll();


private:

	std::map<std::string, Sound> m_sounds;

};

