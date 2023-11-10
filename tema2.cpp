#include "lab_m1/Tema2/tema2.h"
#include "lab_m1/Tema2/transform3D.h"
#include "components/transform.h"


#include <vector>
#include <string>
#include <iostream>

using namespace std;
using namespace m1;

#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */

int timp = 0;
bool gameOver = false;
float tx = 0;
float tz = 0;
int damage = 0;
bool destroyed = false;

Tema2::Tema2()
{
}


Tema2::~Tema2()
{
}


void Tema2::Init()
{
    renderCameraTarget = false;

    // Initialize tx, ty and tz (the translation steps)
   

    xProiectil = 0; 
    zProiectil = 0;
    lovit = false;
    dist = 4;//pt proiectil

    enX = 0;
    enZ = 0;

    enemyPos = 0;

    playerFirst = false;
    maiJos = 0.3;
    chooser = 0;

    camera = new implemented::Camera();
    camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

    {
        Mesh* mesh = new Mesh("box");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "box.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }
     {
        Mesh* mesh = new Mesh("plane");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "plane50.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }


    {
        Mesh* mesh = new Mesh("sphere");
        mesh->LoadMesh(PATH_JOIN(window->props.selfDir, RESOURCE_PATH::MODELS, "primitives"), "sphere.obj");
        meshes[mesh->GetMeshID()] = mesh;
    }


    projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);

    // shadere lab7
    {
        Shader *shader = new Shader("temaShader");
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "VertexShader.glsl"), GL_VERTEX_SHADER);
        shader->AddShader(PATH_JOIN(window->props.selfDir, SOURCE_PATH::M1, "Tema2", "shaders", "FragmentShader.glsl"), GL_FRAGMENT_SHADER);
        shader->CreateAndLink();
        shaders[shader->GetName()] = shader;
    }

     {
        lightpos = glm::vec3(1, 2, 1);
        materialShininess = 1;
        mkd = 50;
        mks = 50;
        color1 = glm::vec3(1, 1, 0);
        color2 = glm::vec3(1, 0, 0);
     
    }
}


void Tema2::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0.3f, 1, 1);//sky colour
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x * 2, resolution.y * 2);
}


void Tema2::Update(float deltaTimeSeconds)
{
    
    glm::mat4 modelMatrix;
    ////////////////////////////////////////////////////////////////////////////////////////PENTRU MAP 
    DrawMap(modelMatrix, chooser);

    ////////////////////////////////////////////////////////////////////////////////////////PENTRU INAMIC

    // algoritm pt mutarea inamicilor
    if (!enemyPos) {
        enZ = enZ -  deltaTimeSeconds;
        enX = enX - deltaTimeSeconds;
    } 
    if (enemyPos) {
        enZ = enZ +  deltaTimeSeconds;
        enX = enX  +  deltaTimeSeconds;
    }
    if (abs(enZ) > 1.25) {
        if (enemyPos == 0) enemyPos = 1;
        else enemyPos = 0;
    }

    DrawEnemy(modelMatrix);

    ////////////////////////////////////////////////////////////////////////////////////////PENTRU PLAYER && GLONT

    // pe BACKSPACE(in dreapta la +) rpentru 3rd person
    if (window->KeyHold(GLFW_KEY_BACKSPACE) && playerFirst == true) {
        playerFirst = false;
        camera->TranslateForward(-2.1);
    }

    // pentru first person
    if (window->KeyHold(GLFW_KEY_LEFT_CONTROL) && playerFirst == false) {
        playerFirst = true;
        camera->TranslateForward(2.1);
    } 

    ////////////////////////////////////////
    // distanta si viteza cu care va trage glontul

    if (lovit == true) {
       zProiectil = zProiectil - 8 * deltaTimeSeconds;
    }
    else if (xProiectil == 0 && lovit == false && window->KeyHold(GLFW_KEY_SPACE)) {
       lovit = true;
        
    }

    //
    if ((xProiectil <  -dist) || (xProiectil > dist) || (zProiectil < -dist) || (zProiectil > dist)) {    
        xProiectil = 0;
        zProiectil = 0;
        lovit = false;
    }
    
    DrawPlayer(modelMatrix, playerFirst);
         
}

