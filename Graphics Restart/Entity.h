#pragma once
#ifndef ENTITY_H
#define ENTITY_H
#include "MovementVector.h"

class Entity
{
public:
	MovementVector vector;
	float x = 0, y = 0, radius = 1;

	virtual void draw() = 0;
	virtual void update(std::string layout) = 0;
};

#endif