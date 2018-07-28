#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <cassert>

#include "Chip8.h"
#include "opcodes.h"

// For Windowing and input
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

// Include the tests
#include "tests.h"

void LoadROM(Chip8* chip8, const char* file_name)
{
    // Load ROM into the memory (starting at 0x200)
    FILE* f = fopen(file_name, "rb");
    if(f)
    {
        uint32_t bytes_read = 0;
        // Read directly into memory
        while(!feof(f))
        {
            // Read into memory
            fread(&chip8->memory[0x200 + bytes_read], 1, 1, f);
            bytes_read++;
        }
        
        fclose(f);
    }
    else
    {
        // Failed to load
        printf("Failed to load ROM %s\n", file_name);
        exit(1);
    }
}

void C8Initialise(Chip8* chip8)
{
    // Setup default program counter
    chip8->pc = 0x200;
    
    chip8->opcode = 0;
    
    // Setup fonts
    for(int i = 0; i < 80; ++i)
        chip8->memory[i] = chip8_fontset[i];
}

void C8GetOpcode(Chip8* chip8)
{
    // Get opcode at current program counter location
    chip8->opcode = (chip8->memory[chip8->pc] << 8) | chip8->memory[chip8->pc+1];
}

void C8EmulateCycle(Chip8* chip8)
{
    // Get the current opcode
    C8GetOpcode(chip8);
    
    // Increment the PC
    chip8->pc +=2;
    
    assert(chip8->pc < MEMSIZE);
    
    // Opcode is now in memory, decode
    //printf("Opcode: 0x%X\n", chip8->opcode);
    opcode_table[chip8->opcode >> 12](chip8);
}

void C8DecrementCounters(Chip8* chip8)
{
    Clock_Time current_time = Clock::now();
    int64_t nanoseconds = PerfNano_Counter(current_time - chip8->start_time).count();
    
    if((nanoseconds / 1000000) > 16)
    {
        if(chip8->delay_timer != 0)
            --chip8->delay_timer;
        
        if(chip8->sound_timer != 0)
            --chip8->sound_timer;
        
        chip8->start_time = Clock::now();
    }
}

#include <iostream>
void PrintGLFWErr(int error, const char* description)
{
    std::cout << "Error " << error << ": " << description << std::endl; 
}

void OpenGLErrCallback(GLenum /*source*/, GLenum type, GLuint id, GLenum severity, GLsizei /*length*/, const GLchar* message, const void* /*userParam*/)
{
    // Basic error message
    std::string errMsg = std::string(message);
    
    // Type of message
    std::string errType;
    
    switch (type) 
    {
        case GL_DEBUG_TYPE_ERROR:
        errType = "ERROR";
        break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        errType = "DEPRECATED_BEHAVIOR";
        break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        errType = "UNDEFINED_BEHAVIOR";
        break;
        case GL_DEBUG_TYPE_PORTABILITY:
        errType = "PORTABILITY";
        break;
        case GL_DEBUG_TYPE_PERFORMANCE:
        errType = "PERFORMANCE";
        break;
        case GL_DEBUG_TYPE_OTHER:
        errType = "OTHER";
        break;
    }
    
    // Message ID
    //uint32_t errId = id;
    
    // Message Severity
    std::string errSeverity;
    
    switch (severity){
        case GL_DEBUG_SEVERITY_LOW:
        errSeverity = "LOW";
        break;
        case GL_DEBUG_SEVERITY_MEDIUM:
        errSeverity = "MEDIUM";
        break;
        case GL_DEBUG_SEVERITY_HIGH:
        errSeverity = "HIGH";
        break;
    }
    
    std::cout << "GLDebug( [" << id << "] " << errType << ":" << errSeverity << ") - " << errMsg<< std::endl; 
}

void CreateWindow(Screen* screen)
{
    if(!glfwInit())
    {
        printf("Failed to initialise the application Window!");
    }
    
    // Set the GLFW error callback
    glfwSetErrorCallback(PrintGLFWErr);
    
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, screen->glContextMajorVersion);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, screen->glContextMinorVersion);
    
    // Set 24 bit depth buffer
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    
    screen->window = glfwCreateWindow(screen->window_width,
                                      screen->window_height, 
                                      screen->window_title.c_str(),
                                      NULL, NULL);
    
    if(!screen->window)
    {
        printf("Failed to create window\n");
        exit(1);
    }
    
    glfwMakeContextCurrent(screen->window);
    
    // Initialise GLEW
    // Note (James): GL_INVALID_ENUM can be ignored here looks to be a long running
    // issue with GLEW and the 3.2+ core profile and methods of getting values
    glewExperimental = GL_TRUE;
    glewInit();
    
    printf("Enabling OpenGL Debug Logging...\n");
    
    // Enable the debug callback
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(OpenGLErrCallback, nullptr);
    glDebugMessageControl(
        GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL, true
        );
    printf("Enabled debug logging successfully\n");
    
    glfwSwapInterval(0);
}

