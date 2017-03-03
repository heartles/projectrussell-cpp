#include <array>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <typeinfo>

#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtx/transform.hpp"

#include "common.h"
#include "game.h"
#include "graphics.h"

using namespace std;

#ifdef WINDOWS
#define getcwd _getcwd
#include <direct.h>
#endif

std::vector<std::string> Args;

static_assert(sizeof(vec3) == sizeof(GLfloat) * 3, "Struct padding in vec3");
static_assert(sizeof(float) == sizeof(GLfloat), "float != GLfloat");

string
GetGameRootDir()
{
    char* path = getcwd(nullptr, 32);
    std::string result = path;
    Log(result);

    return result;
}

int
main(int argc, char** argv)
{
    for (int i = 0; i < argc; i++) {
        Args.push_back(std::string(argv[i]));
    }

#ifdef DEBUG_EXCEPTION_HANDLE
    try {
#endif

        if (!glfwInit()) {
            Log("Could not initialize glfw");
            return -1;
        }

        auto monitor = glfwGetPrimaryMonitor();
        auto mode = glfwGetVideoMode(monitor);
        glfwWindowHint(GLFW_RED_BITS, mode->redBits);
        glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
        glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        GLFWwindow* window = glfwCreateWindow(mode->width, mode->height,
                                              "Unnamed RPG", monitor, nullptr);
        if (!window) {
            Log("Could not create window");
            glfwTerminate();
            return -1;
        }

        glfwMakeContextCurrent(window);
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            Log("Could not load OpenGL");
            glfwTerminate();
            return -1;
        }

        // glDebugMessageCallback(oglDebug, nullptr);

        glfwSwapInterval(0);

        Game g = Game(GetGameRootDir());

        g.Screen =
          OrthoView(Rectangle::FromCorner({}, mode->width, mode->height), &g);

        Log("Initializing Game...");
        Game_Init(g);
        double time = glfwGetTime();
        int DEBUG_loop = 0;
        while (!g.ShouldClose) {
            g.ShouldClose = glfwWindowShouldClose(window) != 0;
            ++DEBUG_loop;

            g.OldInput = g.Input;

            for (size_t i = 0; i < countof_array(g.Input.Keyboard); ++i) {
                g.Input.Keyboard[i] = glfwGetKey(window, i) != 0;
            }

            for (size_t i = 0; i < countof_array(g.Input.Mouse); ++i) {
                g.Input.Mouse[i] = glfwGetMouseButton(window, i) != 0;
            }

            int x, y;
            glfwGetWindowSize(window, &x, &y);
            double mx, my;
            glfwGetCursorPos(window, &mx, &my);

            g.Input.MouseX = 2 * static_cast<float>(mx) / x - 1;
            g.Input.MouseY = 1 - static_cast<float>(my) / y * 2;

            Game_Update(g);

            glClearColor(g.ClearColor.r, g.ClearColor.g, g.ClearColor.b,
                         g.ClearColor.a);
            glClear(GL_COLOR_BUFFER_BIT);
            Game_Render(g);

            glfwSwapBuffers(window);
            glfwPollEvents();

            double newTime = glfwGetTime();
            g.DT = static_cast<float>(newTime - time);

            double fps = 1 / g.DT;

            if (fps < 59) {

                std::printf("Missed a frame! Game loop iteration %d, %f fps\n",
                            DEBUG_loop, fps);
            }

            // std::printf("FPS: %f\n", fps);

            time = newTime;
        }

        glfwTerminate();

#ifdef DEBUG_EXCEPTION_HANDLE
    }

    catch (std::exception e) {
        std::cout << e.what() << endl;
        std::exit(-1);
    }
#endif

    return 0;
}
