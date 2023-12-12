#pragma once

#include "../GameObject.hpp"
#include <functional>
#include <unordered_map>
namespace Compressa
{

    // Handle UI states, callbacks and data
    struct ControlState
    {
        std::unordered_map<std::string, std::pair<bool, std::function<void()>>> variables;
        ;
        bool getVariable(const std::string &name) const
        {
            auto it = variables.find(name);
            if (it != variables.end())
            {
                return it->second.first;
            }
            return false; // Default value if variable not found
        }

        void setVariable(const std::string &name, bool value)
        {
            auto it = variables.find(name);
            if (it != variables.end())
            {
                it->second.first = value;
                if (value && it->second.second)
                {
                    it->second.second();
                }
            }
        }

        void setCallback(const std::string &name, std::function<void()> callback)
        {
            variables[name].second = callback;
        }
        ControlState()
        {
            // Main Toolbar variables
            variables["Add_Window"] = {false, nullptr};
            variables["Object_Explorer"] = {false, nullptr};
            variables["Physics_Window"] = {false, nullptr};
            variables["Materials_Window"] = {false, nullptr};
            variables["Exit"] = {false, nullptr};

            // Secondary variables
            variables["Load_ObjFile"] = {false, nullptr};
            variables["SelectedObject"] = {false, nullptr};
        }
        void doCallbacks()
        {
            for (auto &variable : variables)
            {
                if (variable.second.first && variable.second.second)
                {
                    variable.second.second();
                }
            }
        }
    };

}