void CreateBackbuffer(Screen* screen)
{
    // Pack to 1 byte alignment as we only have 1 colour in our
    // texture
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &screen->back_buffer);
    
    // Bind at this point, we only have one texture so just leave it alone
    glBindTexture(GL_TEXTURE_2D, screen->back_buffer);
    
    // Create an empty texture
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, screen->width, screen->height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //glGenerateMipmap(GL_TEXTURE_2D);
}

void CreateScreenQuad(Screen* screen)
{
    glGenVertexArrays(1, &screen->vertex_array_id);
    glBindVertexArray(screen->vertex_array_id);
    
    glEnableVertexAttribArray(0);
    glGenBuffers(1, &screen->vertex_buffer_verts);
    glBindBuffer(GL_ARRAY_BUFFER, screen->vertex_buffer_verts);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screen_quad_vert), screen_quad_vert, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0,                  
                          3,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );
    
    glEnableVertexAttribArray(1);
    glGenBuffers(1, &screen->vertex_buffer_uv);
    glBindBuffer(GL_ARRAY_BUFFER, screen->vertex_buffer_uv);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screen_quad_uv), screen_quad_uv, GL_STATIC_DRAW);
    
    glVertexAttribPointer(1,                  
                          2,                  // size
                          GL_FLOAT,           // type
                          GL_FALSE,           // normalized?
                          0,                  // stride
                          (void*)0            // array buffer offset
                          );
    
    
    glBindVertexArray(0);
}

void UpdateScreen(Chip8* chip8, Screen* screen)
{
    glBindTexture(GL_TEXTURE_2D, screen->back_buffer);
    
    // Copy the screen bytes from the chip to the texture
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, screen->width, screen->height, GL_RED, GL_UNSIGNED_BYTE, chip8->gfx);
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

void DrawScreen(Screen* screen)
{
    // Clear the screen
    glClearColor(0.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    
    // Bind shader
    glUseProgram(screen->shader_program);
    
    // Bind textures
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(screen->vertex_array_id);
    glBindTexture(GL_TEXTURE_2D, screen->back_buffer);
    
    // Draw the FSQ
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    // Swap buffers
    glfwSwapBuffers(screen->window);
    
    glfwPollEvents();
}

void CreateShader(Screen* screen)
{
    const char* vertex_shader =
        "#version 400\n"
        "layout(location = 0) in vec3 vp;"
        "layout(location = 1) in vec2 uv;"
    
        "out vec2 UV;"
    
        "void main() {"
        "  gl_Position = vec4(vp, 1.0);"
        "  UV = vec2(uv.r, 1-uv.g);"
        "}";
    
    const char* fragment_shader =
        "#version 400\n"
        "in vec2 UV;\n"
        "uniform sampler2D myTextureSampler;\n"
        "out vec3 frag_colour;\n"
        "void main() {\n"
        "  float p = texture( myTextureSampler, UV ).r;\n"
        "  frag_colour = vec3(p, p, p);\n"
        "}\n";
    
    screen->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(screen->vertex_shader, 1, &vertex_shader, NULL);
    glCompileShader(screen->vertex_shader);
    
    screen->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(screen->fragment_shader, 1, &fragment_shader, NULL);
    glCompileShader(screen->fragment_shader);
    
    screen->shader_program = glCreateProgram();
    glAttachShader(screen->shader_program, screen->fragment_shader);
    glAttachShader(screen->shader_program, screen->vertex_shader);
    glLinkProgram(screen->shader_program);
    
    glUseProgram(0);
}

#include <unistd.h>
int main()
{
    // Do tests
    TestAll();
    
    // The Chip8 Chip
    Chip8 chip8 = {};
    C8Initialise(&chip8);
    LoadROM(&chip8, "./games/LANDER");
    
    Screen screen;
    screen.window_title = "Test";
    screen.window_width = 640;
    screen.window_height = 480;
    CreateWindow(&screen);
    CreateBackbuffer(&screen);
    CreateScreenQuad(&screen);
    CreateShader(&screen);
    
    chip8.screen = &screen;
    chip8.start_time = Clock::now();
    C8SetupInput(&chip8);
    
    for(;;)
    {
        // Get keys
        C8GetInput(&chip8);
        
        // Emulate CPU
        C8EmulateCycle(&chip8);
        
        // Decrement counters
        C8DecrementCounters(&chip8);
        
        if(chip8.draw_flag)
        {
            // Update texture and blit to screen
            UpdateScreen(&chip8, &screen);
            chip8.draw_flag = false;
        }
        
        DrawScreen(&screen);
        
        usleep(400);
    }
}

