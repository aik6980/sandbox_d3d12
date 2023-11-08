#pragma once

#include "engine_graphic/engine_graphic.h"

#include "render_pass/render_pass_main.h"
#include "world/component/camera.h"

class Frame_pipeline;

class Mesh_novb_renderer {
  public:
	Mesh_novb_renderer(Engine& engine, Frame_pipeline& frame_pipeline)
		: m_engine(engine)
		, m_frame_pipeline(frame_pipeline)
	{
	}

	void load_resource();
	void update(){};
	void draw();

	struct Instance_data {
		XMFLOAT4 pos_world;
		XMFLOAT4 orient_world;
	};
	int					  m_num_instances = 5;
	vector<Instance_data> m_instance_data;

	void draw_meshes_shadow_map();
	void draw_meshes();

	void build_texture();

	void update_camera();

	Engine&			m_engine;
	Frame_pipeline& m_frame_pipeline;

	// string m_duck_white_mesh;
	// string m_unit_quad_name;
	// string m_unit_cube_name;
	// string m_grid_mesh;

	string m_texture_name			   = "checkerboard_texture";
	string m_point_sampler			   = "point_sampler";
	string m_instance_data_buffer_name = "instance_data_buffer";

	Camera m_light;

	string m_mesh_novb_technique_handle;
};