void Tema2::DrawMap(glm::mat4 modelMatrix, int chooser){


    {
      modelMatrix = glm::mat4(1);
      modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0, 0));
      modelMatrix = glm::scale(modelMatrix, glm::vec3(1));
      RenderMesh(meshes["plane"], shaders["VertexNormal"], modelMatrix);
    }


    // //OBSTACOLE + 3 pe oz
    // if(chooser == 1){
          for(int i = -10; i<=10;i++){//orizontala
            for(float j = 0.5; j <= 1.5; j += 0.5){//inaltime

                for(int k = 1; k <= 10 ; k++){

                   modelMatrix = glm::mat4(1);
                    modelMatrix = glm::translate(modelMatrix, glm::vec3(-9, j, i + k));
                  RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);

                  modelMatrix = glm::mat4(1);
                  modelMatrix = glm::translate(modelMatrix, glm::vec3(-3, j, i + k));
                  RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);

                  modelMatrix = glm::mat4(1);
                  modelMatrix = glm::translate(modelMatrix, glm::vec3(3, j, i + k));
                  RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);
                  

                }

                  //pereti
                  modelMatrix = glm::mat4(1);
                  modelMatrix = glm::translate(modelMatrix, glm::vec3(i, j, -5));
                  RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);

                  //iesiri
                  if(i != 1 && i != 2){
                    modelMatrix = glm::mat4(1);
                    modelMatrix = glm::translate(modelMatrix, glm::vec3(i, j, 5));
                    RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);

                     modelMatrix = glm::mat4(1);
                    modelMatrix = glm::translate(modelMatrix, glm::vec3(i, j, 15));
                    RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);
                  }

                  if(i != -1 && i != -2){
                    modelMatrix = glm::mat4(1);
                    modelMatrix = glm::translate(modelMatrix, glm::vec3(i, j, 10));
                    RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);

                    modelMatrix = glm::mat4(1);
                    modelMatrix = glm::translate(modelMatrix, glm::vec3(i, j, 20));
                    RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);
                  }


              }
                  
            }
        
    // }//if(chooser == 1)

    ////////////////////////////////////////////////////////////////////////////////////////PENTRU HUD

    if(window->KeyHold(GLFW_KEY_W) || window->KeyHold(GLFW_KEY_A) || window->KeyHold(GLFW_KEY_S) || window->KeyHold(GLFW_KEY_D)){
      timp++;
    }else{
      timp += 2;
    }

    //daca se misca timpul trece mai greu altfel trece mai rapid
    DrawHud(modelMatrix);

}
              

void Tema2::DrawEnemy(glm::mat4 modelMatrix){
    {   
        //INAMIC CARE SE MISCA PE OZ cu enZ calculat in Update

        //coliziune inamic jucator
        // if(abs(enX + 2 * 1.3 + 0.6 - xProiectil + 2 * 1.71  ) < 0.05f && abs(enZ + 2 * 1.3 - zProiectil + 2 * 1.71 ) < 0.05f){
        //   // damage++;
        //   destroyed = true;
        // }

        if(destroyed == false){

          modelMatrix = glm::mat4(1);
          modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(0.6, 0.5, 3.5));
          modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(0, 0, enZ));
          modelMatrix = glm::scale(modelMatrix, glm::vec3(1.1f, 1.4f, 0.4f));
          RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
          
        }

        // destroyed = false;
        //INAMIC CARE SE MISCA PE OX cu enX calculat in Update

         // if(destroyed == false){

        modelMatrix = glm::mat4(1);
        modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(0.5, 0.5, 6.5));
        modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(enX, 0, 0));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(1.1f, 1.4f, 0.4f));
        RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
      // }
        //INAMIC CARE SE MISCA IN CERC
        // modelMatrix = glm::mat4(1);
        // modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(0.6, 0.4, 7.5));
        // modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(enX, 0, enZ));
        // modelMatrix = glm::scale(modelMatrix, glm::vec3(1.1f, 1.4f, 0.4f));
        // RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
    }
}

