# TestVulkanPayloadArray

A test program for checking whether arrays can be used as Vulkan ray tracing payload data.

When started, the program should show a red triangle on a green background. However, a blue triangle is rendered.

Different equivalent statements in the first hit shader in Data/Shaders/TestRayTracing.glsl may produce either a red
(correct) or blue (incorrect) triangle.
