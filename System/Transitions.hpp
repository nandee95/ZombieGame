#pragma once

#include "../System/Math.hpp"
#include <vector>

namespace Transition
{
	float linear(float percent, float min, float max);
	float bezierCubics(float percent, float p0, float p1, float p2, float p3);
	float bezierQuadratic(float percent, float p0, float p1, float p2);
}