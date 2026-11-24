#include "lab_m1/lab2/lab2.h"

#include <vector>
#include <iostream>

#include "core/engine.h"
#include "utils/gl_utils.h"

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab2::Lab2()
{
}


Lab2::~Lab2()
{
}


void Lab2::Init()
{
    cullFace = GL_BACK;
    polygonMode = GL_FILL;

    // Load a mesh from file into GPU memory
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Create a mesh box using custom data
    {
        // 8 varfuri: 0-3 fata din fata (z = 1), 4-7 fata din spate (z = -1)
        // VertexFormat(pozitie, culoare)
        vector<VertexFormat> vertices
        {
            VertexFormat(glm::vec3(-1, -1,  1), glm::vec3(0, 1, 1), glm::vec3(0.2, 0.8, 0.6)),
            VertexFormat(glm::vec3( 1, -1,  1), glm::vec3(1, 0, 1)),
            VertexFormat(glm::vec3(-1,  1,  1), glm::vec3(1, 1, 0)),
            VertexFormat(glm::vec3( 1,  1,  1), glm::vec3(1, 0, 0)),
            VertexFormat(glm::vec3(-1, -1, -1), glm::vec3(0, 1, 0)),
            VertexFormat(glm::vec3( 1, -1, -1), glm::vec3(0, 0, 1)),
            VertexFormat(glm::vec3(-1,  1, -1), glm::vec3(1, 1, 1)),
            VertexFormat(glm::vec3( 1,  1, -1), glm::vec3(0.5, 0.5, 0.5)),
        };

        // 6 fete x 2 triunghiuri = 12 triunghiuri
        // fiecare triunghi e dat in sens trigonometric (CCW) privit din afara cubului
        vector<unsigned int> indices =
        {
            0, 1, 2,    // indices for first triangle
            1, 3, 2,    // indices for second triangle
            // spate (z = -1)
            5, 4, 7,
            4, 6, 7,
            // dreapta (x = 1)
            1, 5, 3,
            5, 7, 3,
            // stanga (x = -1)
            4, 0, 6,
            0, 2, 6,
            // sus (y = 1)
            2, 3, 6,
            3, 7, 6,
            // jos (y = -1)
            4, 5, 0,
            5, 1, 0,
        };

        meshes["cube_A"] = new Mesh("generated cube 1");
        meshes["cube_A"]->InitFromData(vertices, indices);

        // Actually create the mesh from the data
        CreateMesh("cube_B", vertices, indices);
    }

    // tetraedru cu 4 varfuri: baza pe y = 0 si un varf sus
    {
        vector<VertexFormat> vertices
        {
            VertexFormat(glm::vec3(-1, 0,  1), glm::vec3(1, 0, 0)),
            VertexFormat(glm::vec3( 1, 0,  1), glm::vec3(0, 1, 0)),
            VertexFormat(glm::vec3( 0, 0, -1), glm::vec3(0, 0, 1)),
            VertexFormat(glm::vec3( 0, 1.5f, 0.33f), glm::vec3(1, 1, 0)),
        };

        // 4 fete, toate CCW privite din afara
        vector<unsigned int> indices =
        {
            0, 2, 1,    // baza (se vede de jos)
            0, 1, 3,    // fata
            1, 2, 3,    // dreapta
            2, 0, 3,    // stanga
        };

        CreateMesh("tetraedru", vertices, indices);
    }

    // patrat din 2 triunghiuri cu orientari opuse
    // triunghiul rosu e CCW privit din fata (+z), cel albastru e CW
    // => cu culling, din fata se vede doar rosul, din spate doar albastrul
    // am pus 6 varfuri (nu 4) ca fiecare triunghi sa aiba culoarea lui
    {
        vector<VertexFormat> vertices
        {
            VertexFormat(glm::vec3(-1, -1, 0), glm::vec3(1, 0, 0)),
            VertexFormat(glm::vec3( 1, -1, 0), glm::vec3(1, 0, 0)),
            VertexFormat(glm::vec3(-1,  1, 0), glm::vec3(1, 0, 0)),
            VertexFormat(glm::vec3( 1, -1, 0), glm::vec3(0, 0, 1)),
            VertexFormat(glm::vec3(-1,  1, 0), glm::vec3(0, 0, 1)),
            VertexFormat(glm::vec3( 1,  1, 0), glm::vec3(0, 0, 1)),
        };

        vector<unsigned int> indices =
        {
            0, 1, 2,    // rosu, CCW din fata
            3, 4, 5,    // albastru, CW din fata (deci CCW din spate)
        };

        CreateMesh("patrat", vertices, indices);
    }
}


