#include "lab_m1/lab1/lab1.h"

#include <vector>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab1::Lab1()
{
    r = 0; g = 0; b = 0;
    idx_culoare = 0;

    pos = glm::vec3(0, 0.5f, 2);

    meshe[0] = "box";
    meshe[1] = "sphere";
    meshe[2] = "teapot";
    idx_mesh = 0;

    vy = 0;
    in_aer = false;

}


Lab1::~Lab1()
{
}


void Lab1::Init()
{
    // Load a mesh from file into GPU memory. We only need to do it once,
    // no matter how many times we want to draw this mesh.
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // mai incarc niste mesh-uri din assets/models/primitives
    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
    {
        Mesh* mesh = new Mesh("teapot");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "teapot.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

}


void Lab1::FrameStart()
{
}


void Lab1::Update(float deltaTimeSeconds)
{
    glm::ivec2 resolution = window->props.resolution;

    // Sets the clear color for the color buffer

    // TODO(student): Generalize the arguments of `glClearColor`.
    // You can, for example, declare three variables in the class header,
    // that will store the color components (red, green, blue).
    glClearColor(r, g, b, 1);

    // Clears the color buffer (using the previously set color) and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);

    // Render the object
    RenderMesh(meshes["box"], glm::vec3(1, 0.5f, 0), glm::vec3(0.5f));

    // Render the object again but with different properties
    RenderMesh(meshes["box"], glm::vec3(-1, 0.5f, 0));

    // inca un obiect, sfera mai mare in spate
    RenderMesh(meshes["sphere"], glm::vec3(0, 1, -2), glm::vec3(1.5f));

    // bonus: saritura cu gravitatie
    if (in_aer) {
        vy -= 9.8f * deltaTimeSeconds;
        pos.y += vy * deltaTimeSeconds;
        if (pos.y <= 0.5f) {
            pos.y = 0.5f;
            vy = 0;
            in_aer = false;
        }
    }

    // obiectul controlat de mine (se misca si isi schimba forma)
    RenderMesh(meshes[meshe[idx_mesh]], pos);

}


void Lab1::FrameEnd()
{
    DrawCoordinateSystem();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab1::OnInputUpdate(float deltaTime, int mods)
{
    // Treat continuous update based on input

    // daca tin click dreapta, WASD misca camera, deci nu mut obiectul
    if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
        return;

    float viteza = 2.0f;

    if (window->KeyHold(GLFW_KEY_W)) pos.z -= viteza * deltaTime;
    if (window->KeyHold(GLFW_KEY_S)) pos.z += viteza * deltaTime;
    if (window->KeyHold(GLFW_KEY_A)) pos.x -= viteza * deltaTime;
    if (window->KeyHold(GLFW_KEY_D)) pos.x += viteza * deltaTime;
    if (window->KeyHold(GLFW_KEY_E)) pos.y += viteza * deltaTime;
    if (window->KeyHold(GLFW_KEY_Q)) pos.y -= viteza * deltaTime;

}


void Lab1::OnKeyPress(int key, int mods)
{
    // Add key press event
    if (key == GLFW_KEY_F) {
        // trec prin cateva culori pe rand
        idx_culoare = (idx_culoare + 1) % 4;
        if (idx_culoare == 0) { r = 0;    g = 0;    b = 0; }
        if (idx_culoare == 1) { r = 0.2f; g = 0.1f; b = 0.4f; }
        if (idx_culoare == 2) { r = 0.1f; g = 0.4f; b = 0.3f; }
        if (idx_culoare == 3) { r = 0.6f; g = 0.6f; b = 0.6f; }
        cout << "culoare fundal: " << r << " " << g << " " << b << endl;

    }

    // M = schimb mesh-ul
    if (key == GLFW_KEY_M) {
        idx_mesh = (idx_mesh + 1) % 3;
        cout << "mesh: " << meshe[idx_mesh] << endl;
    }

    // SPACE = sare (bonus)
    if (key == GLFW_KEY_SPACE && !in_aer) {
        vy = 5.0f;
        in_aer = true;
    }

}


void Lab1::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
    // Treat mouse scroll event
}


void Lab1::OnWindowResize(int width, int height)
{
    // Treat window resize event
}
