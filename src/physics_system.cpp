// internal
#include "physics_system.hpp"
#include "world_init.hpp"
#include "math.h"

// Returns the local bounding coordinates scaled by the current size of the entity
vec2 get_bounding_box(const Motion& motion)
{
	// abs is to avoid negative scale due to the facing direction.
	return { abs(motion.scale.x), abs(motion.scale.y) };
}

// This is a SUPER APPROXIMATE check that puts a circle around the bounding boxes and sees
// if the center point of either object is inside the other's bounding-box-circle. You can
// surely implement a more accurate detection
bool collides(const Motion& motion1, const Motion& motion2)
{
	vec2 dp = motion1.position - motion2.position;
	float dist_squared = dot(dp,dp);
	const vec2 other_bonding_box = get_bounding_box(motion1) / 2.f;
	const float other_r_squared = dot(other_bonding_box, other_bonding_box);
	const vec2 my_bonding_box = get_bounding_box(motion2) / 2.f;
	const float my_r_squared = dot(my_bonding_box, my_bonding_box);
	const float r_squared = max(other_r_squared, my_r_squared);
	if (dist_squared < r_squared)
		return true;
	return false;
}

//bool collides(const Motion& motion1, const Motion& motion2)
//{
//	vec2 dp = motion1.position - motion2.position;
//	float dist_squared = dot(dp, dp);
//	const vec2 other_bonding_box = get_bounding_box(motion1) / 2.f;
//	const float other_radius = dot(other_bonding_box, other_bonding_box);
//	const vec2 my_bonding_box = get_bounding_box(motion2) / 2.f;
//	const float my_radius = dot(my_bonding_box, my_bonding_box);
//	return dist_squared < my_radius + other_radius;
//}

