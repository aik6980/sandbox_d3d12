#include "technique.h"

#include "device.h"
#include "shader.h"
#include "shader_manager.h"

namespace VKN {

    void Technique::destroy()
    {
        auto&& device = m_gfx_device.m_device;

        device.destroyPipeline(m_pipeline);
        device.destroyPipelineLayout(m_pipeline_layout);

        for (auto&& d : m_descriptorset_layouts) {
            device.destroyDescriptorSetLayout(d);
        }
        m_descriptorset_layouts.clear();
    }

    void Technique::create_pipeline()
    {
        auto&& device = m_gfx_device.m_device;

        // Render pass
        auto&& render_pass = m_gfx_device.m_render_pass;

        auto&& vs = mh_vs.lock();
        auto&& ps = mh_ps.lock();

        // Programable state -----------
        std::array<vk::PipelineShaderStageCreateInfo, 2> pipeline_shader_stage_createinfo = {
            vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eVertex, vs->m_shader_module, "main"),
            vk::PipelineShaderStageCreateInfo(vk::PipelineShaderStageCreateFlags(), vk::ShaderStageFlagBits::eFragment, ps->m_shader_module, "main")};

        vk::PipelineVertexInputStateCreateInfo pipeline_vertex_input_state_createinfo;
        if (vs->m_vertex_input_attribute_descriptions.size() > 0) {
            pipeline_vertex_input_state_createinfo = {{}, vs->m_vertex_input_binding_description, vs->m_vertex_input_attribute_descriptions};
        }
        else {
            pipeline_vertex_input_state_createinfo = {{}, 0, nullptr, 0, nullptr};
        }

        // Create a pipeline layout from Shader stages
        auto&& num_descriptorset = vs->m_descriptorset_layoutdata.size() + ps->m_descriptorset_layoutdata.size();
        if (num_descriptorset > 0) {
            m_descriptorset_layouts.reserve(num_descriptorset);
            m_descriptorset_infos.reserve(num_descriptorset);

            {
                auto&& shader_stage = vs;
                for (auto&& layout_data : shader_stage->m_descriptorset_layoutdata) {
                    auto&& layout = device.createDescriptorSetLayout(layout_data.create_info);
                    m_descriptorset_layouts.emplace_back(layout);
                    m_descriptorset_infos.emplace_back(&layout_data);
                }
            }

            {
                auto&& shader_stage = ps;
                for (auto&& layout_data : shader_stage->m_descriptorset_layoutdata) {
                    auto&& layout = device.createDescriptorSetLayout(layout_data.create_info);
                    m_descriptorset_layouts.emplace_back(layout);
                    m_descriptorset_infos.emplace_back(&layout_data);
                }
            }

            m_pipeline_layout = device.createPipelineLayout(vk::PipelineLayoutCreateInfo({}, m_descriptorset_layouts));
        }
        else {
            m_pipeline_layout = device.createPipelineLayout(vk::PipelineLayoutCreateInfo(vk::PipelineLayoutCreateFlags()));
        }

        // ----------

        // Fixed pipeline state -----------
        vk::PipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_createinfo(
            vk::PipelineInputAssemblyStateCreateFlags(), vk::PrimitiveTopology::eTriangleList);

        vk::PipelineViewportStateCreateInfo pipeline_viewport_state_createinfo(vk::PipelineViewportStateCreateFlags(), 1, nullptr, 1, nullptr);

        vk::PipelineRasterizationStateCreateInfo pipeline_rasterization_state_createinfo(vk::PipelineRasterizationStateCreateFlags(), // flags
            false,                                                                                                                    // depthClampEnable
            false,                                                                                                                    // rasterizerDiscardEnable
            vk::PolygonMode::eFill,                                                                                                   // polygonMode
            vk::CullModeFlagBits::eBack,                                                                                              // cullMode
            vk::FrontFace::eClockwise,                                                                                                // frontFace
            false,                                                                                                                    // depthBiasEnable
            0.0f,                                                                                                                     // depthBiasConstantFactor
            0.0f,                                                                                                                     // depthBiasClamp
            0.0f,                                                                                                                     // depthBiasSlopeFactor
            1.0f                                                                                                                      // lineWidth
        );

