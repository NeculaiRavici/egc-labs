#include "lab_m1/lab8/lab8.h"

#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;


/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


Lab8::Lab8()
{
}


Lab8::~Lab8()
{
}


void Lab8::Init()
{
    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    {
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }

    // Create a shader program for drawing face polygon with the color of the normal
    {
        Shader *shader = new Shader("LabShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "lab8", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "lab8", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

    // Light & material properties
    {
        lightPosition = glm::vec3(0, 1, 1);
        lightDirection = glm::vec3(0, -1, 0);
        materialShininess = 30;
        materialKd = 0.5;
        materialKs = 0.5;

        isSpotlight = false;
        cutOff = 30.0f;
        useBlinn = true;

        light2Color = glm::vec3(1.0f, 0.6f, 0.2f);
        light2Position = glm::vec3(-2.5f, 2.0f, 1.5f);
    }
}


void Lab8::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void Lab8::Update(float deltaTimeSeconds)
{
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1, 0));
        RenderSimpleMesh(meshes["sphere"], shaders["LabShader"], modelMatrix, glm::vec3(0.9f, 0.3f, 0.3f));

    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(2, 0.5f, 0));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(60.0f), glm::vec3(1, 0, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.4f, 0.9f, 0.4f));

    }

    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(-2, 0.5f, 0));
        modelMatrix = glm::rotate(modelMatrix, RADIANS(60.0f), glm::vec3(1, 1, 0));
        RenderSimpleMesh(meshes["box"], shaders["LabShader"], modelMatrix, glm::vec3(0.25, 0.75, 0.75));
    }

    // Render ground
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.01f, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
        RenderSimpleMesh(meshes["plane"], shaders["LabShader"], modelMatrix, glm::vec3(0.8f));

    }

    // Render the point light in the scene
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, lightPosition);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
        RenderMesh(meshes["sphere"], shaders["Simple"], modelMatrix);
    }

    // Render the second light
    {
        glm::mat4 modelMatrix = glm::mat4(1);
        modelMatrix = glm::translate(modelMatrix, light2Position);
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
        RenderMesh(meshes["sphere"], shaders["Simple"], modelMatrix);
    }
}


void Lab8::FrameEnd()
{
    DrawCoordinateSystem();
}


void Lab8::RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 & modelMatrix, const glm::vec3 &color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Set shader uniforms for light properties
    glm::vec3 lightPositions[2] = { lightPosition, light2Position };
    glm::vec3 lightColors[2] = { glm::vec3(1), light2Color };

    int light_position = glGetUniformLocation(shader->program, "light_position");
    glUniform3fv(light_position, 2, glm::value_ptr(lightPositions[0]));

    int light_color = glGetUniformLocation(shader->program, "light_color");
    glUniform3fv(light_color, 2, glm::value_ptr(lightColors[0]));

    int light_direction = glGetUniformLocation(shader->program, "light_direction");
    glUniform3f(light_direction, lightDirection.x, lightDirection.y, lightDirection.z);

    // Set eye position (camera position) uniform
    glm::vec3 eyePosition = GetSceneCamera()->m_transform->GetWorldPosition();
    int eye_position = glGetUniformLocation(shader->program, "eye_position");
    glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

    // Set material property uniforms (shininess, kd, ks, object color) 
    int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(material_shininess, materialShininess);

    int material_kd = glGetUniformLocation(shader->program, "material_kd");
    glUniform1f(material_kd, materialKd);

    int material_ks = glGetUniformLocation(shader->program, "material_ks");
    glUniform1f(material_ks, materialKs);

    int object_color = glGetUniformLocation(shader->program, "object_color");
    glUniform3f(object_color, color.r, color.g, color.b);

    int is_spotlight = glGetUniformLocation(shader->program, "is_spotlight");
    glUniform1i(is_spotlight, isSpotlight ? 1 : 0);

    int cut_off = glGetUniformLocation(shader->program, "cut_off");
    glUniform1f(cut_off, cutOff);

    // Specular model: Blinn (half vector) or Phong (reflect)
    glUniform1i(glGetUniformLocation(shader->program, "use_blinn"), useBlinn ? 1 : 0);

    // Bind model matrix
    GLint loc_model_matrix = glGetUniformLocation(shader->program, "Model");
    glUniformMatrix4fv(loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    // Bind view matrix
    glm::mat4 viewMatrix = GetSceneCamera()->GetViewMatrix();
    int loc_view_matrix = glGetUniformLocation(shader->program, "View");
    glUniformMatrix4fv(loc_view_matrix, 1, GL_FALSE, glm::value_ptr(viewMatrix));

    // Bind projection matrix
    glm::mat4 projectionMatrix = GetSceneCamera()->GetProjectionMatrix();
    int loc_projection_matrix = glGetUniformLocation(shader->program, "Projection");
    glUniformMatrix4fv(loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

    // Draw the object
    glBindVertexArray(mesh->GetBuffers()->m_VAO);
    glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_INT, 0);
}


