#include "systems.h"

#include "app.h"
#include "input/input_manager.h"
#include "world/component/components.h"

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
