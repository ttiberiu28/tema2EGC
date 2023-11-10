#pragma once

#include "components/simple_scene.h"
#include "lab_m1/lab5/lab_camera.h"

namespace m1
{
    class Tema2 : public gfxc::SimpleScene
    {
     public:
        Tema2();
        ~Tema2();

        void Init() override;

     private:
        void FrameStart() override;
        void Update(float deltaTimeSeconds) override;
        void FrameEnd() override;

        void RenderMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix) override;
        void RenderColouredMesh(Mesh *mesh, Shader *shader, const glm::mat4 &modelMatrix, const glm::vec3 &color);

        void OnInputUpdate(float deltaTime, int mods) override;
        void OnKeyPress(int key, int mods) override;
        void OnKeyRelease(int key, int mods) override;
        void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
        void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
        void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
        void OnWindowResize(int width, int height) override;
        void DrawMap(glm::mat4 visMatrix, int chooser);
        void DrawEnemy(glm::mat4 visMatrix);
        void DrawPlayer(glm::mat4 visMatrix, bool playerFirst);
        void DrawHud(glm::mat4 visMatrix);

     protected:
        implemented::Camera *camera;
        glm::mat4 projectionMatrix;
        bool renderCameraTarget;
        int chooser;
        // TODO(student): If you need any other class variables, define them here.
        bool projPerspective;
        
        bool playerFirst;
        GLfloat left;
        GLfloat right;
        GLfloat bottom;
        GLfloat top;
        GLfloat fov;
        GLfloat maiJos;


        GLfloat ZNear;
        GLfloat ZFar;
        
        float enX, enZ;
        float xProiectil, zProiectil;
        float angle = 0;
        int dist , enemyPos = 0, contEn = 0, maxCam = 0;
        bool lovit;
        float distCam = 0, hig = 0.17f;

        // lab 7
        glm::vec3 lightpos, color1, color2, plnColor, obColor, headColor, bodyColor, armColor,legs,arms,head,body;
        unsigned int materialShininess;
        float mkd;
        float mks;
    };
}   // namespace m1
