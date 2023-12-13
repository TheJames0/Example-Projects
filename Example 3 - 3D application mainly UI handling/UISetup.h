#pragma once

#include "Device.hpp"
#include "Window.hpp"
#include "GameObject.hpp"
#include "ControlState.h"

#include "ImGui/ImGuiFileDialog.h"
// libs
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_vulkan.h"
#include "ImGui/imgui_impl_glfw.h"

#include "Physics/PhysicsSolver.h"
// std
#include <stdexcept>

// UI Setup class handles IMGui dependencies currently and contains functions for UI elements
namespace Compressa
{
    static void check_vk_result(VkResult err)
    {
        if (err == 0)
            return;
        fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
        if (err < 0)
            abort();
    }

    class UISetup
    {
    public:
        using id_t = unsigned int;
        UISetup(Window &window, Device &device, VkRenderPass renderPass, uint32_t imageCount, ControlState &state, std::vector<GameObject> &gameObjects);
        ~UISetup();
        UISetup(const UISetup &) = delete;

        void newFrame();

        void render(VkCommandBuffer commandBuffer);

        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        void runMainMenu();
        void runAddObjectWindow();
        void runObjectExplorer();
        void runSelectedObjectPanel();
        void runSimulationWindow();
        void runPhysicsWindow(Physics::SimulationSettings &physicsSolver);
        void runMaterialsWindow();
        void runExit();

        const char *openfilename = nullptr;
        id_t selectedId = 0;

    private:
        Device &device;
        ControlState &state;
        Window &window;

        std::vector<GameObject> &gameObjects;

        GameObject &searchForElement();

        VkDescriptorPool descriptorPool;
    };
} // namespace Compressa