#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include "render_system.hpp"
#include <random>

// These are hard coded to the dimensions of the entity texture
const float BUG_BB_WIDTH = 0.6f * 165.f;
const float BUG_BB_HEIGHT = 0.6f * 165.f;
const float EAGLE_BB_WIDTH = 0.6f * 300.f;
const float EAGLE_BB_HEIGHT = 0.6f * 202.f;
const float VORTEX_BB_WIDTH = 0.6f * 300.f;
const float VORTEX_BB_HEIGHT = 0.6f * 300.f;
const float STONE_BB_WIDTH = 0.6f * 100.f;
const float STONE_BB_HEIGHT = 0.6f * 100.f;

// the player
Entity createChicken(RenderSystem* renderer, vec2 pos);
// the prey
Entity createBug(RenderSystem* renderer, vec2 position);
// the enemy
Entity createEagle(RenderSystem* renderer, vec2 position);
// a red line for debugging purposes
Entity createLine(vec2 position, vec2 size);
// a egg
Entity createEgg(vec2 pos, vec2 size);
// a vortex
Entity createVortex(RenderSystem* renderer, vec2 pos);
// a stone
Entity createStone(RenderSystem* renderer, vec2 pos, float rand);


