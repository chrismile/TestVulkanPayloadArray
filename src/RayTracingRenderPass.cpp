/*
 * BSD 2-Clause License
 *
 * Copyright (c) 2021, Christoph Neuhauser
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 *
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <memory>
#include <glm/vec3.hpp>

#include <Utils/StringUtils.hpp>
#include <Graphics/Vulkan/Utils/Device.hpp>
#include <Graphics/Vulkan/Buffers/Framebuffer.hpp>
#include <Graphics/Vulkan/Render/RayTracingPipeline.hpp>
#include <Graphics/Vulkan/Render/Renderer.hpp>
#include "RayTracingRenderPass.hpp"

RayTracingRenderPass::RayTracingRenderPass(sgl::vk::Renderer* renderer) : RayTracingPass(renderer) {
    setupGeometryBuffers();
}

void RayTracingRenderPass::setOutputImage(sgl::vk::ImageViewPtr& imageView) {
    sceneImageView = imageView;

    if (rayTracingData) {
        rayTracingData->setStaticImageView(sceneImageView, 1);
    }
}

void RayTracingRenderPass::setupGeometryBuffers() {
    std::vector<uint32_t> triangleIndices = {0, 1, 2};
    std::vector<glm::vec3> vertexPositions = {
            {-0.5f, -0.5f, 0.0f},
            {0.5f,  -0.5f, 0.0f},
            {0.0f,   0.5f, 0.0f},
    };
    indexBuffer = std::make_shared<sgl::vk::Buffer>(
            device, triangleIndices.size() * sizeof(uint32_t), triangleIndices.data(),
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
            | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
            VMA_MEMORY_USAGE_GPU_ONLY);
    vertexBuffer = std::make_shared<sgl::vk::Buffer>(
            device, vertexPositions.size() * sizeof(glm::vec3), vertexPositions.data(),
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
            VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT
            | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR,
            VMA_MEMORY_USAGE_GPU_ONLY);
    cameraSettingsBuffer = std::make_shared<sgl::vk::Buffer>(
            device, sizeof(CameraSettings),
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VMA_MEMORY_USAGE_GPU_ONLY);

    auto asInput = new sgl::vk::TrianglesAccelerationStructureInput(device);
    asInput->setIndexBuffer(indexBuffer);
    asInput->setVertexBuffer(vertexBuffer, VK_FORMAT_R32G32B32_SFLOAT);
    auto asInputPtr = sgl::vk::BottomLevelAccelerationStructureInputPtr(asInput);

    sgl::vk::BottomLevelAccelerationStructurePtr blas = buildBottomLevelAccelerationStructureFromInput(asInputPtr);

    topLevelAS = std::make_shared<sgl::vk::TopLevelAccelerationStructure>(device);
    topLevelAS->build({ blas }, { sgl::vk::BlasInstance() });
}

void RayTracingRenderPass::loadShader() {
    shaderStages = sgl::vk::ShaderManager->getShaderStages(
            {"TestRayTracing.RayGen", "TestRayTracing.Miss", "TestRayTracing.ClosestHit"});
}

void RayTracingRenderPass::createRayTracingData(
        sgl::vk::Renderer* renderer, sgl::vk::RayTracingPipelinePtr& rayTracingPipeline) {
    rayTracingData = std::make_shared<sgl::vk::RayTracingData>(renderer, rayTracingPipeline);
    rayTracingData->setStaticBuffer(cameraSettingsBuffer, 0);
    rayTracingData->setStaticImageView(sceneImageView, 1);
    rayTracingData->setTopLevelAccelerationStructure(topLevelAS, 2);
}

void RayTracingRenderPass::_render() {
    cameraSettings.inverseViewMatrix = glm::inverse(renderer->getViewMatrix());
    cameraSettings.inverseProjectionMatrix = glm::inverse(renderer->getProjectionMatrix());
    cameraSettingsBuffer->updateData(
            sizeof(CameraSettings), &cameraSettings, renderer->getVkCommandBuffer());

    renderer->transitionImageLayout(sceneImageView->getImage(), VK_IMAGE_LAYOUT_GENERAL);
    renderer->traceRays(rayTracingData, launchSizeX, launchSizeY, launchSizeZ);
    renderer->transitionImageLayout(sceneImageView->getImage(), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
}

sgl::vk::RayTracingPipelinePtr RayTracingRenderPass::createRayTracingPipeline() {
    sgl::vk::ShaderBindingTable sbt = sgl::vk::ShaderBindingTable::generateSimpleShaderBindingTable(shaderStages);
    sgl::vk::RayTracingPipelineInfo rayTracingPipelineInfo(sbt);
    return std::make_shared<sgl::vk::RayTracingPipeline>(device, rayTracingPipelineInfo);
}
