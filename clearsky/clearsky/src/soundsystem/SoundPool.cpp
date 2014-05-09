#include "core/precompiled.h"
#include "soundsystem\SoundPool.h"


namespace clearsky
{

SoundPool::SoundPool()
{

}

SoundPool::~SoundPool()
{
	cleanUp();
}

const bool SoundPool::loaded(const locationStringType path)
{
  this->iter = index.find(path);

  return (iter != index.end());
}

const bool SoundPool::loadSound(const locationStringType path, const sndFileType fileType)
{
  if (loaded(path))
	  return true;

  if (fileType == FT_OGG)
  {
	  auto ptr = std::make_shared<OggVorbisLoader>(OggVorbisLoader());
	  if (!ptr->loadStaticSound(path))
	  {
		  ptr.reset();
		  return false;
	  }

	  this->index.insert(std::pair<locationStringType, std::shared_ptr<SoundLoader>>(path, ptr));
	  return true;
  }
  else if (fileType == FT_WAVE)
  {
	  auto ptr = std::make_shared<WaveLoader>(WaveLoader());
	  if (!ptr->loadStaticSound(path))
	  {
		  ptr.reset();
		  return false;
	  }

	  this->index.insert(std::pair<locationStringType, std::shared_ptr<SoundLoader>>(path, ptr));
	  return true;
  }
  else if (fileType == FT_UNKNOWN)
  {
	  LOG->logMsg(LT_WARNING, "Unknown file type, checking WAVE...");
	  bool ret = false;
	  // Test WAVE
	  ret = loadSound(path, FT_WAVE);
	  if (ret)
		  LOG->logMsg(LT_STATUS, "FileType is WAVE!");
	  else
	  {
		LOG->logMsg(LT_WARNING, "Last error can be ignored!");
		LOG->logMsg(LT_STATUS, "Checking OGG...");
		ret = loadSound(path, FT_OGG);
	  }
	  if (ret)
	  {
		  LOG->logMsg(LT_STATUS, "FileType is OGG!");
	  }
	  else
	  {
		  LOG->logMsg(LT_ERROR, "Invalid sound file type!");
	  }
	  return ret;
  }

  return false;

}

const bool SoundPool::loadSound(const locationStringType path, const sndFileType fileType, Sound* const target)
{
	if (!target)
	{
		LOG->logMsg(LT_ERROR, "Invalid pointer in loadSound method.");
		return false;
	}

	target->setLoc(path);

	if (!this->loadSound(path, fileType))
		return false;

	this->iter = index.find(path);
	if (this->iter != this->index.end())
	{
		this->iter->second->setTargetSound(target);
		return this->iter->second->submitStaticSound();
	}

	return false;
}

const bool SoundPool::loadSound(const locationStringType path, Sound* target)
{
	sndFileType type = this->strToFileType(path);
	if (type)
		return loadSound(path, type, target);

	return false;
}

const bool SoundPool::loadSound(const locationStringType path)
{
	sndFileType type = this->strToFileType(path);
	if (type)
		return loadSound(path, type);

	return false;
}

const bool SoundPool::reloadSound(Sound* target)
{
	this->iter = index.find(target->getLoc());
	if (this->iter != this->index.end())
	{
		this->iter->second->setTargetSound(target);
		return this->iter->second->submitStaticSound();
	}
	LOG->logMsg(LT_ERROR, "Error Code: NF1000 - Sound not reloadable!");
	return false;
}

void SoundPool::deleteSound(const locationStringType path)
{
	if (loaded(path))
	{
		this->iter = index.find(path);
		//SAFE_DELETE(this->iter->second);
		this->index.erase(this->iter);
	}
}

sndFileType SoundPool::strToFileType(const locationStringType path)
{
	if (path.length() < 2)
		return FT_INVALID;

	if (path[path.length()-2] == 'g' || path[path.length()-1] == 'g' || path[path.length()-2] == 'G' || path[path.length()-2] == 'G')
		return FT_OGG;
	else if (path[path.length()-1] == 'e' || path[path.length()-1] == 'E' || path[path.length()-1] == 'v' || path[path.length()-1] == 'V')
		return FT_WAVE;
	else
		return FT_UNKNOWN;
}

void SoundPool::cleanUp()
{
	this->index.clear();
}

const unsigned int SoundPool::memUsage()
{
	unsigned int sum = 0;
	this->iter = index.begin();
	while (this->iter != index.end())
	{
		if (this->iter->second)
			sum += this->iter->second->bufferSize();
		this->iter++;
	}
	return sum;
}

} //namespace clearsky