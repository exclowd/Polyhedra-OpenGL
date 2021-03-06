#include "main.h"
#include "timer.h"
#include "polyhedra.h"
#include "axes.h"
#include "camera.h"

using namespace std;

GLMatrices Matrices;
GLuint programID;
GLFWwindow *window;

/**************************
* Customizable functions *
**************************/

Polyhedron cube;
Camera camera;
Axes axes;
Camera::MOTION Camera::motion;
Polyhedron::MOTION Polyhedron::motion;

float screen_zoom = 1, screen_center_x = 0, screen_center_y = 0;

Timer t60(1.0 / 60);

/* Render the scene with openGL */
/* Edit this function according to your assignment */
void draw() {
    // clear the color and depth in the frame buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // use the loaded shader program
    // Don't change unless you know what you are doing
    glUseProgram(programID);

    // Target - Where is the camera looking at.  Don't change unless you are sure!!
    if (CAMERA_MOVING) {
        auto mat = camera.rotate();
        Matrices.view = glm::lookAt(camera.pos, cube.position, camera.up) * mat; // Rotating Camera for 3D
    } else if (CAMERA_STATIC) {
        Matrices.view = glm::lookAt(camera.pos, camera.pos - glm::vec3(0, 0, 1), UP);
    } else {
        Matrices.view = glm::lookAt(camera.pos, cube.position, UP);
    }

    // Compute Camera matrix (view)
    // Don't change unless you are sure!!
    // Matrices.view = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0)); // Fixed camera for 2D (ortho) in XY plane

    Matrices.projection = glm::perspective((float) glm::radians(45.0), 1.0f, 0.1f, 100.0f);

    // Compute ViewProject matrix as view/camera might not be changed for this frame (basic scenario)
    // Don't change unless you are sure!!
    glm::mat4 VP = Matrices.projection * Matrices.view;

    // Send our transformation to the currently bound shader, in the "MVP" uniform
    // For each model you render, since the MVP will be different (at least the M part)
    // Don't change unless you are sure!!
    glm::mat4 MVP;  // MVP = Projection * View * Model

    // Scene render
    cube.draw(VP);
    axes.draw(VP);
}

void tick_input(GLFWwindow *glfWwindow) {
    {
        if (glfwGetKey(glfWwindow, GLFW_KEY_J)) {
            Camera::set_state(Camera::MOTION::STATIC);
            Polyhedron::motion = Polyhedron::MOTION::STATIC;
            camera.set_position(CAMERA_POS);
        }
        if (glfwGetKey(glfWwindow, GLFW_KEY_K)) {
            Camera::set_state(Camera::MOTION::ROTATION);
            cube.set_position(0, 0);
            camera.set_position(CAMERA_POS);
            camera.update_state(cube.position);
        }
        if (glfwGetKey(glfWwindow, GLFW_KEY_L)) {
            if (Polyhedron::motion == Polyhedron::MOTION::STATIC) {
                Polyhedron::set_state(Polyhedron::MOTION::ROTATION);
            }
        }
        if (glfwGetKey(glfWwindow, GLFW_KEY_SEMICOLON)) {
            if (Polyhedron::motion == Polyhedron::MOTION::ROTATION) {
                Polyhedron::set_state(Polyhedron::MOTION::STATIC);
            }
        }
        if (glfwGetKey(glfWwindow, GLFW_KEY_U)) {
            Camera::set_state(Camera::MOTION::PREDEFINED);
            cube.set_position(0, 0);
            camera.set_position(POS[0]);
        }
        if (glfwGetKey(glfWwindow, GLFW_KEY_I)) {
            Camera::set_state(Camera::MOTION::PREDEFINED);
            cube.set_position(0, 0);
            camera.set_position(POS[1]);
        }
        if (glfwGetKey(glfWwindow, GLFW_KEY_O)) {
            Camera::set_state(Camera::MOTION::PREDEFINED);
            cube.set_position(0, 0);
            camera.set_position(POS[2]);
        }
    }
    {
        int left = glfwGetKey(glfWwindow, GLFW_KEY_LEFT);
        int right = glfwGetKey(glfWwindow, GLFW_KEY_RIGHT);
        int up = glfwGetKey(glfWwindow, GLFW_KEY_UP);
        int down = glfwGetKey(glfWwindow, GLFW_KEY_DOWN);
        int in = glfwGetKey(glfWwindow, GLFW_KEY_PAGE_UP);
        int out = glfwGetKey(glfWwindow, GLFW_KEY_PAGE_DOWN);

        if (CAMERA_STATIC and ANY_MOVEMENT) {
            if (left) camera.pos.x -= camera.speed.x;
            if (up) camera.pos.y += camera.speed.y;
            if (down) camera.pos.y -= camera.speed.y;
            if (right) camera.pos.x += camera.speed.x;
            if (in) camera.pos.z += camera.speed.z;
            if (out) camera.pos.z -= camera.speed.z;
        } else if (CAMERA_MOVING and ANY_MOVEMENT) {
            if (left) {
                camera.pos.x -= camera.speed.x;
            }
            if (up) {
                camera.pos.y += camera.speed.y;
            }
            if (down) {
                camera.pos.y -= camera.speed.y;
            }
            if (right) {
                camera.pos.x += camera.speed.x;
            }
            if (in) {
                camera.pos.z += camera.speed.z;
            }
            if (out) {
                camera.pos.z -= camera.speed.z;
            }
            camera.update_state(cube.position);
        }
    }
    {
        int left = glfwGetKey(glfWwindow, GLFW_KEY_A);
        int right = glfwGetKey(glfWwindow, GLFW_KEY_D);
        int up = glfwGetKey(glfWwindow, GLFW_KEY_W);
        int down = glfwGetKey(glfWwindow, GLFW_KEY_S);
        int in = glfwGetKey(glfWwindow, GLFW_KEY_Q);
        int out = glfwGetKey(glfWwindow, GLFW_KEY_E);

        if (ANY_MOVEMENT) {
            if (left) {
                cube.position.x -= cube.speed;
            }
            if (up) {
                cube.position.y += cube.speed;
            }
            if (down) {
                cube.position.y -= cube.speed;
            }
            if (right) {
                cube.position.x += cube.speed;
            }
            if (in) {
                cube.position.z -= cube.speed;
            }
            if (out) {
                cube.position.z += cube.speed;
            }
            if (CAMERA_MOVING) {
                camera.update_state(cube.position);
            }
        }
    }
}