        vk::PipelineMultisampleStateCreateInfo pipeline_multisample_state_createinfo(vk::PipelineMultisampleStateCreateFlags(), // flags
            vk::SampleCountFlagBits::e1                                                                                         // rasterizationSamples
                                                                                                                                // other values can be default
        );

        vk::StencilOpState                      stencil_op_state(vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::CompareOp::eAlways);
        vk::PipelineDepthStencilStateCreateInfo pipeline_depth_stencil_state_createinfo(vk::PipelineDepthStencilStateCreateFlags(), // flags
            true,                                                                                                                   // depthTestEnable
            true,                                                                                                                   // depthWriteEnable
            vk::CompareOp::eLessOrEqual,                                                                                            // depthCompareOp
            false,                                                                                                                  // depthBoundTestEnable
            false,                                                                                                                  // stencilTestEnable
            stencil_op_state,                                                                                                       // front
            stencil_op_state                                                                                                        // back
        );

        vk::ColorComponentFlags color_component_flags(
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
        vk::PipelineColorBlendAttachmentState pipeline_color_blend_attachment_state(false, // blendEnable
            vk::BlendFactor::eZero,                                                        // srcColorBlendFactor
            vk::BlendFactor::eZero,                                                        // dstColorBlendFactor
            vk::BlendOp::eAdd,                                                             // colorBlendOp
            vk::BlendFactor::eZero,                                                        // srcAlphaBlendFactor
            vk::BlendFactor::eZero,                                                        // dstAlphaBlendFactor
            vk::BlendOp::eAdd,                                                             // alphaBlendOp
            color_component_flags                                                          // colorWriteMask
        );

        vk::PipelineColorBlendStateCreateInfo pipeline_color_blend_state_createinfo(vk::PipelineColorBlendStateCreateFlags(), // flags
            false,                                                                                                            // logicOpEnable
            vk::LogicOp::eNoOp,                                                                                               // logicOp
            pipeline_color_blend_attachment_state,                                                                            // attachments
            {{1.0f, 1.0f, 1.0f, 1.0f}}                                                                                        // blendConstants
        );

        std::array<vk::DynamicState, 2>    dynamic_states = {vk::DynamicState::eViewport, vk::DynamicState::eScissor};
        vk::PipelineDynamicStateCreateInfo pipeline_dynamic_state_createinfo(vk::PipelineDynamicStateCreateFlags(), dynamic_states);
        // -----------

        vk::GraphicsPipelineCreateInfo graphics_pipeline_createinfo(vk::PipelineCreateFlags(), // flags
            pipeline_shader_stage_createinfo,                                                  // stages
            &pipeline_vertex_input_state_createinfo,                                           // pVertexInputState
            &pipeline_input_assembly_state_createinfo,                                         // pInputAssemblyState
            nullptr,                                                                           // pTessellationState
            &pipeline_viewport_state_createinfo,                                               // pViewportState
            &pipeline_rasterization_state_createinfo,                                          // pRasterizationState
            &pipeline_multisample_state_createinfo,                                            // pMultisampleState
            &pipeline_depth_stencil_state_createinfo,                                          // pDepthStencilState
            &pipeline_color_blend_state_createinfo,                                            // pColorBlendState
            &pipeline_dynamic_state_createinfo,                                                // pDynamicState
            m_pipeline_layout,                                                                 // layout
            render_pass                                                                        // renderPass
        );

        vk::Result   result;
        vk::Pipeline pipeline;
        std::tie(result, pipeline) = device.createGraphicsPipeline(nullptr, graphics_pipeline_createinfo);
        switch (result) {
        case vk::Result::eSuccess:
            break;
        case vk::Result::ePipelineCompileRequiredEXT:
            // something meaningfull here
            break;
        default:
            assert(false); // should never happen
        }

        m_pipeline = pipeline;
    }

} // namespace VKN
