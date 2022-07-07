#include "descriptor_pool.h"

#include "device.h"

namespace VKN {

    void Descriptor_pool::reset()
    {
        m_gfx_device.m_device.resetDescriptorPool(m_descriptor_pool);
        m_descriptor_sets.clear();
    }
} // namespace VKN