void tick_elements() {
    cube.tick();
}

/* Initialize the OpenGL rendering properties */
/* Add all the models to be created here */
void initGL(GLFWwindow *glfWwindow, int width, int height) {
    /* Objects should be created before any other gl function and shaders */
    // Create the models

    cube = Polyhedron(0, 0);
    camera = Camera(CAMERA_POS, UP, glm::vec3(0.1f, 0.1f, 0.1f));
    axes = Axes(0, 0);
    camera.update_state(cube.position);

    // Create and compile our GLSL program from the shaders
    programID = LoadShaders("../src/shaders/shader.vert", "../src/shaders/shader.frag");
    // Get a handle for our "MVP" uniform
    Matrices.MatrixID = glGetUniformLocation(programID, "MVP");


    reshapeWindow(glfWwindow, width, height);

    // Background color of the scene
    glClearColor(COLOR_BACKGROUND.r / 256.0, COLOR_BACKGROUND.g / 256.0, COLOR_BACKGROUND.b / 256.0,
                 0.0f); // R, G, B, A
    glClearDepth(1.0f);

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    cout << "VENDOR: " << glGetString(GL_VENDOR) << endl;
    cout << "RENDERER: " << glGetString(GL_RENDERER) << endl;
    cout << "VERSION: " << glGetString(GL_VERSION) << endl;
    cout << "GLSL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
}


int main(int argc, char **argv) {
    srand(time(0));
    int width = 600;
    int height = 600;

    window = initGLFW(width, height);

    initGL(window, width, height);

    /* Draw in loop */
    while (!glfwWindowShouldClose(window)) {
        // Process timers

        if (t60.processTick()) {
            // 60 fps
            // OpenGL Draw commands
            draw();
            // Swap Frame Buffer in double buffering
            glfwSwapBuffers(window);

            tick_elements();
            tick_input(window);
        }

        // Poll for Keyboard and mouse events
        glfwPollEvents();
    }

    quit(window);
}

bool detect_collision(bounding_box_t a, bounding_box_t b) {
    return (abs(a.x - b.x) * 2 < (a.width + b.width)) && (abs(a.y - b.y) * 2 < (a.height + b.height));
}

void reset_screen() {
    float top = screen_center_y + 4 / screen_zoom;
    float bottom = screen_center_y - 4 / screen_zoom;
    float left = screen_center_x - 4 / screen_zoom;
    float right = screen_center_x + 4 / screen_zoom;
    Matrices.projection = glm::ortho(left, right, bottom, top, 0.1f, 500.0f);
}
