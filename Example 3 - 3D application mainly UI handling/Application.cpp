#include "Application.hpp"

#include "CameraControls.hpp"
#include "Camera.hpp"
#include "RenderSystem.hpp"

// Loaders
#include "Loader/Loader.h"

// IMGUI
#include "Controls/UISetup.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_vulkan.h"
#include "ImGui/imgui_impl_glfw.h"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

// std
#include <array>
#include <cassert>
#include <chrono>
#include <stdexcept>

#include <vector>
#include "Model.hpp"
#include <iostream>

namespace Compressa
{

  Application::Application()
  {
  }

  Application::~Application()
  {
  }
  // Setup UI Callback functions to be activated by ui elements
  void Application::setupUICallbacks(UISetup ui)
  {
    controlState = ControlState();
    controlState.setCallback("Add_Window", [&]()
                             { uiSetup.runAddObjectWindow(); });
    controlState.setCallback("Physics_Window", [&, this]()
                             { uiSetup.runPhysicsWindow(physicsSolver.settings); });
    controlState.setCallback("Materials_Window", [&]()
                             { uiSetup.runMaterialsWindow(); });
    controlState.setCallback("Exit", [&]()
                             { uiSetup.runExit(); });
    controlState.setCallback("Load_ObjFile", [&, this]()
                             {
                              std::vector<glm::vec3> vertices{};
                              std::vector<glm::vec3> colors{};
                              std::vector<uint32_t> indices{};
                              std::string filename = std::string(uiSetup.openfilename);
                              Compressa::Util::Loader::LoadObj(filename.c_str(), vertices, colors, indices);

                            
                              auto vertices2 = Model::createVertices(vertices, colors);
                              if (vertices2.size() == 0)
                              {
                                std::cout << "Error loading file" << std::endl;
                                return;
                              }
                                Model::Builder modelBuilder{};
                              modelBuilder.vertices = vertices2;
                              modelBuilder.indices = indices;
                               auto model1 = std::make_unique<Model>(device, modelBuilder);
                               auto model2 = std::shared_ptr<Model>(model1.release());
                               auto cube = GameObject::createGameObject(filename);
                               cube.model = model2;
                               gameObjects.push_back(std::move(cube));
                               controlState.setVariable("Load_ObjFile", false); });

    controlState.setCallback("Object_Explorer", [&]()
                             { uiSetup.runObjectExplorer(); });
    controlState.setCallback("SelectedObject", [&, this]()
                             {
                              try{uiSetup.runSelectedObjectPanel(); } catch(std::exception e){std::cout << "Error: " << e.what() << std::endl;} });
  }

    // Run the application
  void Application::run()
  {
    UISetup uiSetup = UISetup(window, device, renderer.getSwapChainRenderPass(), renderer.getImageCount(), controlState, gameObjects);

    RenderSystem simpleRenderSystem{device, renderer.getSwapChainRenderPass()};
    Camera camera{};
    auto viewerObject = GameObject::createGameObject("CameraObject");
    CameraControls cameraController{};
    auto currentTime = std::chrono::high_resolution_clock::now();
    while (!window.shouldClose())
    {
      glfwPollEvents();

      auto newTime = std::chrono::high_resolution_clock::now();
      float frameTime =
          std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
      physicsSolver.Update(frameTime);
      currentTime = newTime;
      // Physics Update
      cameraController.moveInPlaneXZ(window.getGLFWwindow(), frameTime, viewerObject);
      camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

      float aspect = renderer.getAspectRatio();
      camera.setPerspectiveProjection(glm::radians(50.f), aspect, 0.1f, 10.f);

      if (auto commandBuffer = renderer.beginFrame())
      {
        uiSetup.newFrame();
        controlState.doCallbacks();
        renderer.beginSwapChainRenderPass(commandBuffer);

        simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects, camera);
        uiSetup.runMainMenu();
        uiSetup.render(commandBuffer);
        renderer.endSwapChainRenderPass(commandBuffer);
        renderer.endFrame();
      }
    }

    vkDeviceWaitIdle(device.device());
  }
} // namespace Compressa
