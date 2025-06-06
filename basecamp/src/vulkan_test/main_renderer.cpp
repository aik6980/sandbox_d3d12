#include "main_renderer.h"

// systems
#include "gfx_device/technique_instance.h"

// app
#include "gfx_device/gfx_main.h"

void Main_renderer::draw()
{
    auto&& gfx_device     = Gfx_main::gfx_device();
    auto&& shader_manager = Gfx_main::shader_manager();

    auto&& command_buffer = gfx_device.curr_command_buffer();
    if (!command_buffer) {
        return;
    }

    // setup render pass
    auto&& render_target_image = gfx_device.backbuffer_colour_image();
    auto&& depth_target_image  = gfx_device.backbuffer_depth_image();

    gfx_device.transition_image_layout(render_target_image,
        VKN::Device::Transition_image_layout_info{
            .dst_layout       = vk::ImageLayout::eColorAttachmentOptimal,
            .src_layout       = vk::ImageLayout::eUndefined,
            .dst_access_flags = vk::AccessFlagBits2::eColorAttachmentWrite,
            .src_access_flags = vk::AccessFlagBits2::eNone,
            .dst_stage_flags  = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
            .src_stage_flags  = vk::PipelineStageFlagBits2::eTopOfPipe,
        });

    vk::ClearColorValue clear_colour{{{0.2f, 0.2f, 0.2f, 0.2f}}};
    vk::ClearDepthStencilValue clear_depth = {
        .depth   = 1.0f,
        .stencil = 0u,
    };

    vk::RenderingAttachmentInfo colour_attachment{
        .imageView   = gfx_device.backbuffer_colour_image_view(),
        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .loadOp      = vk::AttachmentLoadOp::eClear,
        .storeOp     = vk::AttachmentStoreOp::eStore,
        .clearValue  = clear_colour,
    };

    vk::RenderingInfo rendering_info{
        .renderArea =
            {
                .offset = {0, 0},
                .extent = gfx_device.backbuffer_colour_size(),
            },
        .layerCount           = 1,
        .colorAttachmentCount = 1,
        .pColorAttachments    = &colour_attachment,

    };

    command_buffer->beginRendering(&rendering_info);

    command_buffer->setViewport(0,
        vk::Viewport(0.0f,
            0.0f,
            static_cast<float>(rendering_info.renderArea.extent.width),
            static_cast<float>(rendering_info.renderArea.extent.height),
            0.0f,
            1.0f));
    command_buffer->setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), rendering_info.renderArea.extent));

    auto&& technique = shader_manager.get_technique("test/single_triangle").lock();
    command_buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, technique->m_pipeline);
    // m_command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0,
    // descriptorSet, nullptr);

    // command_buffer.bindVertexBuffers(0, 0, nullptr, nullptr);

    // m_command_buffer.draw(12 * 3, 1, 0, 0);
    command_buffer->draw(3, 1, 0, 0);

    // 2nd draw
    // auto&& technique          = shader_manager.get_technique("t1").lock();
    // auto&& technique_instance = VKN::Technique_instance(*technique);
    // float data[]              = {4.0f, 1.0f};
    // technique_instance.set_constant("Data_cbv", data, sizeof(data));

    // command_buffer->bindPipeline(vk::PipelineBindPoint::eGraphics, technique->m_pipeline);
    // technique_instance.set_descriptor_set_parameters();

    // command_buffer->bindVertexBuffers(0, resource_manager->m_vertex_buffer.m_buffer, {0});
    // command_buffer->bindIndexBuffer(m_resource_manager->m_index_buffer.m_buffer, 0, vk::IndexType::eUint32);

    // command_buffer->drawIndexed(36, 1, 0, 0, 0);

    command_buffer->endRendering();

    gfx_device.transition_image_layout(render_target_image,
        VKN::Device::Transition_image_layout_info{
            .dst_layout       = vk::ImageLayout::ePresentSrcKHR,
            .src_layout       = vk::ImageLayout::eColorAttachmentOptimal,
            .dst_access_flags = vk::AccessFlagBits2::eNone,
            .src_access_flags = vk::AccessFlagBits2::eColorAttachmentWrite,
            .dst_stage_flags  = vk::PipelineStageFlagBits2::eBottomOfPipe,
            .src_stage_flags  = vk::PipelineStageFlagBits2::eColorAttachmentOutput,
        });
}