void PhysicsSystem::debug() {
	ComponentContainer<Motion>& motion_container = registry.motions;
	Entity chicken = registry.players.entities[0]; // assumed there's only 1 chicken, if multiple, needs an alternative
	Motion& chicken_motion = registry.motions.get(chicken);
	Mesh* chicken_mesh = registry.meshPtrs.get(chicken);

	Transform transform;
	transform.translate(chicken_motion.position);
	transform.rotate(-chicken_motion.angle);
	transform.scale(chicken_motion.scale);

	uint size_before_adding_new = (uint)motion_container.components.size();
	for (uint i = 0; i < size_before_adding_new; i++)
	{
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];

		// don't draw debugging visuals around debug lines
		if (registry.debugComponents.has(entity_i))
			continue;

		// !!! TODO A2: implement debug bounding boxes instead of crosses
		const vec2 bonding_box = get_bounding_box(motion_i);
		float radius = sqrt(dot(bonding_box / 2.f, bonding_box / 2.f));
		vec2 vertical_line = { motion_i.scale.x / 50.f, 2 * radius };
		vec2 horizontal_line = { 2 * radius, motion_i.scale.y / 50.f };
		Entity line1 = createLine(vec2(motion_i.position.x - radius, motion_i.position.y), vertical_line);
		Entity line2 = createLine(vec2(motion_i.position.x + radius, motion_i.position.y), vertical_line);
		Entity line3 = createLine(vec2(motion_i.position.x, motion_i.position.y - radius), horizontal_line);
		Entity line4 = createLine(vec2(motion_i.position.x, motion_i.position.y + radius), horizontal_line);

		if (registry.meshPtrs.has(entity_i)) {
			vec2 point = { 5.f, 5.f };
			for (uint j = 0; j < chicken_mesh->vertices.size(); j++) {
				vec3 chicken_mesh_position = transform.mat *
					vec3(chicken_mesh->vertices[j].position.x, chicken_mesh->vertices[j].position.y, 1.f);
				createLine(vec2(chicken_mesh_position), point);
			}
		}

	}
}
void PhysicsSystem::step(float elapsed_ms)
{
	// Move bug based on how much time has passed, this is to (partially) avoid
	// having entities move at different speed based on the machine.
	auto& motion_registry = registry.motions;
	auto& physics_registry = registry.physics;
	for (uint i = 0; i < motion_registry.size(); i++)
	{
		Motion& motion = motion_registry.components[i];
		Entity entity = motion_registry.entities[i];
		float step_seconds = elapsed_ms / 1000.f;
		motion.position += step_seconds * motion.velocity;

		// A2: handle collisions with left/right walls, objective: to bounce off walls
		if (registry.eatables.has(entity) || registry.deadlys.has(entity)) {
			if (!registry.players.has(entity)) {
				vec2 bb = get_bounding_box(motion);
				if (motion.position.x + bb.x / 2.f >= window_width_px) {
					motion.position.x = window_width_px - bb.x / 2.f;
					motion.velocity.x = -motion.velocity.x;
				}

				if (motion.position.x - bb.x / 2.f <= 0.f) {
					motion.position.x = bb.x / 2.f;
					motion.velocity.x = -motion.velocity.x;
				}
			}
		}
		if (registry.deadlys.has(entity)) {
			vec2 bb = get_bounding_box(motion);
			if (motion.position.y + bb.y / 2.f >= window_height_px) {
				motion.position.y = window_height_px - bb.y / 2.f;
				motion.velocity.y = -motion.velocity.y;
			}

			if (motion.position.y - bb.y / 2.f <= 0.f) {
				motion.position.y = bb.y / 2.f;
				motion.velocity.y = -motion.velocity.y;
			}
		}
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A3: HANDLE EGG UPDATES HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 3
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// Check for collisions between all moving entities
	ComponentContainer<Motion>& motion_container = registry.motions;
	for (uint i = 0; i < motion_container.components.size(); i++)
	{
		Motion& motion_i = motion_container.components[i];
		Entity entity_i = motion_container.entities[i];

		// note starting j at i+1 to compare all (i,j) pairs only once (and to not compare with itself)
		for (uint j = i + 1; j < motion_container.components.size(); j++)
		{
			Motion& motion_j = motion_container.components[j];
			if (collides(motion_i, motion_j))
			{
				
				Entity entity_j = motion_container.entities[j];
				// Create a collisions event
				// We are abusing the ECS system a bit in that we potentially insert muliple collisions for the same entity
				registry.collisions.emplace_with_duplicates(entity_i, entity_j);
				registry.collisions.emplace_with_duplicates(entity_j, entity_i);
			}
		}
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: HANDLE CHICKEN - WALL collisions HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// you may need the following quantities to compute wall positions
	(float)window_width_px; (float)window_height_px;

	Entity chicken = registry.players.entities[0]; // assumed there's only 1 chicken, if multiple, needs an alternative
	Motion& chicken_motion = registry.motions.get(chicken);
	vec2 chicken_position = chicken_motion.position;
	vec2 chicken_bb = get_bounding_box(chicken_motion); // bounding box of chicken
	Mesh* chicken_mesh = registry.meshPtrs.get(chicken);

	Transform transform;
	transform.translate(chicken_motion.position);
	transform.rotate(-chicken_motion.angle);
	transform.scale(chicken_motion.scale);

	// for efficiency, check if bounding box touches the walls first. If yes check vertices of the chicken
	if (chicken_position.x - chicken_bb.x / 2.f <= 0.f ||
		chicken_position.x + chicken_bb.x / 2.f >= window_width_px || 
		chicken_position.y - chicken_bb.y / 2.f <= 0.f ||
		chicken_position.y + chicken_bb.y / 2.f >= window_height_px) {
		// used for proper offseting when chicken mesh collides with walls
		// not setting the chicken position during looping to avoid glitchy (repeatedly) assignment of position
		// only keep the max and min values to avoid reposition the chicken by the wrong offset ( too small or too large) 
		float min_mesh_position_x = std::numeric_limits<float>::infinity();
		float max_mesh_position_x = -std::numeric_limits<float>::infinity();
		float min_mesh_position_y = std::numeric_limits<float>::infinity();
		float max_mesh_position_y = -std::numeric_limits<float>::infinity();
		// keep track of which walls does the chicken mesh collide with, could have more than 1 wall at a time
		bool left = false; bool right = false; bool top = false; bool bot = false;

		for (uint i = 0; i < chicken_mesh->vertices.size(); i++) {
			// local frame of chicken mesh -> world frame
			vec3 chicken_mesh_position = transform.mat * vec3(chicken_mesh->vertices[i].position.x, 
				chicken_mesh->vertices[i].position.y, 1.f);
			// comparison between mesh position and walls
			if (chicken_mesh_position.x <= 0.f) {
				left = true;
				if (chicken_mesh_position.x < min_mesh_position_x)
					min_mesh_position_x = chicken_mesh_position.x;
			}
			else if (chicken_mesh_position.x >= window_width_px) { 
				right = true;
				if (chicken_mesh_position.x > max_mesh_position_x) 
					max_mesh_position_x = chicken_mesh_position.x;
			}
			if (chicken_mesh_position.y <= 0.f) { 
				top = true;
				if (chicken_mesh_position.y < min_mesh_position_y) 
					min_mesh_position_y = chicken_mesh_position.y;
			}	
			else if (chicken_mesh_position.y >= window_height_px) {
				bot = true;
				if (chicken_mesh_position.y > max_mesh_position_y)
					max_mesh_position_y = chicken_mesh_position.y;
			}
		}
		// determine which walls the chicken mesh is colliding with, for efficiency, do not consider the opposte walls case
		if (left)
			chicken_motion.position.x = chicken_position.x + abs(min_mesh_position_x);
		else if (right)
			chicken_motion.position.x = chicken_position.x - abs(window_width_px - max_mesh_position_x);
		if (top)
			chicken_motion.position.y = chicken_position.y + abs(min_mesh_position_y);
		else if (bot)
			chicken_motion.position.y = chicken_position.y - abs(window_height_px - max_mesh_position_y);
	}

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A2: DRAW DEBUG INFO HERE on Chicken mesh collision
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 2
	// You will want to use the createLine from world_init.hpp
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

	// debugging of bounding boxes
	if (debugging.in_debug_mode)
		debug();

	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// TODO A3: HANDLE EGG collisions HERE
	// DON'T WORRY ABOUT THIS UNTIL ASSIGNMENT 3
	// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	for (uint i = 0; i < physics_registry.size(); i++)
	{
		float h = elapsed_ms / 1000.f;
		Entity entity = physics_registry.entities[i];
		if (!registry.physics_laws.has(entity) || !registry.physics_laws.get(entity).obey_gravity) continue;
		Physics& physics = physics_registry.components[i];
		Motion& motion = motion_registry.get(entity);
		physics.force = physics.mass * physics.gravity;
		physics.acceleration = physics.force / physics.mass;
		physics.mid_position = motion.position + 0.5f * h * motion.velocity;
		physics.mid_velocity = motion.velocity + 0.5f * h * physics.acceleration;	
		physics.mid_force = physics.mass * (physics.mid_velocity - motion.velocity) / h;
		physics.mid_acceleration = physics.mid_force / physics.mass;
		motion.position += h * physics.mid_velocity;
		motion.velocity += h * physics.mid_acceleration;
	}
}