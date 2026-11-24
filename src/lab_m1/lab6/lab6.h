#pragma once

#include <vector>

#include "components/simple_scene.h"


namespace m1
{
    class Lab6 : public gfxc::SimpleScene
    {
     public:
        Lab6();
        ~Lab6();

        void Init() override;

        Mesh *CreateMesh(const char *name, const std::vector<VertexFormat> &vertices, const std::vector<unsigned int> &indices);

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderSimpleMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;

        // Cerinta 5: daca e true, CreateMesh trimite normala pe pipe-ul 3 si culoarea pe pipe-ul 1
        bool swapPipes = false;
        // Tasta P: deseneaza cubul cu pipe-urile interschimbate
        bool showSwapped = false;
        // Tasta B: bonus (pozitie + culoare variate in timp)
        bool bonus = false;
    };
}   // namespace m1
