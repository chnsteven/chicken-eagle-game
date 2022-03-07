#pragma once

#include <vector>

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
	// the diagonal distance of the window from one corner to another corner 
	const float radius_of_window = sqrt(dot((float)window_width_px, (float)window_height_px));
	// an epsilon is the distance at which the bug AI is triggered
	const float epsilon = radius_of_window / 4.f;
	// bug decision tree
	void bugDT(Motion& bug, Motion& chicken);
	// simple function to check if bug is epsilon close to the chicken
	bool in_bug_range(Motion& bug, Motion& chicken);
};