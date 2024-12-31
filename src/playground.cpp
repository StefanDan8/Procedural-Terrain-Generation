
#define GLFW_INCLUDE_NONE

#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "imgui.h"
#include <cmath>
#include <cstdio>
#include <iostream>
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "AppConfig.hpp"
#include "PerlinOOP.hpp"
#include "Renderer.hpp"
#include "UserInterface.hpp"

#include "Mesh.hpp"

int framebufferWidth, framebufferHeight;
const unsigned WINDOW_WIDTH = 600;
const unsigned WINDOW_HEIGHT = 600;

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
   glViewport(0, 0, width, height);
}

// Vertex vertices[] = {
// Vertex{glm::vec3(-0.5f, 0.0f, 0.5f), glm::vec3(0.3f, 0.70f, 0.44f)},
// Vertex{glm::vec3(-0.5f, 0.0f, -0.5f), glm::vec3(0.83f, 0.70f, 0.44f)},
// Vertex{glm::vec3(0.5f, 0.0f, -0.5f), glm::vec3(0.83f, 0.70f, 0.44f)},
// Vertex{glm::vec3(0.5f, 0.0f, 0.5f), glm::vec3(0.83f, 0.70f, 0.44f)},
// Vertex{glm::vec3(0.0f, 0.8f, 0.0f), glm::vec3(0.92f, 0.86f, 0.76f)}};

// // Indices for vertices order
// GLuint indices[] =
//    {
//       0, 1, 2,
//       0, 2, 3,
//       0, 1, 4,
//       1, 2, 4,
//       2, 3, 4,
//       3, 0, 4};

int main() {
   perlin::AppConfig::initialize(42);
   const unsigned sizeX = 12;
   const unsigned sizeY = 12;
   std::vector<std::pair<unsigned, double>> params4{std::make_pair(12, 1), std::make_pair(6, 3), std::make_pair(4, 3),
                                                    std::make_pair(3, 0.3), std::make_pair(3, 0.2)};

   perlin::PerlinNoise2D noise = perlin::PerlinNoise2D(sizeX, sizeY, params4);
   perlin::matrix result(sizeX, std::vector<double>(sizeY, 0.0));
   noise.fill(result);
   render::writeMatrixToFile(render::normalizeMatrix(result), "myMesh.txt");

   std::cout << "Hi" << std::endl;

   glfwInit();

   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

   GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Group44", NULL, NULL);
   if (window == NULL) {
      std::cout << "Failed to create GLFW window.\n";
      glfwTerminate();
      return -1;
   }
   glfwMakeContextCurrent(window);

   gladLoadGL();

   // Query the actual framebuffer size and set the viewport accordingly

   glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
   glViewport(0, 0, framebufferWidth, framebufferHeight);

   //glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
   // glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
   // glClear(GL_COLOR_BUFFER_BIT);
   // glfwSwapBuffers(window);

   glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

   Shader shaderProgram("default.vert", "default.frag");

   // std::vector<Vertex> verts(vertices, vertices + sizeof(vertices) / sizeof(Vertex));
   // std::vector<GLuint> ind(indices, indices + sizeof(indices) / sizeof(GLuint));
   // Mesh pyramid(verts, ind);

   Mesh myMesh(result);
   //PrintMesh(myMesh);

   glEnable(GL_DEPTH_TEST);

   Camera camera(WINDOW_WIDTH, WINDOW_HEIGHT, glm::vec3(0.0f, 0.0f, 2.0f));

   while (!glfwWindowShouldClose(window)) {
      glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      shaderProgram.Activate();

      camera.Inputs(window);

      camera.updateMatrix(45.0f, 0.1f, 100.0f);
      myMesh.Draw(shaderProgram, camera);

      glfwSwapBuffers(window);
      glfwPollEvents();
   }

   shaderProgram.Delete();

   glfwDestroyWindow(window);
   glfwTerminate();
   return 0;
}

