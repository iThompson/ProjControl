#pragma once

#include "resource.h"

void StartTimedRedraw(int millis);
void StopTimedRedraw();
void ForceRedraw();
void ForceResize();
RECT GetClientRect();
