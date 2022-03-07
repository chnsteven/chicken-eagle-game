
#define GL3W_IMPLEMENTATION
#include <gl3w.h>

// stlib
#include <chrono>

// internal
#include "ai_system.hpp"
#include "physics_system.hpp"
#include "render_system.hpp"
#include "world_system.hpp"

using Clock = std::chrono::high_resolution_clock;
struct Mode
{
	bool basic = true;
	bool advance = false;
};

float x = 0.f;


// Entry point
int main()
{
	// Global systems
	WorldSystem world;
	RenderSystem renderer;
	PhysicsSystem physics;
	AISystem ai;

	// Initializing window
	GLFWwindow* window = world.create_window();
	if (!window) {
		// Time to read the error message
		printf("Press any key to exit");
		getchar();
		return EXIT_FAILURE;
	}

	// initialize the main systems
	renderer.init(window);
	world.init(&renderer);

	// variable timestep loop
	auto t = Clock::now();
	while (!world.is_over()) {
		// Processes system messages, if this wasn't present the window would become unresponsive
		glfwPollEvents();

		// Calculating elapsed times in milliseconds from the previous iteration
		auto now = Clock::now();
		float elapsed_ms =
			(float)(std::chrono::duration_cast<std::chrono::microseconds>(now - t)).count() / 1000;
		t = now;

		// TODO A2: you can implement the debug freeze here but other places are possible too.
		// freeze for 0.5 second
		if (debugging.in_freeze_mode && x <= 500.f) {
			ai.debug();
			physics.debug();
			renderer.draw();

			x += elapsed_ms;
		}	
		else {
			world.step(elapsed_ms);
			ai.step(elapsed_ms);
			physics.step(elapsed_ms);
			world.handle_collisions();
			renderer.draw();
			x = 0.f;
			debugging.in_freeze_mode = false;
		}
	}

	return EXIT_SUCCESS;
}
