#pragma once

#include "common/common.h"
#include "engine_graphic/engine_graphic.h"

#include "scene_container.h"
#include "shaders/hlsl_shared_struct.h"
#include "world/component/camera.h"

class Mesh_renderer;
class Mesh_novb_renderer;
class Raytrace_renderer;
class Post_renderer;

class Render_pass_main;
class Render_pass_shadow_map;
class Render_pass_raytrace_main;

class Frame_pipeline : public Engine_client {
  public:
	Frame_pipeline(Engine& engine);

	virtual void load_resource() override;
	virtual void update() override;
	virtual void draw() override;

	void register_mesh_data(const string& name, const Mesh_data& mesh_data)
	{
		auto&& pair = std::make_pair(name, std::make_shared<Mesh_data>(mesh_data));
		m_mesh_data_pool.insert(pair);
	}

	std::weak_ptr<Mesh_data> get_mesh_data(const string& name)
	{
		auto&& found = m_mesh_data_pool.find(name);
		if (found != std::end(m_mesh_data_pool)) {
			return found->second;
		}

		return std::weak_ptr<Mesh_data>();
	}

	const auto& get_mesh_data_pool() const
	{
		return m_mesh_data_pool;
	}

	unique_ptr<Mesh_renderer>	   m_mesh_renderer;
	unique_ptr<Mesh_novb_renderer> m_mesh_novb_renderer;
	unique_ptr<Raytrace_renderer>  m_raytrace_renderer;
	unique_ptr<Post_renderer>	   m_post_renderer;

	unique_ptr<Render_pass_main>		  m_render_pass_main;
	unique_ptr<Render_pass_shadow_map>	  m_render_pass_shadow_map;
	unique_ptr<Render_pass_raytrace_main> m_render_pass_raytrace_main;

	Camera			m_camera;
	Scene_container m_scene_container;

  private:
	Engine& m_engine;

	std::unordered_map<string, std::shared_ptr<Mesh_data>> m_mesh_data_pool;
};
