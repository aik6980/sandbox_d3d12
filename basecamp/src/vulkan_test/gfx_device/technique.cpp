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

    void Technique::create_pipeline(vk::Format color_format, vk::Format depth_format)
    {
        auto&& device = m_gfx_device.m_device;

        auto&& vs = mh_vs.lock();
        auto&& ps = mh_ps.lock();

        // Programable state -----------
        std::array<vk::PipelineShaderStageCreateInfo, 2> pipeline_shader_stage_createinfo = {
            vk::PipelineShaderStageCreateInfo{
                .stage = vk::ShaderStageFlagBits::eVertex, .module = vs->m_shader_module, .pName = "main"},
            vk::PipelineShaderStageCreateInfo{
                .stage = vk::ShaderStageFlagBits::eFragment, .module = ps->m_shader_module, .pName = "main"}};

        vk::PipelineVertexInputStateCreateInfo pipeline_vertex_input_state_createinfo;
        if (vs->m_vertex_input_attribute_descriptions.size() > 0) {
            pipeline_vertex_input_state_createinfo = {
                .vertexBindingDescriptionCount   = 1,
                .pVertexBindingDescriptions      = &vs->m_vertex_input_binding_description,
                .vertexAttributeDescriptionCount = (uint32_t)vs->m_vertex_input_attribute_descriptions.size(),
                .pVertexAttributeDescriptions    = vs->m_vertex_input_attribute_descriptions.data(),
            };
        }

        // Create a pipeline layout from Shader stages
        auto&& num_descriptorset = vs->m_descriptorset_layoutdata.size() + ps->m_descriptorset_layoutdata.size();
        if (num_descriptorset > 0) {
            m_descriptorset_layouts.reserve(num_descriptorset);
            m_descriptorset_infos.reserve(num_descriptorset);

            {
                auto&& shader_stage = vs;
                for (auto&& layout_data : shader_stage->m_descriptorset_layoutdata) {
                    auto&& layout = device.createDescriptorSetLayout(layout_data.set_create_info);
                    m_descriptorset_layouts.emplace_back(layout);
                    m_descriptorset_infos.emplace_back(&layout_data);
                }
            }

            {
                auto&& shader_stage = ps;
                for (auto&& layout_data : shader_stage->m_descriptorset_layoutdata) {
                    auto&& layout = device.createDescriptorSetLayout(layout_data.set_create_info);
                    m_descriptorset_layouts.emplace_back(layout);
                    m_descriptorset_infos.emplace_back(&layout_data);
                }
            }

            vk::PipelineLayoutCreateInfo pipeline_layout_createinfo{
                .setLayoutCount = (uint32_t)m_descriptorset_layouts.size(),
                .pSetLayouts    = m_descriptorset_layouts.data(),
            };
            m_pipeline_layout = device.createPipelineLayout(pipeline_layout_createinfo);
        }
        else {
            m_pipeline_layout = device.createPipelineLayout(vk::PipelineLayoutCreateInfo{});
        }

        // ----------

        // Fixed pipeline state -----------
        vk::PipelineInputAssemblyStateCreateInfo pipeline_input_assembly_state_createinfo{
            .topology = vk::PrimitiveTopology::eTriangleList};

        vk::PipelineRasterizationStateCreateInfo pipeline_rasterization_state_createinfo{
            .flags                   = vk::PipelineRasterizationStateCreateFlags(),
            .depthClampEnable        = false,                       // depthClampEnable
            .rasterizerDiscardEnable = false,                       // rasterizerDiscardEnable
            .polygonMode             = vk::PolygonMode::eFill,      // polygonMode
            .cullMode                = vk::CullModeFlagBits::eBack, // cullMode
            .frontFace               = vk::FrontFace::eClockwise,   // frontFace
            .depthBiasEnable         = false,                       // depthBiasEnable
            .depthBiasConstantFactor = 0.0f,                        // depthBiasConstantFactor
            .depthBiasClamp          = 0.0f,                        // depthBiasClamp
            .depthBiasSlopeFactor    = 0.0f,                        // depthBiasSlopeFactor
            .lineWidth               = 1.0f,                        // lineWidth
        };

        vk::PipelineMultisampleStateCreateInfo pipeline_multisample_state_createinfo{
            .flags                = vk::PipelineMultisampleStateCreateFlags(), // flags
            .rasterizationSamples = vk::SampleCountFlagBits::e1,               // rasterizationSamples
                                                                               // other values can be default
        };

        vk::StencilOpState stencil_op_state(
            vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::StencilOp::eKeep, vk::CompareOp::eAlways);
        vk::PipelineDepthStencilStateCreateInfo pipeline_depth_stencil_state_createinfo{
            .flags                 = vk::PipelineDepthStencilStateCreateFlags(), // flags
            .depthTestEnable       = true,                                       // depthTestEnable
            .depthWriteEnable      = true,                                       // depthWriteEnable
            .depthCompareOp        = vk::CompareOp::eLessOrEqual,                // depthCompareOp
            .depthBoundsTestEnable = false,                                      // depthBoundTestEnable
            .stencilTestEnable     = false,                                      // stencilTestEnable
            .front                 = stencil_op_state,                           // front
            .back                  = stencil_op_state,                           // back
        };

        vk::ColorComponentFlags color_component_flags(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                                      vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA);
        vk::PipelineColorBlendAttachmentState pipeline_color_blend_attachment_state(false, // blendEnable
            vk::BlendFactor::eZero,                                                        // srcColorBlendFactor
            vk::BlendFactor::eZero,                                                        // dstColorBlendFactor
            vk::BlendOp::eAdd,                                                             // colorBlendOp
            vk::BlendFactor::eZero,                                                        // srcAlphaBlendFactor
            vk::BlendFactor::eZero,                                                        // dstAlphaBlendFactor
            vk::BlendOp::eAdd,                                                             // alphaBlendOp
            color_component_flags                                                          // colorWriteMask
        );

        vk::PipelineColorBlendStateCreateInfo pipeline_color_blend_state_createinfo{
            .flags           = vk::PipelineColorBlendStateCreateFlags(), // flags
            .logicOpEnable   = false,                                    // logicOpEnable
            .logicOp         = vk::LogicOp::eNoOp,                       // logicOp
            .attachmentCount = 1,
            .pAttachments    = &pipeline_color_blend_attachment_state, // attachments
            .blendConstants  = {{1.0f, 1.0f, 1.0f, 1.0f}}              // blendConstants
        };

        const int num_render_targets = 1;

        vk::PipelineViewportStateCreateInfo pipeline_viewport_state_createinfo{
            .flags         = vk::PipelineViewportStateCreateFlags(),
            .viewportCount = num_render_targets,
            .scissorCount  = num_render_targets,
        };

        vk::PipelineRenderingCreateInfo render_info{
            .colorAttachmentCount    = num_render_targets,
            .pColorAttachmentFormats = &color_format,
            .depthAttachmentFormat   = depth_format,
        };

        // Dynamic states
        std::array<vk::DynamicState, 2> dynamic_states = {
            vk::DynamicState::eViewport, vk::DynamicState::eScissor,
            // vk::DynamicState::eCullMode,
            // vk::DynamicState::eFrontFace,
            // vk::DynamicState::ePrimitiveTopology,
        };

        vk::PipelineDynamicStateCreateInfo pipeline_dynamic_state_createinfo{
            .dynamicStateCount = (uint32_t)dynamic_states.size(),
            .pDynamicStates    = dynamic_states.data(),
        };
        // -----------

        vk::GraphicsPipelineCreateInfo graphics_pipeline_createinfo{
            .pNext               = &render_info,
            .flags               = vk::PipelineCreateFlags(),               // flags
            .stageCount          = pipeline_shader_stage_createinfo.size(), // stages
            .pStages             = pipeline_shader_stage_createinfo.data(),
            .pVertexInputState   = &pipeline_vertex_input_state_createinfo,   // pVertexInputState
            .pInputAssemblyState = &pipeline_input_assembly_state_createinfo, // pInputAssemblyState
            .pTessellationState  = nullptr,                                   // pTessellationState
            .pViewportState      = &pipeline_viewport_state_createinfo,       // pViewportState
            .pRasterizationState = &pipeline_rasterization_state_createinfo,  // pRasterizationState
            .pMultisampleState   = &pipeline_multisample_state_createinfo,    // pMultisampleState
            .pDepthStencilState  = &pipeline_depth_stencil_state_createinfo,  // pDepthStencilState
            .pColorBlendState    = &pipeline_color_blend_state_createinfo,    // pColorBlendState
            .pDynamicState       = &pipeline_dynamic_state_createinfo,        // pDynamicState
            .layout              = m_pipeline_layout,                         // layout
            .renderPass          = nullptr, // renderPass, and since we are using dynamic rendering this will set as null
            .subpass             = 0,
        };

        vk::Result result;
        vk::Pipeline pipeline;
        std::tie(result, pipeline) = device.createGraphicsPipeline(nullptr, graphics_pipeline_createinfo);
        switch (result) {
        case vk::Result::eSuccess:
            break;
        case vk::Result::ePipelineCompileRequiredEXT:
            // something meaningful here
            break;
        default:
            assert(false); // should never happen
        }

        m_pipeline = pipeline;
    }

} // namespace VKN
