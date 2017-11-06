#pragma once

#include <glm/vec3.hpp>
#include <GLM/gtc/matrix_access.hpp>
#include <GLM/gtx/string_cast.hpp>
#include <iostream>

namespace Math
{
	// Linear interpolation
	template <typename T>
	T lerp(T d0, T d1, float t)
	{
		return (1 - t) * d0 + d1 * t;
	}

	// inverse lerp
	// Regular lerp: given p0, p1 and a t value you get a point p between p0 and p1
	// Inverse lerp: given p0, p1 and p between p0 and p1 you will get the t value to compute p
	template <typename T>
	float invLerp(T d, T d0, T d1)
	{
		return (d - d0) / (d1 - d0);
	}

	
	
	// Returns a point between p1 and p2
	// This function gives the *exact* same result as the one above,
	// it comes from expanding and simplifying the UMP formula (see the example on BlackBoard)
	// This method is significantly faster than the one above.
	template <typename T>
	T catmull(T p0, T p1, T p2, T p3, float t)
	{
		return	((p1 * 2.0f) + (-p0 + p2) * t +
			((p0 * 2.0f) - (p1 * 5.0f) + (p2 * 4.0f) - p3) * (t * t) +
			(-p0 + (p1 * 3.0f) - (p2 * 3.0f) + p3) * (t * t * t)) * 0.5f;
	}

	template <typename T>
	T invCatmull(T p, T p1, T p2 )
	{
		return (p - p1) / (p2 - p1);
	}
	
}