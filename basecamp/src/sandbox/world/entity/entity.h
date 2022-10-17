#pragma once

struct World_assets {
    constexpr static std::string_view m_grid = "grid_mesh";
    constexpr static std::string_view m_cube = "cube_mesh";
    constexpr static std::string_view m_duck = "DuckWhite.fbx";
};

entt::entity Make_player(entt::registry& reg);
entt::entity Make_camera(entt::registry& reg);

entt::entity Make_object(entt::registry& reg, const Vector3& pos, const std::string& mesh);
