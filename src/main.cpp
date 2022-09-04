#include <iostream>
#include <SDL.h>
#include <span>
#include <fstream>
#include <chrono>
#include <sstream>
#include <filesystem>
#include <glbinding/gl/gl.h>
#include <glbinding/glbinding.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "model.hpp"

using namespace gl;

struct VertexPC {
    glm::vec3 pos;
    glm::vec3 color;
};

uint32_t createShader(std::string vertex_data, std::string fragment_data) {
    const int LOG_SIZE = 512;
    int  success;
    char infoLog[LOG_SIZE];

    const char* raw_vertex_data = vertex_data.c_str();
    const char* raw_fragment_data = fragment_data.c_str();

    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &raw_vertex_data, NULL);
    glCompileShader(vertexShader);
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, LOG_SIZE, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\t" << infoLog << "\n";
    }

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &raw_fragment_data, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, LOG_SIZE, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\t" << infoLog << "\n";
    }

    auto shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, LOG_SIZE, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\t" << infoLog << "\n";
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    return shaderProgram;
}

void printVector(glm::vec3 v) {
    std::cout << '[' << v.x << ',' << v.y << ',' << v.z << "]\n";
}

int main(int argc, char* argv[]) {
    std::span args{argv, static_cast<uint32_t>(argc)};
    std::string filePath = std::string(args[0]);

    std::filesystem::path shaderPath = filePath;
    shaderPath = shaderPath.parent_path().parent_path().append("shaders");

    const uint32_t INITIAL_WIDTH = 800;
    const uint32_t INITIAL_HEIGHT = 600;
    const float ROTATION_SPEED = 5.0;
    const float MOVEMENT_SPEED = 5.0;


    float fov = glm::radians(60.0f);
    int width = 800;
    int height = 600;

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window;
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    window = SDL_CreateWindow("name", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    auto glcontext = SDL_GL_CreateContext(window);
    glbinding::initialize([](const char * name) { return reinterpret_cast<glbinding::ProcAddress>(SDL_GL_GetProcAddress(name)); }, false);

    bool should_window_close = false;
    bool wireframe_mode = false;
    bool capture_mouse = false;
    bool vsync = true;
    bool key_w = false;
    bool key_s = false;
    bool key_a = false;
    bool key_d = false;
    bool key_space = false;
    bool key_shift = false;
    bool key_v = false;
    bool key_b = false;

    bool ignoreNextMouseEvent = false;

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_MULTISAMPLE);
    // glEnable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);
    glDisable(GL_CULL_FACE);

    std::ifstream vertex_file (shaderPath / "cube.vert");
    std::ifstream fragment_file (shaderPath / "cube.frag");

    std::stringstream ss;
    ss << vertex_file.rdbuf();
    std::string vertex_shader_data = ss.str();
    ss.str("");
    ss << fragment_file.rdbuf();
    std::string fragment_shader_data = ss.str();

    unsigned int shaderProgram = createShader(vertex_shader_data, fragment_shader_data);

    std::vector<VertexPC> verts = {
        { { -0.5, -0.5,  0.5 }, { 0.0, 0.0, 1.0 } },
        { {  0.5, -0.5,  0.5 }, { 0.0, 1.0, 0.0 } },
        { { -0.5,  0.5,  0.5 }, { 0.0, 1.0, 1.0 } },
        { {  0.5,  0.5,  0.5 }, { 1.0, 0.0, 0.0 } },
        { { -0.5, -0.5, -0.5 }, { 0.0, 0.0, 1.0 } },
        { {  0.5, -0.5, -0.5 }, { 0.0, 1.0, 0.0 } },
        { { -0.5,  0.5, -0.5 }, { 0.0, 1.0, 1.0 } },
        { {  0.5,  0.5, -0.5 }, { 1.0, 0.0, 0.0 } },
    };
    // 0 1 2 3  4 5 6 7
    std::vector<uint32_t> indicies = {
        3, 0, 1, 
        3, 2, 0,
        2, 4, 0,
        2, 6, 4,
        3, 5, 7,
        3, 1, 5,
        6, 2, 3,
        6, 3, 7,
        4, 1, 0,
        4, 5, 1,
        7, 5, 4,
        7, 4, 6,
    };

    Model cube_mdl {};
    cube_mdl.bufferVertexArray(verts);
    cube_mdl.bufferElementArray(indicies);
    cube_mdl.addAttributeVec3();
    cube_mdl.addAttributeVec3();

    // auto proj = glm::perspectiveFov( fov, (float)width, (float)height, 0.01f, 100.0f );
    auto proj = glm::perspective( fov, (float)width / (float)height, 0.01f, 100.0f );

    glm::vec3 worldUp = { 0.0f, 1.0f, 0.0f };
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 cameraRight = glm::normalize(glm::cross(worldUp, cameraFront));
    glm::vec3 cameraUp = glm::cross(cameraFront, cameraRight);

    float yaw = 0.0, pitch = 0.0;


    auto lastTime = std::chrono::high_resolution_clock::now();

    SDL_Event event;
    while (!should_window_close) {
        while(SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: should_window_close = true; break;
                case SDL_WINDOWEVENT: {
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_RESIZED: {
                            width = event.window.data1;
                            height = event.window.data2;
                            proj = glm::perspective( fov, (float)width / (float)height, 0.01f, 100.0f );
                            glViewport(0, 0, width, height);
                        } break;
                    }
                } break;
                case SDL_KEYDOWN: 
                case SDL_KEYUP: {
                    switch (event.key.keysym.scancode)
                    {
                        case SDL_SCANCODE_Z: {
                            wireframe_mode = !wireframe_mode;
                            if (wireframe_mode)
                                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                            else
                                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                        } break;
                        case SDL_SCANCODE_W: {
                            key_w = event.key.state == SDL_PRESSED;
                        }break;
                        case SDL_SCANCODE_S: {
                            key_s = event.key.state == SDL_PRESSED;
                        }break;
                        case SDL_SCANCODE_A: {
                            key_a = event.key.state == SDL_PRESSED;
                        }break;
                        case SDL_SCANCODE_D: {
                            key_d = event.key.state == SDL_PRESSED;
                        }break;
                        case SDL_SCANCODE_SPACE: {
                            key_space = event.key.state == SDL_PRESSED;
                        }break;
                        case SDL_SCANCODE_LSHIFT: {
                            key_shift = event.key.state == SDL_PRESSED;
                        }break;
                        case SDL_SCANCODE_V: {
                            if (event.key.state == SDL_RELEASED) {
                                vsync = !vsync;
                                SDL_GL_SetSwapInterval(vsync);
                            }
                        }break;
                        case SDL_SCANCODE_B: {
                            if (event.key.state == SDL_RELEASED) {
                                capture_mouse = !capture_mouse;

                                // SDL_CaptureMouse(capture_mouse ? SDL_TRUE : SDL_FALSE);
                                // SDL_SetWindowMouseGrab(window, capture_mouse ? SDL_TRUE : SDL_FALSE);
                                SDL_SetRelativeMouseMode(capture_mouse ? SDL_TRUE : SDL_FALSE);
                                ignoreNextMouseEvent = true;
                            }
                        }break;
                    }
                } break;
                case SDL_MOUSEMOTION: {
                    if (capture_mouse && !ignoreNextMouseEvent) {
                        float sensitivity = 0.2;
                        float xOffset = (float)event.motion.xrel * sensitivity;
                        float yOffset = -(float)event.motion.yrel * sensitivity;

                        yaw += xOffset;
                        pitch += yOffset;
                        if(pitch > 89.0f)
                            pitch =  89.0f; 
                        if(pitch < -89.0f)
                            pitch = -89.0f;

                        glm::vec3 direction;
                        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
                        direction.y = sin(glm::radians(pitch));
                        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
                        cameraFront = glm::normalize(direction);
                        cameraRight = glm::normalize(glm::cross(worldUp, cameraFront));
                        cameraUp = glm::cross(cameraFront, cameraRight);
                        ignoreNextMouseEvent = true;
                        SDL_WarpMouseInWindow(window, width / 2, height / 2);
                    }
                    else
                    {
                        ignoreNextMouseEvent = false;
                    }
                } break;
            }
        }

        const double SECOND_TO_MICRO = 1000000.0;
        auto nowTime = std::chrono::high_resolution_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(nowTime - lastTime).count() / SECOND_TO_MICRO;
        lastTime = nowTime;

        float speed = MOVEMENT_SPEED * elapsedTime;
    
        if (key_a) {
            cameraPos += speed * cameraRight;
        }
        if (key_d) {
            cameraPos -= speed * cameraRight;
        }
        if (key_w) {
            cameraPos += speed * cameraFront;
        }
        if (key_s) {
            cameraPos -= speed * cameraFront;
        }
        if (key_space) {
            cameraPos += speed * cameraUp;
        }
        if (key_shift) {
            cameraPos -= speed * cameraUp;
        }


        auto view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        auto vp = proj * view;

        glClearColor(0.5, 1.0, 0.3, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glUseProgram(shaderProgram);

        // for (int i = 0; i < 16; i++)
        // {
        //     for (int j = 0; j < 16; j++)
        //     {
            //     for (int k = 0; k < 16; k++)
            //     {

        glm::vec3 cube_pos = { (float)0, (float)0, (float)0 };
        auto model = glm::translate(glm::mat4(1.0), cube_pos);
        auto mvp = vp * model;
        unsigned int transformLoc = glGetUniformLocation(shaderProgram, "mvp");
        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(mvp));
        cube_mdl.drawElementsAll();
        //         }
        //     }
        // }

        SDL_GL_SwapWindow(window);

    }
    SDL_DestroyWindow(window);
    SDL_Quit();
}
