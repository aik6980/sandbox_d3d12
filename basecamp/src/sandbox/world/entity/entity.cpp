#include "entity.h"
#include "world/component/arcball.h"
#include "world/component/components.h"

entt::entity Make_player(entt::registry& reg)
{
    const entt::entity e = reg.create();
    reg.emplace<Player>(e);
    reg.emplace<Transform>(e);

    return e;
}

entt::entity Make_camera(entt::registry& reg)
{
    const entt::entity e = reg.create();

    auto&&  obj = reg.emplace<Arcball>(e);
    Vector3 cam_pos(0.0f, 8.0f, 12.0f);
    obj.init(cam_pos, Vector3::Zero);

    return e;
}
