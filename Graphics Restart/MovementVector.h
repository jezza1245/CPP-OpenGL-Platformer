#pragma once
#ifndef MOVEMENTVECTOR_H
#define MOVEMENTVECTOR_H
#include <cmath>

class MovementVector
{
public:
	float xPart, yPart;
	MovementVector()
	{
		xPart = 0;
		yPart = 0;
	}
	MovementVector(float x, float y)
	{
		xPart = x;
		yPart = y;
	}

	float getMagnitude() const
	{
		return sqrt(pow(xPart, 2) + pow(yPart, 2));
	}

	MovementVector operator+(const MovementVector& v) const
	{
		return {(xPart + v.xPart), (yPart + v.yPart)};
	}
	void operator+=(const MovementVector& v)
	{
		xPart += v.xPart;
		yPart += v.yPart;
	}
	MovementVector operator-(const MovementVector& v) const
	{
		return {(xPart - v.xPart), (yPart - v.yPart)};
	}
	MovementVector operator*(const MovementVector& v) const
	{
		return {(xPart * v.xPart), (yPart * v.yPart)};
	}
	MovementVector operator-() const {
		return {-xPart, -yPart};
	}
	MovementVector operator*(int num) const
	{
		return { (xPart *num), (yPart * num) };
	}

};

#endif