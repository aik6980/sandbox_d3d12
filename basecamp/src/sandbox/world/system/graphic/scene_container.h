#pragma once

#include "common/common.h"

struct Scene_object_data {
	Matrix transform;
	bool   cast_shadow	  = false;
	bool   receive_shadow = false;
};

class Scene_container {
  public:
	std::unordered_multimap<string, Scene_object_data> m_scene_instances;

	void add_instance(const string& id, const Scene_object_data& object_data)
	{
		m_scene_instances.insert({id, object_data});
	}

	void clear()
	{
		m_scene_instances.clear();
	}
};
