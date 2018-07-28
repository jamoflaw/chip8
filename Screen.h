#ifndef _SCREEN_H
#define _SCREEN_H

// For Windowing and input
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

const float screen_quad_vert[] = {
    -1.0f, -1.0f, 0.0f,
    1.0f,  -1.0f, 0.0f,
    1.0f,  1.0f,  0.0f,
    -1.0f, -1.0f, 0.0f,
    1.0f,  1.0f,  0.0f,
    -1.0f, 1.0f,  0.0f,
};

const float screen_quad_uv[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 0.0f,
    1.0f, 1.0f,
    0.0f, 1.0f,
};

struct Screen
{
    const uint32_t width = SCREEN_WIDTH;
    const uint32_t height = SCREEN_HEIGHT;
    
    GLFWwindow* window;
    
    std::string window_title;
    uint32_t window_width;
    uint32_t window_height;
    
    // Set the default GL version
    uint32_t glContextMajorVersion = 3;
    uint32_t glContextMinorVersion = 3;
    
    // Screen backbuffer
    uint32_t back_buffer;
    
    // Screen Vertex Buffer Objects
    uint32_t vertex_array_id;
    uint32_t vertex_buffer_verts;
    uint32_t vertex_buffer_uv;
    
    // Screen Shaders
    uint32_t shader_program;
    
    uint32_t vertex_shader;
    uint32_t fragment_shader;
    
    uint32_t texture_sampler;
};

#endif