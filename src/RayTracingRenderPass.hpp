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

#ifndef TESTVULKANSGL_RAYTRACINGRENDERPASS_HPP
#define TESTVULKANSGL_RAYTRACINGRENDERPASS_HPP

#include <Graphics/Vulkan/Render/Passes/Pass.hpp>
#include <Graphics/Vulkan/Render/AccelerationStructure.hpp>

class RayTracingRenderPass : public sgl::vk::RayTracingPass {
public:
    explicit RayTracingRenderPass(sgl::vk::Renderer* renderer);

    // Public interface.
    void setOutputImage(sgl::vk::ImageViewPtr& colorImage);

private:
    void loadShader() override;
    sgl::vk::RayTracingPipelinePtr createRayTracingPipeline() override;
    void createRayTracingData(sgl::vk::Renderer* renderer, sgl::vk::RayTracingPipelinePtr& rayTracingPipeline) override;
    void _render() override;

    sgl::vk::ImageViewPtr sceneImageView;

    void setupGeometryBuffers();
    sgl::vk::BufferPtr indexBuffer;
    sgl::vk::BufferPtr vertexBuffer;

    // Uniform buffer object storing the camera settings.
    struct CameraSettings {
        glm::mat4 inverseViewMatrix;
        glm::mat4 inverseProjectionMatrix;
    };
    CameraSettings cameraSettings{};
    sgl::vk::BufferPtr cameraSettingsBuffer;

    // Acceleration structure.
    sgl::vk::TopLevelAccelerationStructurePtr topLevelAS;
};

#endif //TESTVULKANSGL_RAYTRACINGRENDERPASS_HPP
