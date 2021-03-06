/* Copyright (c) 2017 Jin Li, http://www.luvfight.me

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#include "Const/Header.h"
#include "Cache/SoundCache.h"
#include "Audio/Sound.h"
#include "Basic/Content.h"

NS_DOROTHY_BEGIN

SoundFile* SoundCache::update(String name, SoundFile* soundFile)
{
	string fullPath = SharedContent.getFullPath(name);
	_soundFiles[fullPath] = soundFile;
	return soundFile;
}

SoundFile* SoundCache::get(String filename)
{
	string fullPath = SharedContent.getFullPath(filename);
	auto it = _soundFiles.find(fullPath);
	if (it != _soundFiles.end())
	{
		return it->second;
	}
	return nullptr;
}

SoundFile* SoundCache::load(String filename)
{
	string fullPath = SharedContent.getFullPath(filename);
	auto it = _soundFiles.find(fullPath);
	if (it != _soundFiles.end())
	{
		return it->second;
	}
	string ext = filename.getFileExtension();
	switch (Switch::hash(ext))
	{
		case "wav"_hash:
		case "ogg"_hash:
		{
			auto data = SharedContent.loadFile(fullPath);
			SoundFile* soundFile = SoundFile::create(std::move(data));
			_soundFiles[fullPath] = soundFile;
			return soundFile;
		}
		default:
		{
			Log("file named \"%s\" is not soundFile file.", filename);
			return nullptr;
		}
	}
}

SoundFile* SoundCache::update(String filename, const Uint8* data, Sint64 size)
{
	string fullPath = SharedContent.getFullPath(filename);
	SoundFile* soundFile = SoundFile::create(MakeOwnArray(c_cast<Uint8*>(data), s_cast<size_t>(size)));
	_soundFiles[fullPath] = soundFile;
	return soundFile;
}

void SoundCache::loadAsync(String filename, const function<void(SoundFile*)>& handler)
{
	string ext = filename.getFileExtension();
	switch (Switch::hash(ext))
	{
		case "wav"_hash:
		case "ogg"_hash:
		{
			string fullPath = SharedContent.getFullPath(filename);
			SharedContent.loadFileAsyncUnsafe(fullPath, [this, fullPath, handler](Uint8* data, Sint64 size)
			{
				SoundFile* soundFile = SoundFile::create(MakeOwnArray(data, s_cast<size_t>(size)));
				_soundFiles[fullPath] = soundFile;
				handler(soundFile);
			});
			break;
		}
		default:
		{
			Log("file named \"%s\" is not soundFile file.", filename);
			handler(nullptr);
			break;
		}
	}
}

bool SoundCache::unload(SoundFile* soundFile)
{
	for (const auto& it : _soundFiles)
	{
		if (it.second == soundFile)
		{
			_soundFiles.erase(_soundFiles.find(it.first));
			return true;
		}
	}
	return false;
}

bool SoundCache::unload(String filename)
{
	string fullPath = SharedContent.getFullPath(filename);
	auto it = _soundFiles.find(fullPath);
	if (it != _soundFiles.end())
	{
		_soundFiles.erase(it);
		return true;
	}
	return false;
}

bool SoundCache::unload()
{
	if (_soundFiles.empty())
	{
		return false;
	}
	_soundFiles.clear();
	return true;
}

void SoundCache::removeUnused()
{
	vector<unordered_map<string,Ref<SoundFile>>::iterator> targets;
	for (auto it = _soundFiles.begin();it != _soundFiles.end();++it)
	{
		if (it->second->isSingleReferenced())
		{
			targets.push_back(it);
		}
	}
	for (const auto& it : targets)
	{
		_soundFiles.erase(it);
	}
}

NS_DOROTHY_END
