// internal
#include "ai_system.hpp"

// Game configuration
// the diagonal distance of the window from one corner to another corner 
const float radius_of_window = sqrt(dot((float)window_width_px, (float)window_height_px));
// an epsilon is the distance at which the bug AI is triggered
const float bug_epsilon = radius_of_window / 4.f;
const float eagle_epsilon = radius_of_window / 3.f;

vec2 cal_distance(Motion& obj1, Motion& obj2) {
	return vec2(abs(obj1.position.x - obj2.position.x), abs(obj1.position.y - obj2.position.y));
}

void wander_around(Motion& eagle) {
	if (0.f <= eagle.position.y && eagle.position.y <= 100.f)
		eagle.velocity.y = abs(eagle.velocity.y);
	else if (window_height_px - 100.f <= eagle.position.y && eagle.position.y <= window_height_px)
		eagle.velocity.y = -abs(eagle.velocity.y);
	if (0.f <= eagle.position.x && eagle.position.x <= 100.f)
		eagle.velocity.x = abs(eagle.velocity.x);
	else if (window_height_px - 100.f <= eagle.position.x && eagle.position.x <= window_height_px)
		eagle.velocity.x = -abs(eagle.velocity.x);
}

bool in_bug_range(Motion& bug, Motion& chicken) {
	vec2 distance = cal_distance(bug, chicken);
	return distance.x <= bug_epsilon && distance.y <= bug_epsilon;
}
bool in_eagle_range(Motion& eagle, Motion& chicken) {
	vec2 distance = cal_distance(eagle, chicken);
	return distance.x <= eagle_epsilon && distance.y <= eagle_epsilon;
}

void AISystem::debug() {
	if (debugging.in_debug_mode || debugging.in_freeze_mode) {
		for (uint i = 0; i < registry.eatables.size(); i++) {
			Entity entity_i = registry.eatables.entities[i];
			Motion& motion_i = registry.motions.get(entity_i);

			// don't draw debugging visuals around debug lines
			if (registry.players.has(entity_i))
				continue;

			vec2 direction_vector_x = { motion_i.velocity.x, 10.f };
			vec2 direction_vector_y = { 10.f , motion_i.velocity.y };

			Entity direction_vector_line_x = createLine(motion_i.position + vec2(motion_i.velocity.x / 2.f, 0.f), direction_vector_x);
			Entity direction_vector_line_y = createLine(motion_i.position + vec2(0.f, motion_i.velocity.y / 2.f), direction_vector_y);
		}
		for (uint i = 0; i < registry.deadlys.size(); i++) {
			Entity entity_i = registry.deadlys.entities[i];
			Motion& motion_i = registry.motions.get(entity_i);

			// don't draw debugging visuals around debug lines
			if (registry.players.has(entity_i))
				continue;

			vec2 direction_vector_x = { motion_i.velocity.x, 10.f };
			vec2 direction_vector_y = { 10.f , motion_i.velocity.y };

			Entity direction_vector_line_x = createLine(motion_i.position + vec2(motion_i.velocity.x / 2.f, 0.f), direction_vector_x);
			Entity direction_vector_line_y = createLine(motion_i.position + vec2(0.f, motion_i.velocity.y / 2.f), direction_vector_y);
		}
	}
}
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
		bugDT(bug_motion, chicken_motion);
	}

	// loop over entities with deadly component, namely eagle
	for (Entity entity : registry.deadlys.entities) {
		// skip the entity that doesn't have motion component
		if (!registry.motions.has(entity))
			continue;
		Motion& eagle_motion = registry.motions.get(entity);
		// detection area is a box with epsilon width and height
		eagleDT(eagle_motion, chicken_motion, uniform_dist(rng) * registry.eatables.size());
	}

	(void)elapsed_ms; // placeholder to silence unused warning until implemented

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: DRAW DEBUG INFO HERE on AI path
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// You will want to use the createLine from world_init.hpp
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
}

// super simple decision tree, change the bug's velocities according to some chicken position
void AISystem::bugDT(Motion& bug, Motion& chicken) {
	// detection area is a box with epsilon width and height
	if (in_bug_range(bug, chicken)) {
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
	// if not in range, let bug drops with default velocity in y
	else {
		bug.velocity.y = 50.f;
	}
}

void AISystem::eagleDT(Motion& eagle, Motion& chicken, float random_float) {
	// detection area is a box with epsilon width and height
	// attempt to find the closest bug around the eagle, if found, move in opposite direction.
	// if not found and if chicken is in eagle's range, chase the chicken
	if (registry.eatables.size() > 0) {
		float weight = 0.5;
		int random_index = (int) random_float;
		Entity random_eatable = registry.eatables.entities[random_index];
		Motion& random_eatable_motion = registry.motions.get(random_eatable);
		Entity chicken = registry.players.entities[0];
		Motion& chicken_motion = registry.motions.get(chicken);
		vec2 direction_vector_to_random_eatable = normalize(eagle.position - random_eatable_motion.position);
		vec2 direction_vector_to_chicken = -normalize(eagle.position - chicken_motion.position);
		vec2 direction_vector;
		//vec2 distance = cal_distance(eagle, chicken);
		//float dist = sqrt(dot(distance.x, distance.y));
		if (in_eagle_range(eagle, random_eatable_motion) && in_eagle_range(eagle, random_eatable_motion)) {
			direction_vector = weight * direction_vector_to_chicken + (1 - weight) * direction_vector_to_random_eatable;
			eagle.velocity = normalize(direction_vector) * 100.f;
		}
		else if (in_eagle_range(eagle, random_eatable_motion)) {
			eagle.velocity = normalize(direction_vector_to_random_eatable) * 100.f;
		}
		else if (in_eagle_range(eagle, chicken_motion)) {
			eagle.velocity = normalize(direction_vector_to_chicken) * 100.f;
		}
		// if not in range, let bug drops with default velocity in y
		else
			wander_around(eagle);
	}
}