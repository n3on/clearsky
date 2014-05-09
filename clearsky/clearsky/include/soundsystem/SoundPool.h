#pragma once
#ifndef _CLEARSKY_SOUND_POOL_H_
#define _CLEARSKY_SOUND_POOL_H_

#include "soundsystem\XAudioIncludes.h"
#include "soundsystem\SoundLoader.h"

namespace clearsky
{
	enum sndFileType
	{
		FT_INVALID = 0,
		FT_OGG,
		FT_WAVE,
		FT_UNKNOWN
	};

	class CLEARSKY_API SoundPool
	{
	public:

		SoundPool();
		virtual ~SoundPool();

		const bool loadSound(const locationStringType path, const sndFileType fileType);

		const bool loadSound(const locationStringType path, const sndFileType fileType, Sound* target);

		const bool loadSound(const locationStringType path, Sound* target);

		const bool loadSound(const locationStringType path);

		const bool reloadSound(Sound* target);

		void deleteSound(const locationStringType path);

		void cleanUp();

		const unsigned int memUsage();

	protected:

		const bool loaded(const locationStringType path);

		sndFileType strToFileType(const locationStringType path);

		std::map<locationStringType, std::shared_ptr<SoundLoader>> index;
		std::map<locationStringType, std::shared_ptr<SoundLoader>>::iterator iter;
	};

}

#endif