#version 450


layout(binding = 0) uniform UniformBufferObject {
    vec3 uColor;
    vec3 uPos;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {

    mat4 model = mat4(1.0, 0.0, 0.0, 0.0,
                       0.0, 1.0, 0.0, 0.0,
                       0.0, 0.0, 1.0, 0.0,
                       ubo.uPos.x, ubo.uPos.y, ubo.uPos.z, 1.0);

    
    

    gl_Position = model * vec4(inPosition, 1.0);
    fragColor = ubo.uColor;
}
