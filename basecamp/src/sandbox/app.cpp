#include "app.h"

#include <atomic>
#include <thread>

#include "common/common_cpp.h"
#include "engine_graphic/engine_graphic_cpp.h"
#include "input/input_manager.h"
#include "world/component/components.h"
#include "world/entity/entity.h"
#include "world/system/graphic/frame_pipeline.h"
#include "world/system/graphic/mesh_renderer.h"
#include "world/system/graphic/raytrace_renderer.h"
#include "world/system/graphic/render_pass/render_pass_main.h"
#include "world/system/systems.h"

#include <future>

std::unique_ptr<Engine>		   App::m_engine;
std::unique_ptr<Input_manager> App::m_input_manager;
entt::registry				   App::m_reg;

Frame_pipeline* App::m_renderer = nullptr;

std::chrono::time_point<std::chrono::steady_clock> App::m_time_begin_app;
std::chrono::time_point<std::chrono::steady_clock> App::m_time_begin_frame;
std::chrono::microseconds						   App::m_duration_frame;

std::unique_ptr<std::thread> render_thread;
std::atomic<bool>			 game_running = true;

std::future<int> fut_val;

void App::on_init(HINSTANCE hInstance, HWND hWnd)
{
	m_hInstance = hInstance;
	m_hWnd		= hWnd;

	m_input_manager = make_unique<Input_manager>();
	m_input_manager->init(m_hWnd);

	m_engine = make_unique<Engine>();
	Engine::InitData init_data;
	init_data.HWnd = hWnd;
	m_engine->init(init_data);

	unique_ptr<Frame_pipeline> renderer(new Frame_pipeline(*m_engine));
	m_renderer		 = renderer.get();
	renderer->m_name = "renderer";
	m_engine->register_client(std::move(renderer));

	load_world_assets();

	m_engine->load_resource();

	// time
	m_time_begin_frame = m_time_begin_app = std::chrono::high_resolution_clock::now();

	// world
	create_world();

	// thread
	// m_engine->update();
	// render_thread.reset(new std::thread(
	//	[&]() { while (game_running) { m_engine->draw(); }
	//}));

	// fut_val = prom_val.get_future();
}

void App::on_update()
{
	// frame time
	m_duration_frame   = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - m_time_begin_frame);
	m_time_begin_frame = std::chrono::steady_clock::now();

	// input
	m_input_manager->update();

	// system
	Player_input(m_reg);
	Camera_update(m_reg);

	Animation_update(m_reg);

	if (App::input().is_keydown(OIS::KC_SPACE)) {
		std::promise<int> prom_val;
		prom_val.set_value(10);
		fut_val = prom_val.get_future();
	}

	// if (fut_val.valid()) {
	//	DBG::OutputString("promised fulfill %d", fut_val.get());
	// }

	// pre render
	m_renderer->m_scene_container.clear();
	Object_render(m_reg);

	m_engine->update();
	m_engine->draw();
}

void App::on_destroy()
{
	if (fut_val.valid()) {
		DBG::OutputString("promised fulfill %d", fut_val.get());
	}

	game_running = false;
	// render_thread->join();

	m_engine->destroy();
	m_input_manager->destroy();
}

void App::load_world_assets()
{
	{
		auto&& mesh_data = MeshDataGenerator::create_grid(25.0, 25.0, 10, 10);
		m_renderer->register_mesh_data(World_assets::m_grid.data(), mesh_data);
	}

	{
		auto&& mesh_data = MeshDataGenerator::create_unit_cube();
		m_renderer->register_mesh_data(World_assets::m_cube.data(), mesh_data);
	}

	{
		Model_loader model_loader;
		model_loader.load(World_assets::m_duck.data());
		auto&& mesh_data = model_loader.meshes();
		m_renderer->register_mesh_data(World_assets::m_duck.data(), mesh_data);
	}
}

void App::create_world()
{
	Make_player(m_reg);
	Make_camera(m_reg);

	Make_object(m_reg, Vector3(0.0f, 0.0f, 0.0f), World_assets::m_grid.data());

	auto&& e		 = Make_object(m_reg, Vector3(0.0f, 0.1f, 0.0f), World_assets::m_duck.data());
	auto&& transform = m_reg.get<Transform>(e);
	transform.scale	 = Vector3::One * 0.1f;

	for (int i = 0; i < 5; ++i) {
		auto&& e = Make_object(m_reg, Vector3(0.0f, 0.0f, 0.0f), World_assets::m_cube.data());
		m_reg.emplace<Animation_comp>(e);
	}
}
