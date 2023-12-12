#pragma once

#include "Device.hpp"
#include "GameObject.hpp"
#include "Renderer.hpp"
#include "Window.hpp"
#include "Controls/UISetup.h"
#include "Controls/ControlState.h"
#include "Physics/PhysicsSolver.h"
// std
#include <memory>
#include <vector>

namespace Compressa
{
  class Application
  {
  public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    Application();
    ~Application();

    Application(const Application &) = delete;
    Application &operator=(const Application &) = delete;

    void run();
    void addGameObject();
    void setupUICallbacks(UISetup ui);

  private:
    Window window{WIDTH, HEIGHT, "Compressa"};
    Device device{window};
    Renderer renderer{window, device};
    ControlState controlState;
    std::vector<GameObject>
        gameObjects;
    Physics::PhysicsSolver physicsSolver{};
  };
} // namespace Compressa