/*
 *  These are callback functions. To find more about callbacks and
 *  how they behave, see `input_controller.h`.
 */


void Lab8::OnInputUpdate(float deltaTime, int mods)
{
    float speed = 2;

    if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
    {
        glm::vec3 up = glm::vec3(0, 1, 0);
        glm::vec3 right = GetSceneCamera()->m_transform->GetLocalOXVector();
        glm::vec3 forward = GetSceneCamera()->m_transform->GetLocalOZVector();
        forward = glm::normalize(glm::vec3(forward.x, 0, forward.z));

        // Control light position using on W, A, S, D, E, Q
        if (window->KeyHold(GLFW_KEY_W)) lightPosition -= forward * deltaTime * speed;
        if (window->KeyHold(GLFW_KEY_A)) lightPosition -= right * deltaTime * speed;
        if (window->KeyHold(GLFW_KEY_S)) lightPosition += forward * deltaTime * speed;
        if (window->KeyHold(GLFW_KEY_D)) lightPosition += right * deltaTime * speed;
        if (window->KeyHold(GLFW_KEY_E)) lightPosition += up * deltaTime * speed;
        if (window->KeyHold(GLFW_KEY_Q)) lightPosition -= up * deltaTime * speed;

        // Bonus: rotate the spot direction with the arrow keys
        float rotSpeed = RADIANS(60.0f) * deltaTime;
        glm::vec3 worldUp = glm::vec3(0, 1, 0);
        glm::vec3 spotRight = glm::cross(lightDirection, worldUp);
        if (glm::length(spotRight) < 1e-3f) spotRight = right;   // pointing straight up/down
        spotRight = glm::normalize(spotRight);

        if (window->KeyHold(GLFW_KEY_UP))    lightDirection = glm::vec3(glm::rotate(glm::mat4(1),  rotSpeed, spotRight) * glm::vec4(lightDirection, 0));
        if (window->KeyHold(GLFW_KEY_DOWN))  lightDirection = glm::vec3(glm::rotate(glm::mat4(1), -rotSpeed, spotRight) * glm::vec4(lightDirection, 0));
        if (window->KeyHold(GLFW_KEY_LEFT))  lightDirection = glm::vec3(glm::rotate(glm::mat4(1),  rotSpeed, worldUp) * glm::vec4(lightDirection, 0));
        if (window->KeyHold(GLFW_KEY_RIGHT)) lightDirection = glm::vec3(glm::rotate(glm::mat4(1), -rotSpeed, worldUp) * glm::vec4(lightDirection, 0));
        lightDirection = glm::normalize(lightDirection);

        // Bonus: widen / narrow the cone with + / -
        float angleSpeed = 20.0f * deltaTime;
        if (window->KeyHold(GLFW_KEY_EQUAL) || window->KeyHold(GLFW_KEY_KP_ADD))      cutOff += angleSpeed;
        if (window->KeyHold(GLFW_KEY_MINUS) || window->KeyHold(GLFW_KEY_KP_SUBTRACT)) cutOff -= angleSpeed;
        cutOff = glm::clamp(cutOff, 1.0f, 89.0f);
    }
}


void Lab8::OnKeyPress(int key, int mods)
{
    // Add key press event

    // R reloads the shaders (alternative to F5, which is a brightness key on this laptop)
    if (key == GLFW_KEY_R)
    {
        ReloadShaders();
        cout << "Shaders reloaded" << endl;
    }

    // B toggles the specular model: Blinn (half vector) <-> Phong (reflect)
    if (key == GLFW_KEY_B)
    {
        useBlinn = !useBlinn;
        cout << "Specular: " << (useBlinn ? "Blinn (N.H)" : "Phong (V.R)") << endl;
    }

    // F toggles between point light and spotlight
    if (key == GLFW_KEY_F)
    {
        isSpotlight = !isSpotlight;
        cout << (isSpotlight ? "Spotlight ON" : "Point light ON") << endl;
    }
}


void Lab8::OnKeyRelease(int key, int mods)
{
    // Add key release event
}


void Lab8::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    // Add mouse move event
}


void Lab8::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button press event
}


void Lab8::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    // Add mouse button release event
}


void Lab8::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void Lab8::OnWindowResize(int width, int height)
{
}