void Tema2::DrawPlayer(glm:: mat4 modelMatrix, bool playerFirst){//desenez player + glont
     if (playerFirst == false) {
    

        {   
            //picioare
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(tx - 0.25f , 0.125f - maiJos, tx));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 0.25f));
            RenderColouredMesh(meshes["box"], shaders["temaShader"], modelMatrix,glm::vec3(0, 0.5f, 1));
        }

        {
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(tx - 0.25f, 0.375f - maiJos, tz));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 0.25f));
            RenderColouredMesh(meshes["box"], shaders["temaShader"], modelMatrix, glm::vec3(0.3f, 0.3f, 1));

        }

        {   

            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(tx + 0.25f, 0.125f - maiJos, tz));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 0.25f));
            RenderColouredMesh(meshes["box"], shaders["temaShader"], modelMatrix,glm::vec3(0, 0.5f, 1));
        }

        {
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(tx + 0.25, 0.375f - maiJos, tz));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 0.25f));
            RenderColouredMesh(meshes["box"], shaders["temaShader"], modelMatrix, glm::vec3(0.3f, 0.3f, 1));

        }
        //corp


        {
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(tx, 0.85f - maiJos, tz));
            modelMatrix = glm::rotate(modelMatrix, RADIANS(60.0f), glm::vec3(1, 0, 0));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f + 0.25f, 0.65f, 0.3f));
            RenderColouredMesh(meshes["box"], shaders["temaShader"], modelMatrix, glm::vec3(0, 0.5f, 1));
        }

        //cap
        {
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(tx, 0.85f + 0.5f - maiJos, tz));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f, 0.25f, 0.25f));
            RenderColouredMesh(meshes["box"], shaders["temaShader"], modelMatrix, glm::vec3(0.7f, 0, 0.4f));

        }

        //maini
        {   

            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(tx -0.5f , 0.75f - maiJos, tz));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.25f, 0.25f));
            RenderColouredMesh(meshes["box"], shaders["temaShader"], modelMatrix,glm::vec3(0.3f, 0.1f, 1));
        }

        {
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(tx - 0.5f, 1 - maiJos, tz));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.25f, 0.25f));
            RenderColouredMesh(meshes["box"], shaders["temaShader"], modelMatrix, glm::vec3(0.7f, 0, 0.4f));

        }

        {   

            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(tx + 0.5f, 0.75f- maiJos, tz));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.25f, 0.25f));
            RenderColouredMesh(meshes["box"], shaders["temaShader"], modelMatrix,glm::vec3(0.3f, 0.3f, 1));
        }

        {
            modelMatrix = glm::mat4(1);
            modelMatrix = glm::translate(modelMatrix, glm::vec3(tx + 0.5f, 1- maiJos, tz));
            modelMatrix = glm::scale(modelMatrix, glm::vec3(0.15f, 0.25f, 0.25f));
            RenderColouredMesh(meshes["box"], shaders["temaShader"], modelMatrix,glm::vec3(0.8f, 0, 0.4f));

        }
    }else{//playerul trage doar in first person 
        
        modelMatrix = glm::mat4(1);
        modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(0, 0.6f, 1.3f));
        modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(tx + xProiectil,  0, tz + zProiectil));
        modelMatrix = glm::scale(modelMatrix, glm::vec3(0.3f));
        RenderColouredMesh(meshes["sphere"], shaders["temaShader"], modelMatrix, glm::vec3(0, 0, 0));
    }
}

