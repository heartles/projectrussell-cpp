#include <array>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <typeinfo>
#include <unistd.h>

#include "GLFW/glfw3.h"

#include "common.h"
#include "game.h"
#include "graphics.h"
#include "audio.h"
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
    std::size_t size = 32;
    char *path = new char[size];

    while(!getcwd(path, size)) {
        assert(errno == ERANGE);
        delete[] path;
        size += 32;
        path = new char[size];
    }

    std::string result =
      (path ? path : throw std::runtime_error("getcwd returned nullptr"));
    Log(result);

    free(path);
    return result;
}

int
main(int argc, char **argv)
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

        GLFWwindow *window = glfwCreateWindow(
          mode->width, mode->height, "Unnamed RPG", nullptr, nullptr);
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

		Game g{ GetGameRootDir() };

        g.Screen =
          OrthoView(Rectangle::FromCorner({}, mode->width, mode->height), &g);

        Log("Initializing Game...");
        Game_Init(g);
        double time = glfwGetTime();
        int DEBUG_loop = 0;
        while (!g.ShouldClose) {
            g.ShouldClose = glfwWindowShouldClose(window) != 0;
            ++DEBUG_loop;

            g.OldInput = g.CurrentInput;

            for (size_t i = 0; i < countof_array(g.CurrentInput.Keyboard); ++i) {
                g.CurrentInput.Keyboard[i] = glfwGetKey(window, i) != 0;
            }

            for (size_t i = 0; i < countof_array(g.CurrentInput.Mouse); ++i) {
                g.CurrentInput.Mouse[i] = glfwGetMouseButton(window, i) != 0;
            }

            int x, y;
            glfwGetWindowSize(window, &x, &y);
            double mx, my;
            glfwGetCursorPos(window, &mx, &my);

            g.CurrentInput.MouseX = 2 * static_cast<float>(mx) / x - 1;
            g.CurrentInput.MouseY = 1 - static_cast<float>(my) / y * 2;

            Game_Update(g);

            glClearColor(
              g.ClearColor.r, g.ClearColor.g, g.ClearColor.b, g.ClearColor.a);
            glClear(GL_COLOR_BUFFER_BIT);
            Game_Render(g);
			g.Audio.RenderSound(g.DT);

            glfwSwapBuffers(window);
            glfwPollEvents();

            const double newTime = glfwGetTime();
            g.DT = static_cast<float>(newTime - time);

            const float fps = 1 / g.DT;

            if (fps < 59) {

                std::printf("Missed a frame! Game loop iteration %d, %f fps\n",
                            DEBUG_loop,
                            fps);
            }

            // std::printf("FPS: %f\n", fps);

            time = newTime;
        }

        glfwTerminate();

#ifdef DEBUG_EXCEPTION_HANDLE
    } catch (std::exception e) {
        std::cout << e.what() << endl;
        std::exit(-1);
    }
#endif

    return 0;
}
