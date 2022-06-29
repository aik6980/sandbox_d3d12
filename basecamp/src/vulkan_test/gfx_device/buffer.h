#pragma once

#include "vma/vma.h"

namespace VKN {

class Buffer {
  public:
    vk::Buffer      m_buffer;
    vma::Allocation m_allocation;
};

} // namespace VKN