void Lab2::CreateMesh(const char *name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned int> &indices)
{
    // VAO-ul tine minte toate legaturile de mai jos (VBO, IBO, atribute)
    unsigned int VAO = 0;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // VBO = varfurile (pozitie, normala, coord textura, culoare)
    unsigned int VBO = 0;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // copiez varfurile din RAM pe GPU
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

    // IBO = indicii (cum leg varfurile in triunghiuri)
    unsigned int IBO = 0;
    glGenBuffers(1, &IBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices[0]) * indices.size(), &indices[0], GL_STATIC_DRAW);

    // ========================================================================
    // This section demonstrates how the GPU vertex shader program
    // receives data. It will be learned later, when GLSL shaders will be
    // introduced. For the moment, just think that each property value from
    // our vertex format needs to be sent to a certain channel, in order to
    // know how to receive it in the GLSL vertex shader.

    // Set vertex position attribute
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), 0);

    // Set vertex normal attribute
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(sizeof(glm::vec3)));

    // Set texture coordinate attribute
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3)));

    // Set vertex color attribute
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexFormat), (void*)(2 * sizeof(glm::vec3) + sizeof(glm::vec2)));
    // ========================================================================

    // dezleg VAO-ul ca sa nu se mai lege alte comenzi de el din greseala
    glBindVertexArray(0);

    // Check for OpenGL errors
    if (GetOpenGLError() == GL_INVALID_OPERATION)
    {
        cout << "\t[NOTE] : For students : DON'T PANIC! This error should go away when completing the tasks." << std::endl;
        cout << "\t[NOTE] : For developers : This happens because OpenGL core spec >=3.1 forbids null VAOs." << std::endl;
    }

    // Mesh information is saved into a Mesh object
    meshes[name] = new Mesh(name);
    meshes[name]->InitFromBuffer(VAO, static_cast<unsigned int>(indices.size()));
}


void Lab2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();

    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Lab2::Update(float deltaTimeSeconds)
{
    glLineWidth(3);
    glPointSize(5);
    glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

    // pornesc face culling si aleg ce fete arunc (GL_BACK sau GL_FRONT, schimb cu F2)
    glEnable(GL_CULL_FACE);
    glCullFace(cullFace);

    // Render an object using face normals for color
    RenderMesh(meshes["box"], shaders["VertexNormal"], glm::vec3(0, 0.5f, -1.5f), glm::vec3(0.75f));

    // Render an object using colors from vertex
    RenderMesh(meshes["cube_A"], shaders["VertexColor"], glm::vec3(-1.5f, 0.5f, 0), glm::vec3(0.25f));

    // cubul facut de mine cu CreateMesh, in dreapta (cube_A e in stanga)
    RenderMesh(meshes["cube_B"], shaders["VertexColor"], glm::vec3(1.5f, 0.5f, 0), glm::vec3(0.25f));

    RenderMesh(meshes["tetraedru"], shaders["VertexColor"], glm::vec3(-2, 0, -2), glm::vec3(0.5f));

    RenderMesh(meshes["patrat"], shaders["VertexColor"], glm::vec3(2, 0.5f, -2), glm::vec3(0.5f));

    // opresc culling-ul ca sa nu afecteze ce se deseneaza dupa (axele din FrameEnd)
    glDisable(GL_CULL_FACE);

}


void Lab2::FrameEnd()
{
    DrawCoordinateSystem();
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab2::OnInputUpdate(float deltaTime, int mods)
{
}


void Lab2::OnKeyPress(int key, int mods)
{
    // F2 (sau V, pe laptop F2 e tasta de volum) = schimb intre GL_BACK si GL_FRONT
    // aici doar salvez starea, o aplic in Update cu glCullFace
    if (key == GLFW_KEY_F2 || key == GLFW_KEY_V)
    {
        if (cullFace == GL_BACK)
            cullFace = GL_FRONT;
        else
            cullFace = GL_BACK;
        cout << "culling: " << (cullFace == GL_BACK ? "GL_BACK" : "GL_FRONT") << endl;
    }

    if (key == GLFW_KEY_SPACE)
    {
        switch (polygonMode)
        {
        case GL_POINT:
            polygonMode = GL_FILL;
            break;
        case GL_LINE:
            polygonMode = GL_POINT;
            break;
        default:
            polygonMode = GL_LINE;
            break;
        }
    }
}


void Lab2::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Lab2::OnWindowResize(int width, int height)
{
}
