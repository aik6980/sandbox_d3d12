#include "entity.h"
#include "world/component/components.h"

entt::entity Make_player(entt::registry& reg)
{
    const entt::entity e = reg.create();
    reg.emplace<Player>(e);
    reg.emplace<Transform>(e);

    return e;
}
