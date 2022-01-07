#pragma once

struct Resource_state_tracker {
    D3D12_RESOURCE_STATES init_state;
    D3D12_RESOURCE_STATES curr_state;
};