void Tema2::DrawHud(glm:: mat4 modelMatrix){

    //health
    if(damage == 0){

    modelMatrix = glm::mat4(1);
    modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(2.2, 2 ,0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0025f));
    RenderColouredMesh(meshes["plane"], shaders["temaShader"], modelMatrix, color2);  

     modelMatrix = glm::mat4(1);
    modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(2, 2 ,0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0025f));
    RenderColouredMesh(meshes["plane"], shaders["temaShader"], modelMatrix, color2);

     modelMatrix = glm::mat4(1);
    modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(1.8 ,2 ,0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0025f));
    RenderColouredMesh(meshes["plane"], shaders["temaShader"], modelMatrix, color2);

     modelMatrix = glm::mat4(1);
    modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(1.6, 2, 0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0025f));
    RenderColouredMesh(meshes["plane"], shaders["temaShader"], modelMatrix, color2);

     modelMatrix = glm::mat4(1);
    modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(1.4, 2,0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0025f));
    RenderColouredMesh(meshes["plane"], shaders["temaShader"], modelMatrix, color2);

    }else if(damage > 0){}

    //timer game

    //timpul de joc( DACA NU TE MISTI CU JUCATORUL TRECE DE 2 ORI MAI REPEDE)
    if(timp >= 0 && timp < 800){
     modelMatrix = glm::mat4(1);
   
    modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(2.2, 1.8 ,0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0025f));
    RenderColouredMesh(meshes["plane"], shaders["temaShader"], modelMatrix, color1);  

     modelMatrix = glm::mat4(1);
    modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(2, 1.8 ,0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0025f));
    RenderColouredMesh(meshes["plane"], shaders["temaShader"], modelMatrix, color1);

     modelMatrix = glm::mat4(1);
    modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(1.8 ,1.8 ,0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0025f));
    RenderColouredMesh(meshes["plane"], shaders["temaShader"], modelMatrix, color1);
  
     modelMatrix = glm::mat4(1);
    modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(1.6, 1.8, 0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0025f));
    RenderColouredMesh(meshes["plane"], shaders["temaShader"], modelMatrix, color1);
    
     modelMatrix = glm::mat4(1);
    modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(1.4, 1.8,0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0025f));
    RenderColouredMesh(meshes["plane"], shaders["temaShader"], modelMatrix, color1);

     }else if(timp >= 800 && timp < 1600){

      modelMatrix = glm::mat4(1);
    modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(2, 1.8 ,0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0025f));
    RenderColouredMesh(meshes["plane"], shaders["temaShader"], modelMatrix, color1);

     modelMatrix = glm::mat4(1);
    modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(1.8 ,1.8 ,0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0025f));
    RenderColouredMesh(meshes["plane"], shaders["temaShader"], modelMatrix, color1);
  
     modelMatrix = glm::mat4(1);
    modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(1.6, 1.8, 0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0025f));
    RenderColouredMesh(meshes["plane"], shaders["temaShader"], modelMatrix, color1);
    
     modelMatrix = glm::mat4(1);
    modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(1.4, 1.8,0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0025f));
    RenderColouredMesh(meshes["plane"], shaders["temaShader"], modelMatrix, color1);
    }else if(timp >= 1600 && timp < 2400){
       modelMatrix = glm::mat4(1);
    modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(1.8 ,1.8 ,0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0025f));
    RenderColouredMesh(meshes["plane"], shaders["temaShader"], modelMatrix, color1);
  
     modelMatrix = glm::mat4(1);
    modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(1.6, 1.8, 0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0025f));
    RenderColouredMesh(meshes["plane"], shaders["temaShader"], modelMatrix, color1);
    
     modelMatrix = glm::mat4(1);
    modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(1.4, 1.8,0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0025f));
    RenderColouredMesh(meshes["plane"], shaders["temaShader"], modelMatrix, color1);
  }else if(timp >= 2400 && timp < 3200){

     modelMatrix = glm::mat4(1);
    modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(1.6, 1.8, 0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0025f));
    RenderColouredMesh(meshes["plane"], shaders["temaShader"], modelMatrix, color1);
    
     modelMatrix = glm::mat4(1);
    modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(1.4, 1.8,0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0025f));
    RenderColouredMesh(meshes["plane"], shaders["temaShader"], modelMatrix, color1);
  }else if(timp >= 3200 && timp < 4000){
    modelMatrix = glm::mat4(1);
    modelMatrix = modelMatrix *  glm::translate(modelMatrix, glm::vec3(1.4, 1.8,0));
    modelMatrix = glm::rotate(modelMatrix, RADIANS(90.0f), glm::vec3(1, 0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(0.0025f));
    RenderColouredMesh(meshes["plane"], shaders["temaShader"], modelMatrix, color1);
  }else if(timp >= 4000 && gameOver == false){
    printf("GAME OVER!!!\n");
    gameOver = true;
  }


  }


