#include "UISetup.h"

#include "ImGui/imgui_impl_vulkan.h"
#include "ImGui/imgui_impl_glfw.h"

// Materials
#include "Physics/PhysicsMaterial.h"
#include "Loader/Loader.h"

// std
#include <stdexcept>
#include <iostream>
#include <string>
#include <filesystem>
namespace Compressa
{

    inline void SetupImGuiStyle()
    {
        ImGuiStyle &style = ImGui::GetStyle();
        style.Alpha = 0.95;
        style.ChildRounding = 5;
        style.WindowRounding = 5;
        style.GrabRounding = 1;
        style.GrabMinSize = 20;
        style.FrameRounding = 5;

        style.Colors[ImGuiCol_Text] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.00f, 0.40f, 0.41f, 1.00f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
        style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_Border] = ImVec4(0.00f, 1.00f, 1.00f, 0.0f);
        style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.44f, 0.80f, 0.80f, 0.18f);
        style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.44f, 0.80f, 0.80f, 0.27f);
        style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.44f, 0.81f, 0.86f, 0.66f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.14f, 0.48f, 0.41f, 0.73f);
        style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.54f);
        style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.27f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
        style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.22f, 0.29f, 0.30f, 0.71f);
        style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.44f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);

        style.Colors[ImGuiCol_CheckMark] = ImVec4(0.00f, 1.00f, 1.00f, 0.68f);
        style.Colors[ImGuiCol_SliderGrab] = ImVec4(0.00f, 1.00f, 1.00f, 0.36f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.76f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.00f, 0.65f, 0.65f, 0.46f);
        style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.01f, 1.00f, 1.00f, 0.43f);
        style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.62f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 1.00f, 1.00f, 0.33f);
        style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.42f);
        style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);

        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 1.00f, 1.00f, 0.54f);
        style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.00f, 1.00f, 1.00f, 0.74f);
        style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);

        style.Colors[ImGuiCol_PlotLines] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.00f, 1.00f, 1.00f, 1.00f);
        style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.00f, 1.00f, 1.00f, 0.22f);
    }

    // ok this just initializes imgui using the provided integration files. So in our case we need to
    // initialize the vulkan and glfw imgui implementations, since that's what our engine is built
    // using.
    UISetup::UISetup(
        Window &window, Device &dev, VkRenderPass renderPass, uint32_t imageCount, ControlState &state, std::vector<GameObject> &gameObjects)
        : device{dev}, state{state}, window{window}, gameObjects{gameObjects}
    {
        // set up a descriptor pool stored on this instance, see header for more comments on this.
        VkDescriptorPoolSize pool_sizes[] = {
            {VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
            {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
            {VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
            {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
            {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
            {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000}};
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
        pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;
        if (vkCreateDescriptorPool(device.device(), &pool_info, nullptr, &descriptorPool) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to set up descriptor pool for imgui");
        }

        // Setup Dear ImGui context
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO &io = ImGui::GetIO();
        (void)io;
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        // Setup Dear ImGui style
        // ImGui::StyleColorsDark();
        SetupImGuiStyle();
        //  Setup Platform/Renderer backends
        //  Initialize imgui for vulkan
        ImGui_ImplGlfw_InitForVulkan(window.getGLFWwindow(), true);
        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = device.getInstance();
        init_info.PhysicalDevice = device.getPhysicalDevice();
        init_info.Device = device.device();
        init_info.QueueFamily = device.getGraphicsQueueFamily();
        init_info.Queue = device.graphicsQueue();

        // pipeline cache is a potential future optimization, ignoring for now
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = descriptorPool;
        // todo, I should probably get around to integrating a memory allocator library such as Vulkan
        // memory allocator (VMA) sooner than later. We don't want to have to update adding an allocator
        // in a ton of locations.
        init_info.Allocator = VK_NULL_HANDLE;
        init_info.MinImageCount = 2;
        init_info.ImageCount = imageCount;
        init_info.CheckVkResultFn = check_vk_result;
        ImGui_ImplVulkan_Init(&init_info, renderPass);

        // upload fonts, this is done by recording and submitting a one time use command buffer
        // which can be done easily bye using some existing helper functions on the lve device object
        auto commandBuffer = device.beginSingleTimeCommands();
        ImGui_ImplVulkan_CreateFontsTexture();
        device.endSingleTimeCommands(commandBuffer);
        ImGui_ImplVulkan_DestroyFontsTexture();
    }

    UISetup::~UISetup()
    {
        vkDestroyDescriptorPool(device.device(), descriptorPool, nullptr);
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();
    }

    void UISetup::newFrame()
    {
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
    }

    // this tells imgui that we're done setting up the current frame,
    // then gets the draw data from imgui and uses it to record to the provided
    // command buffer the necessary draw commands
    void UISetup::render(VkCommandBuffer commandBuffer)
    {
        ImGui::Render();
        ImDrawData *drawdata = ImGui::GetDrawData();
        ImGui_ImplVulkan_RenderDrawData(drawdata, commandBuffer);
    }

    /// UI Code definitions
    void UISetup::runMainMenu()
    {
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Objects"))
            {
                if (ImGui::MenuItem("Add Object", "Ctrl+A"))
                {
                    state.setVariable("Add_Window", !state.getVariable("Add_Window"));
                }
                if (ImGui::MenuItem("Object Explorer", "Ctrl+O"))
                {
                    state.setVariable("Object_Explorer", !state.getVariable("Object_Explorer"));
                    if (state.getVariable("Object_Explorer") == false)
                    {
                        state.setVariable("SelectedObject", false);
                    }
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Physics"))
            {
                if (ImGui::MenuItem("Physics Settings", "Ctrl+A"))
                {
                    state.setVariable("Physics_Window", !state.getVariable("Physics_Window"));
                }
                if (ImGui::MenuItem("Material Settings", "Ctrl+A"))
                {
                    state.setVariable("Materials_Window", !state.getVariable("Materials_Window"));
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Exit"))
            {
                if (ImGui::MenuItem("Exit", "Ctrl+X"))
                {
                    state.setVariable("Exit", true);
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }
    }
    void UISetup::runAddObjectWindow()
    {
        // open Dialog Simple
        ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose File", ".obj", ".");

        // display
        if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"))
        {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                openfilename = ImGuiFileDialog::Instance()->GetFilePathName().c_str();
                state.setVariable("Load_ObjFile", true);
            }

            // close
            ImGuiFileDialog::Instance()->Close();
            state.setVariable("Add_Window", false);
        }
    }
    void UISetup::runObjectExplorer()
    {
        ImGui::Begin("Object Explorer");
        if (ImGui::BeginTable("Objects", 3)) // 2 columns
        {
            ImGui::TableSetupColumn("ID");
            ImGui::TableSetupColumn("Name");
            ImGui::TableHeadersRow();

            for (GameObject &gameobject : gameObjects)
            {
                ImGui::TableNextRow();
                ImGui::TableSetColumnIndex(0);
                bool isSelected = (gameobject.getId() == selectedId);
                if (ImGui::Selectable(std::to_string(gameobject.getId()).c_str(), isSelected, ImGuiSelectableFlags_SpanAllColumns))
                {
                    std::cout << gameobject.getId() << std::endl;
                    selectedId = gameobject.getId();
                    state.setVariable("SelectedObject", true);
                }
                ImGui::TableSetColumnIndex(1);
                ImGui::Text(gameobject.getName().c_str()); // Assuming GameObject has a getName() method
                ImGui::TableSetColumnIndex(2);
            }
            ImGui::EndTable();
        }

        ImGui::End();
    }
    void UISetup::runExit()
    {
        glfwSetWindowShouldClose(window.getGLFWwindow(), GL_TRUE);
        std::cout << "Exit" << std::endl;
    }
    void UISetup::runSelectedObjectPanel()
    {

        GameObject &gameObject = searchForElement();
        ImGui::Begin("Selected Object");
        ImGui::Text("Name: %s", gameObject.getName().c_str());
        ImGui::Text("ID: %d", gameObject.getId());

        // Input fields for Vector3 position
        glm::vec3 position = gameObject.transform.getTranslation();
        float positionArray[3] = {position.x, position.y, position.z};
        ImGui::Text("Position");
        ImGui::SetNextItemWidth(80);
        if (ImGui::InputFloat("PX", &positionArray[0]))
        {
            glm::vec3 newPosition = glm::vec3(positionArray[0], positionArray[1], positionArray[2]);
            gameObject.transform.setTranslation(newPosition);
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        if (ImGui::InputFloat("PY", &positionArray[1]))
        {
            glm::vec3 newPosition = glm::vec3(positionArray[0], positionArray[1], positionArray[2]);
            gameObject.transform.setTranslation(newPosition);
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        if (ImGui::InputFloat("PZ", &positionArray[2]))
        {
            glm::vec3 newPosition = glm::vec3(positionArray[0], positionArray[1], positionArray[2]);
            gameObject.transform.setTranslation(newPosition);
        }
        // Input fields for Vector3 rotation
        glm::vec3 rotation = glm::degrees(gameObject.transform.getRotation());
        float rotationArray[3] = {rotation.x, rotation.y, rotation.z};
        ImGui::Text("Rotation");
        ImGui::SetNextItemWidth(80);
        if (ImGui::InputFloat("RX", &rotationArray[0]))
        {
            glm::vec3 newRotation = glm::vec3(rotationArray[0], rotationArray[1], rotationArray[2]);
            gameObject.transform.setRotation(glm::radians(newRotation));
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        if (ImGui::InputFloat("RY", &rotationArray[1]))
        {
            glm::vec3 newRotation = glm::vec3(rotationArray[0], rotationArray[1], rotationArray[2]);
            gameObject.transform.setRotation(glm::radians(newRotation));
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        if (ImGui::InputFloat("RZ", &rotationArray[2]))
        {
            glm::vec3 newRotation = glm::vec3(rotationArray[0], rotationArray[1], rotationArray[2]);
            gameObject.transform.setRotation(glm::radians(newRotation));
        }
        // Input fields for Vector3 position
        glm::vec3 scale = gameObject.transform.getScale();
        float scaleArray[3] = {scale.x, scale.y, scale.z};
        ImGui::Text("Scale");
        ImGui::SetNextItemWidth(80);
        if (ImGui::InputFloat("SX", &scaleArray[0]))
        {
            glm::vec3 newScale = glm::vec3(scaleArray[0], scaleArray[1], scaleArray[2]);
            gameObject.transform.setScale(newScale);
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        if (ImGui::InputFloat("SY", &scaleArray[1]))
        {
            glm::vec3 newScale = glm::vec3(scaleArray[0], scaleArray[1], scaleArray[2]);
            gameObject.transform.setScale(newScale);
        }
        ImGui::SameLine();
        ImGui::SetNextItemWidth(80);
        if (ImGui::InputFloat("SZ", &scaleArray[2]))
        {
            glm::vec3 newScale = glm::vec3(scaleArray[0], scaleArray[1], scaleArray[2]);
            gameObject.transform.setScale(newScale);
        }
        if (ImGui::Button("Delete"))
        {
            std::cout << "Deleted element " << gameObject.getId() << std::endl;

            try
            {
                // Wait for the device to be idle before destroying the game object
                vkDeviceWaitIdle(device.device());
                gameObjects.erase(gameObjects.begin() + gameObject.getId());
            }
            catch (const std::exception &e)
            {
                // Handle the exception here
                std::cout << "Exception occurred: " << e.what() << std::endl;
            }

            selectedId = -1;
            state.setVariable("SelectedObject", false);
        }
        ImGui::End();
    }
    GameObject &UISetup::searchForElement()
    {
        for (GameObject &gameobject : gameObjects)
        {
            if (gameobject.getId() == selectedId)
            {
                return gameobject;
            }
        }
    }
    void UISetup::runPhysicsWindow(Physics::SimulationSettings &settings)
    {
        ImGui::Begin("Physics Settings");
        if (ImGui::SliderFloat("Time Step", &settings.timeStep, 0.0f, 3.0f))
        {
            // Handle time step change
        }
        if (ImGui::InputInt("Sub Steps", &settings.subSteps))
        {
            // Handle sub steps change
        }
        if (ImGui::InputFloat("Gravity", &settings.gravity))
        {
            // Handle gravity change
        }
        if (ImGui::InputFloat("Drag Coefficient", &settings.dragCoefficient))
        {
        }
        ImGui::Text("Elapsed Time: %f s", settings.elapsedTime);
        ImGui::Text("Step speed == : %f ms per S", settings.selectedStepRate * 1000);
        ImGui::End();
    }
    void UISetup::runMaterialsWindow()
    {
        std::vector<std::string> files = Compressa::Util::Loader::listFilesWithExtension("mcompressa");
        ImGui::Begin("Materials Settings");

        if (ImGui::BeginTabBar("FileTabs"))
        {
            for (std::string file : files)
            {
                std::filesystem::path filePath(file);
                std::string fileName = filePath.filename().string();
                fileName = fileName.substr(0, fileName.find_last_of("."));
                if (ImGui::BeginTabItem(fileName.c_str()))
                {
                    std::vector<Definitions::PhysicsMaterial> materials = Compressa::Util::Loader::LoadMaterialDefinitions(file.c_str());
                    std::cout << materials.size() << std::endl;

                    for (Definitions::PhysicsMaterial &material : materials)
                    {
                        Definitions::PhysicsMaterial newMaterial(material);
                        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(material.color.x, material.color.y, material.color.z, 1.0f));        // Red color
                        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(material.color.x, material.color.y, material.color.z, 0.5f)); // Slightly transparent red when hovered
                        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(material.color.x, material.color.y, material.color.z, 0.3f));  // More transparent red when active
                        ImVec4 invertedColor = ImVec4(0.9f - material.color.x, 0.9f - material.color.y, 0.9f - material.color.z, 255.0f);

                        ImGui::PushStyleColor(ImGuiCol_Text, invertedColor);
                        std::string headerLabel = material.name + "##" + file;
                        if (ImGui::CollapsingHeader(headerLabel.c_str()))
                        {
                            ImGui::PopStyleColor();
                            ImGui::Indent(); // Add indentation for each material

                            std::string headerstiffness = "Stiffness##" + material.name + file;
                            std::string headercx = "Colour X##" + material.name + file;
                            std::string headercy = "Colour Y##" + material.name + file;
                            std::string headercz = "Colour Z##" + material.name + file;
                            if (ImGui::InputFloat(headerstiffness.c_str(), &newMaterial.stiffness))
                            {

                                // Handle stiffness change
                                Util::Loader::UpdateMaterialDefintions(file.c_str(), material.name.c_str(), newMaterial);
                            }

                            if (ImGui::InputFloat(headercx.c_str(), &newMaterial.color.x))
                            {
                                // Handle color.x change
                                Util::Loader::UpdateMaterialDefintions(file.c_str(), material.name.c_str(), newMaterial);
                            }

                            if (ImGui::InputFloat(headercy.c_str(), &newMaterial.color.y))
                            {
                                // Handle color.y change
                                Util::Loader::UpdateMaterialDefintions(file.c_str(), material.name.c_str(), newMaterial);
                            }

                            if (ImGui::InputFloat(headercz.c_str(), &newMaterial.color.z))
                            {
                                // Handle color.z change
                                Util::Loader::UpdateMaterialDefintions(file.c_str(), material.name.c_str(), newMaterial);
                            }
                            ImGui::Unindent();
                        }
                        else
                        {
                            ImGui::PopStyleColor();
                        }

                        ImGui::PopStyleColor(3);
                    }

                    ImGui::EndTabItem();
                }
            }

            ImGui::EndTabBar();
        }

        ImGui::End();
    }
}