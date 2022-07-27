#define GL_GLEXT_PROTOTYPES 1

#include <iostream>
#include <fstream>
#include <unistd.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "rtmath.h"

using namespace std;

float vertices[] = {-1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f,
                    1.0f, 1.0f, 0.0f, 1.0f, -1.0f, 0.0f, -1.0f, 1.0f, 0.0f};

GLuint loadShaders(string vertFileSrc, string fragFileSrc) {
    ifstream vertFile(vertFileSrc);
    ifstream fragFile(fragFileSrc);
    
    string vertShaderStr((istreambuf_iterator<char>(vertFile)), istreambuf_iterator<char>());
    string fragShaderStr((istreambuf_iterator<char>(fragFile)), istreambuf_iterator<char>());

    const char *vertShaderChr = vertShaderStr.c_str();
    const char *fragShaderChr = fragShaderStr.c_str();

    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertShaderChr, NULL);
    glCompileShader(vertShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << endl;
    }

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragShaderChr, NULL);
    glCompileShader(fragShader);

    glGetShaderiv(fragShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragShader, 512, NULL, infoLog);
        cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << endl;
    }

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    return shaderProgram;
}

int main(int argc, char *argv[]) {
    srand(time(nullptr));

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window = SDL_CreateWindow("Ray Tracing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_MAXIMIZED | SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL);
    SDL_SetRelativeMouseMode(SDL_TRUE);

    int curwidth, curheight;
    SDL_GetWindowSize(window, &curwidth, &curheight);

    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    SDL_GL_SetSwapInterval(0);

    GLuint renderShader = loadShaders("shader.vert", "render.frag");
    GLuint drawShader = loadShaders("shader.vert", "draw.frag");

    GLuint VBO, VAO;

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    GLuint framebuffers[2];
    glGenFramebuffers(2, framebuffers);

    GLuint textures[2];
    glGenTextures(2, textures);

    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, curwidth, curheight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, curwidth, curheight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[0]);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textures[0], 0);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[1]);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textures[1], 0);
    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers);

    bool running = true;
    bool mouseGrab = true;
    bool moving[6] = {false, false, false, false, false, false};

    vec3 position;
    vec2 direction;
    int framesStill = 1;

    while(running) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    switch(event.key.keysym.scancode) {
                        case SDL_SCANCODE_W:
                            moving[0] = true;
                            break;
                        case SDL_SCANCODE_A:
                            moving[1] = true;
                            break;
                        case SDL_SCANCODE_S:
                            moving[2] = true;
                            break;
                        case SDL_SCANCODE_D:
                            moving[3] = true;
                            break;
                        case SDL_SCANCODE_SPACE:
                            moving[4] = true;
                            break;
                        case SDL_SCANCODE_LSHIFT:
                            moving[5] = true;
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch(event.key.keysym.scancode) {
                        case SDL_SCANCODE_W:
                            moving[0] = false;
                            break;
                        case SDL_SCANCODE_A:
                            moving[1] = false;
                            break;
                        case SDL_SCANCODE_S:
                            moving[2] = false;
                            break;
                        case SDL_SCANCODE_D:
                            moving[3] = false;
                            break;
                        case SDL_SCANCODE_SPACE:
                            moving[4] = false;
                            break;
                        case SDL_SCANCODE_LSHIFT:
                            moving[5] = false;
                            break;
                        case SDL_SCANCODE_ESCAPE:
                            mouseGrab = !mouseGrab;
                            SDL_SetRelativeMouseMode(mouseGrab ? SDL_TRUE : SDL_FALSE);
                            break;
                    }
                    break;
            }
        }

        if(moving[0])
            position += vec3(direction.x, 0)*0.25;
        if(moving[2])
            position -= vec3(direction.x, 0)*0.25;
        if(moving[1])
            position -= vec3(direction.x+M_PI_2, 0)*0.25;
        if(moving[3])
            position += vec3(direction.x+M_PI_2, 0)*0.25;
        if(moving[4])
            position.z += 0.25;
        if(moving[5])
            position.z -= 0.25;
        
        int mouseX = 0, mouseY = 0;
        if(mouseGrab) {
            SDL_GetRelativeMouseState(&mouseX, &mouseY);
            direction += vec2(mouseX, -mouseY)*0.0025;
            if(direction.y < -M_PI_2)
                direction.y = -M_PI_2;
            else if(direction.y > M_PI_2)
                direction.y = M_PI_2;
        }
        
        if(mouseX == 0 && mouseY == 0 && moving[0] == false && moving[1] == false && moving[2] == false && moving[3] == false && moving[4] == false && moving[5] == false)
            framesStill++;
        else
            framesStill = 1;
        
        int prevw = curwidth;
        int prevh = curheight;
        SDL_GetWindowSize(window, &curwidth, &curheight);
        if(prevw != curwidth || prevh != curheight) {
            glBindTexture(GL_TEXTURE_2D, textures[0]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, curwidth, curheight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
            glBindTexture(GL_TEXTURE_2D, textures[1]);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, curwidth, curheight, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        }

        glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[framesStill % 2]);
        glViewport(0, 0, curwidth, curheight);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(renderShader);
        glUniform2f(glGetUniformLocation(renderShader, "u_resolution"), curwidth, curheight);
        glUniform3f(glGetUniformLocation(renderShader, "u_position"), position.x, position.y, position.z);
        glUniform2f(glGetUniformLocation(renderShader, "u_direction"), direction.x, direction.y);
        glUniform1f(glGetUniformLocation(renderShader, "u_frames"), (float)framesStill);
        glUniform1f(glGetUniformLocation(renderShader, "u_rand"), (float)rand()/(float)RAND_MAX);

        glBindTexture(GL_TEXTURE_2D, textures[(framesStill + 1) % 2]);
        glUniform1i(glGetUniformLocation(renderShader, "u_sample"), 0);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices));

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, curwidth, curheight);
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(drawShader);
        glUniform2f(glGetUniformLocation(drawShader, "u_resolution"), curwidth, curheight);
        glBindTexture(GL_TEXTURE_2D, textures[framesStill % 2]);
        glUniform1i(glGetUniformLocation(drawShader, "u_sample"), 0);

        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices));

        SDL_GL_SwapWindow(window);

        usleep(1000000 / 75);
    }

    return 0;
}
