#include <SDL.h>
#include "Time.h"

double Time::deltaTime = 0;
Uint64 Time::NOW = 0;
Uint64 Time::LAST = 0;

void Time::Calculate(void)
{
    LAST = NOW;
    SetNow(SDL_GetPerformanceCounter());
    deltaTime = (double)((NOW - LAST) / (double)SDL_GetPerformanceFrequency());
}

void Time::SetNow(Uint64 _NOW)
{
    NOW = _NOW;
}

double Time::GetDeltaTime(void)
{
    return deltaTime;
}