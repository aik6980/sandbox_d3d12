#pragma once

#include "common/common.h"

struct Transform {
	Vector3	   pos	  = Vec3::Origin;
	Quaternion orient = Quat::Identity;
	Vector3	   scale  = Vec3::One;
};

struct Player {};

struct Light {};

struct Animation_comp {};

struct Object_renderer_comp {
	std::string name;
	bool		cast_shadow	   = false;
	bool		receive_shadow = false;
};