// int main() {
//    perlin::AppConfig::initialize(42);
//    // const unsigned sizeX = 360;
//    // const unsigned sizeY = 360;
//    // std::vector<std::pair<unsigned, double>> params{std::make_pair(256, 1), std::make_pair(128, 0.5), std::make_pair(64, 0.25), std::make_pair(32, 0.125), std::make_pair(16, 0.0625), std::make_pair(8, 0.03)};
//    // std::vector<std::pair<unsigned, double>> params2{std::make_pair(360, 60), std::make_pair(180, 80), std::make_pair(120, 70),
//    //                                                  std::make_pair(90, 125), std::make_pair(45, 40), std::make_pair(12, 30), std::make_pair(8, 15)};
//    // // std::vector<std::pair<unsigned, double>> params3{std::make_pair(360, 60), std::make_pair(180, 120), std::make_pair(120, 100),
//    // //                                                  std::make_pair(90, 60), std::make_pair(45, 30), std::make_pair(12, 15), std::make_pair(8, 6), std::make_pair(3, 2)};
//    // //render::ReLU(result, -10);
//    // std::vector<std::pair<unsigned, double>> params4{std::make_pair(360, 60), std::make_pair(180, 300), std::make_pair(120, 60),
//    //                                                  std::make_pair(90, 40), std::make_pair(45, 20), std::make_pair(12, 10), std::make_pair(8, 5), std::make_pair(3, 2)};

//    // perlin::PerlinNoise2D noise = perlin::PerlinNoise2D(sizeX, sizeY, params4);
//    // perlin::matrix result(sizeX, std::vector<double>(sizeY, 0.0));
//    // noise.fill(result);
//    // //render::writeMatrixToFile(render::normalizeMatrix(result), "oop.txt");

//    // //render::ReLU(result, -10);
//    // perlin::matrix filter(sizeX, std::vector<double>(sizeY, 0.0));
//    // std::vector<std::pair<unsigned, double>> filterParams{std::make_pair(180, 10), std::make_pair(120, 10), std::make_pair(60, 10), std::make_pair(30, 1)};
//    // perlin::PerlinNoise2D noiseFilter(sizeX, sizeY, filterParams);
//    // noiseFilter.fill(filter);

//    // render::Max(result, filter);
//    // render::create_png(render::normalizeMatrix(result), "oop46.png");
//    // render::createPolyMesh(result, 360, 360, "oop46.obj");
//    // // render::writeMatrixToFile(render::normalizeMatrix(result), "stacked.txt");
//    // render::createPolyMesh(result, 4, 4, "stacked.obj");
//    const unsigned sizeX = 12;
//    const unsigned sizeY = 12;
//    std::vector<std::pair<unsigned, double>> params{std::make_pair(256, 1), std::make_pair(128, 0.5), std::make_pair(64, 0.25), std::make_pair(32, 0.125), std::make_pair(16, 0.0625), std::make_pair(8, 0.03)};
//    std::vector<std::pair<unsigned, double>> params2{std::make_pair(360, 60), std::make_pair(180, 80), std::make_pair(120, 70),
//                                                     std::make_pair(90, 125), std::make_pair(45, 40), std::make_pair(12, 30), std::make_pair(8, 15)};
//    // std::vector<std::pair<unsigned, double>> params3{std::make_pair(360, 60), std::make_pair(180, 120), std::make_pair(120, 100),
//    //                                                  std::make_pair(90, 60), std::make_pair(45, 30), std::make_pair(12, 15), std::make_pair(8, 6), std::make_pair(3, 2)};
//    //render::ReLU(result, -10);
//    std::vector<std::pair<unsigned, double>> params4{std::make_pair(12, 1), std::make_pair(6, 3), std::make_pair(4, 3),
//                                                     std::make_pair(3, 0.3), std::make_pair(3, 0.2)};

//    perlin::PerlinNoise2D noise = perlin::PerlinNoise2D(sizeX, sizeY, params4);
//    perlin::matrix result(sizeX, std::vector<double>(sizeY, 0.0));
//    noise.fill(result);
//    //render::writeMatrixToFile(render::normalizeMatrix(result), "oop.txt");

//    //render::ReLU(result, -10);
//    perlin::matrix filter(sizeX, std::vector<double>(sizeY, 0.0));
//    std::vector<std::pair<unsigned, double>> filterParams{std::make_pair(12, 0.5), std::make_pair(6, 1), std::make_pair(3, 1), std::make_pair(1, 1)};
//    perlin::PerlinNoise2D noiseFilter(sizeX, sizeY, filterParams);
//    noiseFilter.fill(filter);

//    render::Max(result, filter);
//    render::create_png(render::normalizeMatrix(result), "small.png");
//    render::createPolyMesh(result, 1, 1, "small.obj");
//    // render::writeMatrixToFile(render::normalizeMatrix(result), "stacked.txt");
//    render::createPolyMesh(result, 1, 1, "small_stacked.obj");
// }
