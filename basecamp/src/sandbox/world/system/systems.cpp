#include "systems.h"

#include "app.h"
#include "input/input_manager.h"
#include "world/component/arcball.h"
#include "world/component/components.h"
#include "world/system/graphic/frame_pipeline.h"

void Player_input(entt::registry& reg)
{
	auto view = reg.view<Player, Transform>();
	for (const entt::entity e : view) {
		if (App::input().is_keydown(OIS::KC_W)) {
			// DBG::OutputString(L"Time, %f %f \n", App::get_duration_app(), App::get_duration_frame());
			view.get<Transform>(e).pos.y += 1.0f * App::get_duration_frame();
		}

		if (App::input().is_keydown(OIS::KC_S)) {
			view.get<Transform>(e).pos.y -= 1.0f * App::get_duration_frame();
		}
	}
}

void Camera_update(entt::registry& reg)
{
	auto view = reg.view<Arcball>();
	for (const entt::entity e : view) {
		auto&& arcball = view.get<Arcball>(e);
		arcball.update();

		App::m_renderer->m_camera.m_position = arcball.pos();
		App::m_renderer->m_camera.m_view	 = arcball.view();
	}
}

void Animation_update(entt::registry& reg)
{
	auto view = reg.view<Animation_comp, Transform>();

	auto&& num_ent		 = view.size_hint();
	auto&& t			 = App::get_duration_app();
	int	   num_instances = 5;
	int	   counter		 = 0;
	for (const entt::entity e : view) {
		float	 phase = counter * XM_2PI / num_instances;
		XMVECTOR pos   = XMVectorSet(4.0f * sin(phase), 4.0f + sin(phase + t * 1.5f), 4.0f * cos(phase), 0.0f);

		auto&& transform = view.get<Transform>(e);
		transform.pos	 = pos;

		counter++;
	}
}

void Object_render(entt::registry& reg)
{
	auto view = reg.view<Object_renderer_comp, Transform>();
	for (const entt::entity e : view) {
		auto&& transform = view.get<Transform>(e);
		auto&& render	 = view.get<Object_renderer_comp>(e);

		auto&& scene_container = App::m_renderer->m_scene_container;

		auto&& world_mat = Matrix::CreateScale(transform.scale) * Matrix::CreateFromQuaternion(transform.orient) * Matrix::CreateTranslation(transform.pos);
		scene_container.add_instance(render.name, {.transform = world_mat});
	}
}