void Tema2::FrameEnd()
{
    if(timp >=4000){
      glClearColor(0, 0,0, 0);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      // DrawCoordinateSystem(camera->GetViewMatrix(), projectionMatrix);
    }
    
    
}


void Tema2::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
    if (!mesh || !shader || !shader->program)
        return;

    // Render an object using the specified shader and the specified position
    shader->Use();
    glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
    glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
    glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

    mesh->Render();
    
}

void Tema2::RenderColouredMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix,const glm::vec3 &color)
{
    if (!mesh || !shader || !shader->GetProgramID())
        return;

    // Render an object using the specified shader and the specified position
    glUseProgram(shader->program);

    // Set shader uniforms for light & material properties
    // TODO(student): Set light position 
    GLint locLightPos = glGetUniformLocation(shader->program, "light_position");
    glUniform3fv(locLightPos, 1, glm::value_ptr(lightpos));

    GLint MaterialL = glGetUniformLocation(shader->program, "material_shininess");
    glUniform1i(MaterialL, materialShininess);

    GLint MaterialLKd = glGetUniformLocation(shader->program, "material_kd");  // diffuse light
    glUniform1f(MaterialLKd, mkd);

    GLint MaterialLKs = glGetUniformLocation(shader->program, "material_ks");  // specular light
    glUniform1f(MaterialLKs, mks);

    GLint ObjectL = glGetUniformLocation(shader->program, "object_color");
    glUniform3fv(ObjectL, 1, glm::value_ptr(color));

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


void Tema2::OnInputUpdate(float deltaTime, int mods)
{   
      
    float cameraSpeed = 2.0;

    if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)){
    

      if (window->KeyHold(GLFW_KEY_W)) {

        //coliziune player-inamic
        if(abs(enX - tx ) < 0.0075 && abs(enZ  - tz ) < 0.0075){
          damage++;
          destroyed = true;
        }
        // }else{
          camera->MoveForward(deltaTime * cameraSpeed);
        // }
      }
        
      if (window->KeyHold(GLFW_KEY_A)) {
        // if(abs(enX - tx) < 0.2f && abs(enZ - tz) < 0.2f){
          // damage++;
        // }else{
        camera->TranslateRight(-deltaTime * cameraSpeed);
        // }
      }
        
      if (window->KeyHold(GLFW_KEY_S)){
        // if(abs(enX - tx) < 0.2f && abs(enZ - tz) < 0.2f){
          // damage++;
        // }else{
        camera->MoveForward(-deltaTime * cameraSpeed);
        // }
      } 

      if (window->KeyHold(GLFW_KEY_D)) {
        // if(abs(enX - tx) < 0.2f && abs(enZ - tz) < 0.2f){
          // damage++;
        // }else{
        camera->TranslateRight(deltaTime * cameraSpeed);
        // }
      }
        
   }

   //viteza de rotatie
   float cameraRotateSpeed = 1.0;

    //rotatia la tanga
    if (window->KeyHold(GLFW_KEY_Q)) {

        if (renderCameraTarget == false) {
          //calculez unghiul in functie de viteza camerei si delta time
          angle = angle + deltaTime * cameraRotateSpeed ;
          camera->RotateFirstPerson_OY(deltaTime * cameraRotateSpeed );
            
        } else {
            camera->RotateThirdPerson_OY(deltaTime * cameraRotateSpeed );
        }
    }

     if (window->KeyHold(GLFW_KEY_E)) {
        if (renderCameraTarget == false) {
          angle = angle - deltaTime * cameraRotateSpeed;
          camera->RotateFirstPerson_OY(deltaTime * (-cameraRotateSpeed) );
          
        } else {
          camera->RotateThirdPerson_OY(deltaTime * (-cameraRotateSpeed));
        }
    }

}


void Tema2::OnKeyPress(int key, int mods)
{
  
}


void Tema2::OnKeyRelease(int key, int mods)
{
}


void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
   
}


void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods){}


void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods){}


void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY){}


void Tema2::OnWindowResize(int width, int height){}
