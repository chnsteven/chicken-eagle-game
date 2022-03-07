#pragma once

#include <vector>
#include <math.h> 
#include "tiny_ecs_registry.hpp"
#include "common.hpp"
#include "world_init.hpp"

// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
// DON'T WORRY ABOUT THIS CLASS UNTIL ASSIGNMENT 3
// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

class AISystem
{
public:
	void debug();
	void step(float elapsed_ms);
private:
	// bug decision tree
	void bugDT(Motion& bug, Motion& chicken);
	void eagleDT(Motion& eagle, Motion& chicken);
};