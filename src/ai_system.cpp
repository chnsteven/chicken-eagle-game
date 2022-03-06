// internal
#include "ai_system.hpp"

void AISystem::step(float elapsed_ms)
{
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: HANDLE BUG AI HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// You will likely want to write new functions and need to create
	// new data structures to implement a more sophisticated Bug AI.
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	Entity chicken = registry.players.entities[0];
	Motion& chicken_motion = registry.motions.get(chicken);
	
	// loop over entities with eatable component, namely chicken and bugs
	for (Entity entity : registry.eatables.entities) {
		// skip the players and entity that doesn't have motion component
		if (registry.players.has(entity) || !registry.motions.has(entity))
			continue;
		
		Motion& bug_motion = registry.motions.get(entity);
		// detection area is a box with epsilon width and height
		if (in_bug_range(bug_motion, chicken_motion)) {
			bugDT(bug_motion, chicken_motion);
		}
		// if not in range, let bug drops with default velocity in y
		else {
			bug_motion.velocity.y = 50.f;
		}
	}

	(void)elapsed_ms; // placeholder to silence unused warning until implemented

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: DRAW DEBUG INFO HERE on AI path
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// You will want to use the createLine from world_init.hpp
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

bool AISystem::in_bug_range(Motion& bug, Motion& chicken) {
	vec2 distance;
	distance.x = abs(bug.position.x - chicken.position.x);
	distance.y = abs(bug.position.y - chicken.position.y);
	return distance.x <= epsilon && distance.y <= epsilon;
}

// super simple decision tree, change the bug's velocities according to some chicken position
void AISystem::bugDT(Motion& bug, Motion& chicken) {
	vec2 velocity_before = bug.velocity;

	// bug is left chicken
	if (bug.position.x < chicken.position.x) {
		bug.velocity.x = -abs(velocity_before.x);
	}
	// bug is right chicken
	else if (bug.position.x > chicken.position.x) {
		bug.velocity.x = abs(velocity_before.x);
	}

	// bug is above chicken
	if (bug.position.y < chicken.position.y) {
		bug.velocity.y = -abs(velocity_before.y);
	}
	// bug is below chicken
	else if (bug.position.y > chicken.position.y) {
		bug.velocity.y = abs(velocity_before.y);
	}
}