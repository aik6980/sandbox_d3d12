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
            view.get<Transform>(e).m_pos.y += 1.0f * App::get_duration_frame();
        }

        if (App::input().is_keydown(OIS::KC_S)) {
            view.get<Transform>(e).m_pos.y -= 1.0f * App::get_duration_frame();
        }
    }
}

void Player_render(entt::registry& reg)
{
    auto view = reg.view<Player, Transform>();
    for (const entt::entity e : view) {
    }
}

void Camera_update(entt::registry& reg)
{
    auto view = reg.view<Arcball>();
    for (const entt::entity e : view) {
        auto&& arcball = view.get<Arcball>(e);
        arcball.update();

        App::m_renderer->m_camera.m_position = arcball.pos();
        App::m_renderer->m_camera.m_view     = arcball.view();

        auto&& world = Matrix::CreateFromQuaternion(arcball.m_orient);
        // world.Translation(arcball.m_pos * 0.5f);
        App::m_renderer->m_test_world = world;
    }
}
