#pragma once

#include "resource.h"

void StartTimedRedraw(UINT millis);
void StopTimedRedraw();
void ForceRedraw();
void ForceResize();
RECT GetClientRect();
