/* Copyright (c) 2017 Jin Li, http://www.luvfight.me

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE. */

#pragma once

#include "Event/EventQueue.h"
#include <random>

struct SDL_Window;
union SDL_Event;

NS_DOROTHY_BEGIN

typedef Delegate<void(const SDL_Event&)> SDLEventHandler;

class Application
{
public:
	virtual ~Application() { }
	PROPERTY_READONLY(Uint32, Frame);
	PROPERTY_READONLY(int, Width);
	PROPERTY_READONLY(int, Height);
	PROPERTY_READONLY(double, LastTime);
	PROPERTY_READONLY(double, DeltaTime);
	PROPERTY_READONLY(double, EclapsedTime);
	PROPERTY_READONLY(double, CPUTime);
	PROPERTY_READONLY(double, TotalTime);
	PROPERTY_READONLY(const Slice, Platform);
	PROPERTY_READONLY_CALL(Uint32, Rand);
	PROPERTY_READONLY(Uint32, RandMin);
	PROPERTY_READONLY(Uint32, RandMax);
	PROPERTY_READONLY(SDL_Window*, SDLWindow);
	PROPERTY(Uint32, MaxFPS);
	PROPERTY(Uint32, MinFPS);
	PROPERTY(Uint32, Seed);
	PROPERTY_BOOL(FPSLimited);
	SDLEventHandler eventHandler;
	int run();
	void shutdown();
	void invokeInRender(const function<void()>& func);
	void invokeInLogic(const function<void()>& func);
	static int mainLogic(void* userData);
#if BX_PLATFORM_WINDOWS
	inline void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}
	inline void operator delete(void* p)
	{
		_mm_free(p);
	}
#endif // BX_PLATFORM_WINDOWS
protected:
	Application();
	void updateDeltaTime();
	void updateWindowSize();
	void makeTimeNow();
	void setupSdlWindow();
	bgfx::RenderFrame::Enum renderFrame();
private:
	bool _fpsLimited;
	int _width;
	int _height;
	Uint32 _seed;
	Uint32 _maxFPS;
	Uint32 _minFPS;
	uint32_t _frame;
	const double _frequency;
	double _lastTime;
	double _deltaTime;
	double _cpuTime;
	double _totalTime;
	bx::Thread _logicThread;
	EventQueue _logicEvent;
	EventQueue _renderEvent;
	SDL_Window* _sdlWindow;
	std::mt19937 _randomEngine;
	SINGLETON_REF(Application, LuaEngine);
};

#define SharedApplication \
	Dorothy::Singleton<Dorothy::Application>::shared()

class BGFXDora
{
public:
	bool init();
	virtual ~BGFXDora();
	SINGLETON_REF(BGFXDora);
};

#define SharedBGFX \
	Dorothy::Singleton<Dorothy::BGFXDora>::shared()

NS_DOROTHY_END
