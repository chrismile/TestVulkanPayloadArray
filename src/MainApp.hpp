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

#ifndef MAINAPP_HPP
#define MAINAPP_HPP

#include <string>
#include <vector>
#include <map>

#include <Utils/SciVis/SciVisApp.hpp>
#include <Graphics/Shader/Shader.hpp>
#include <Graphics/Vulkan/Render/Data.hpp>
#include <ImGui/Widgets/TransferFunctionWindow.hpp>

class RayTracingRenderPass;

class MainApp : public sgl::SciVisApp {
public:
    MainApp();
    ~MainApp() override;
    void render() override;
    void update(float dt) override;
    void resolutionChanged(sgl::EventPtr event) override;

private:
    void renderGui() override;
    void reloadDataSet() override {}

    // Coloring & filtering dependent on importance criteria.
    sgl::TransferFunctionWindow transferFunctionWindow;

    // Render passes.
    std::shared_ptr<RayTracingRenderPass> rayTracingRenderPass;
};

#endif // MAINAPP_HPP