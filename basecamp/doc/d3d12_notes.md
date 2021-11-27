## Resource state common

Hi, 

I have a question from my Shadow mapping test 
Here I have 
1 Create depth_buffer
2 State change render_device.buffer_state_transition(depth_buffer.lock(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
3 Draw Object into Shadow map
4 render_device.buffer_state_transition(depth_buffer.lock(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_COMMON);
5 Draw Object using Shadow map as SRV 

I expected that I need to  do this
render_device.buffer_state_transition(*depth_buffer.lock(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_GENERIC_READ); 
before 5. 
because I didn't, I thought I will encounter D3DError saying my ShadowMap srv is in the incorrect state, however, my shadowmap SRV seem to work without it, and there is no D3D Error. 

Any idea why?

A:
Depth-stencil resources must be non-simultaneous-access textures and thus can never be implicitly promoted, You're transitioning your depth buffer into a COMMON state, the debug layer won't complain because is in the COMMON state, Any resource in the COMMON state can be accessed as through it were in a single state with 1 WRITE flag, or 1 or more READ flags. For best performance you should avoid transition the resource to COMMON, I would transition your depth buffer to D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE for SRVs 

Is there a D3D Warning? 

You won't, because of you transition your resource to COMMON state, Any resource in the COMMON state can be accessed as through it were in a single state with 1 WRITE flag, or 1 or more READ flags you should avoid transitioning to the COMMON state because is bad practice, you can find more info about this in this video: https://youtu.be/Db2TaG49SRg 
The best suggestion here I would recommend is that, when you are creating buffers, always create them in the COMMON state, and utilize implicit promotion/decay upon first use on the command list, for textures, unless they are created with D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS, you should always create them in the optimal state, for example, if you are creating a render target, create the resource in D3D12_RESOURCE_STATE_RENDER_TARGET, then explicitly transition them, we need to keep track of textures that don't have D3D12_RESOURCE_FLAG_ALLOW_SIMULTANEOUS_ACCESS because they can't be promoted nor decayed.


## What is resource state promotion
https://devblogs.microsoft.com/directx/a-look-inside-d3d12-resource-state-barriers/#resource-state-promotion-and-decay

