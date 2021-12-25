# TestVulkanPayloadArray

A test program for checking whether arrays can be used as Vulkan ray tracing payload data.

## How to build

To build the program, please use `build-windows.bat` (inside of a Visual Studio Developer PowerShell on Windows)
or `build-linux.bat` on Linux. The build process was tested on Windows 10 with Visual Studio 2022 and Ubuntu 20.04.

**The build process might take some time on Windows**, as vcpkg is used for compiling and installing all project
dependencies.

## Problem description

When started, the program should show a red triangle on a green background. However, a blue triangle is rendered.

Different statements in the first hit shader in Data/Shaders/TestRayTracing.glsl that should be equivalent may produce
either a red (correct) or blue (incorrect) triangle.

![Correct](/home/christoph/Programming/C++/TestVulkanPayloadArray/screenshots/incorrect.png)
Figure 1. The incorrect result.

![Correct](/home/christoph/Programming/C++/TestVulkanPayloadArray/screenshots/correct.png)
Figure 2. The correct result.

## Shader code

```glsl
-- RayGen

#version 460
#extension GL_EXT_ray_tracing : require

layout (binding = 0) uniform CameraSettings {
    mat4 inverseViewMatrix;
    mat4 inverseProjectionMatrix;
} camera;

layout(binding = 1, rgba8) uniform image2D outputImage;

layout(binding = 2) uniform accelerationStructureEXT topLevelAS;

struct RayPayload {
    vec4 colors[1];
};
layout(location = 0) rayPayloadEXT RayPayload payload;

void main() {
    vec2 fragNdc = 2.0 * ((vec2(gl_LaunchIDEXT.xy) + vec2(0.5)) / vec2(gl_LaunchSizeEXT.xy)) - 1.0;

    vec3 rayOrigin = (camera.inverseViewMatrix * vec4(0.0, 0.0, 0.0, 1.0)).xyz;
    vec3 rayTarget = (camera.inverseProjectionMatrix * vec4(fragNdc.xy, 1.0, 1.0)).xyz;
    vec3 rayDirection = (camera.inverseViewMatrix * vec4(normalize(rayTarget.xyz), 0.0)).xyz;

    for (int i = 0; i < 1; i++) {
        payload.colors[i] = vec4(0.0, 0.0, 1.0, 1.0);
    }

    float tMin = 0.0001f;
    float tMax = 1000.0f;
    traceRayEXT(topLevelAS, gl_RayFlagsOpaqueEXT, 0xFF, 0, 0, 0, rayOrigin, tMin, rayDirection, tMax, 0);

    vec4 color = payload.colors[0];
    imageStore(outputImage, ivec2(gl_LaunchIDEXT.xy), color);
}

-- Miss

#version 460
#extension GL_EXT_ray_tracing : require

struct RayPayload {
    vec4 colors[1];
};
layout(location = 0) rayPayloadInEXT RayPayload payload;

void main() {
    payload.colors[0] = vec4(0.0, 1.0, 0.0, 1.0);
}

-- ClosestHit

#version 460
#extension GL_EXT_ray_tracing : require

struct RayPayload {
    vec4 colors[1];
};
layout(location = 0) rayPayloadInEXT RayPayload payload;

void swap(inout vec4 a, inout vec4 b) {
    vec4 temp = a;
    a = b;
    b = temp;
}

void main() {
    vec4 newColor = vec4(1.0, 0.0, 0.0, 1.0);
    for (int i = 0; i < 1; i++) {
        // Doesn't work.
        swap(newColor, payload.colors[0]);

        // Works.
        //swap(newColor, payload.colors[0]);

        // Works.
        //payload.colors[i] = newColor;

        // Doesn't work.
        //vec4 temp = payload.colors[i];
        //payload.colors[i] = newColor;
        //newColor = temp;

        // Doesn't work.
        //vec4 temp = newColor;
        //newColor = payload.colors[i];
        //payload.colors[i] = temp;
    }
}
```
