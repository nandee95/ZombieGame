#include "../System/Transitions.hpp"

float Transition::linear(float percent, float min, float max)
{
	return min + (max - min)*percent;
}

float Transition::bezierCubics(float percent, float p0, float p1, float p2, float p3)
{
	const float yb = linear(percent, p1, p2);
	return linear(percent, linear(percent, linear(percent, p0, p1), yb), linear(percent, yb, linear(percent, p2, p3)));
}

float Transition::bezierQuadratic(float percent, float p0, float p1, float p2)
{
	return linear(percent, linear(percent, p0, p1), linear(percent, p1, p2));
}
