#pragma once

#include "../GameObject.hpp"
#include <functional>
#include <unordered_map>
#include <iostream>
#include <stdexcept>
namespace Compressa
{

    // Handle UI states, callbacks and data
    struct ControlState
    {
        std::unordered_map<std::string, std::pair<bool, std::function<void()>>> variables;
        ;
        // return the state of a variable with the given key
        bool getVariable(const std::string &name) const
        {
            for (auto &[key, pair] : variables)
            {
                if (key == name)
                {
                    auto &[boolValue, callbackValue] = pair;
                    return boolValue;
                }
            }
            std::cerr << "Variable not found: " << &name << std::endl;
            return false; // Default value if variable not found
        }

        void setVariable(const std::string &name, bool value)
        {
            for (auto &[key, pair] : variables)
            {
                if (key == name)
                {
                    auto &[boolValue, callbackValue] = pair;
                    boolValue = value;
                    return;
                }
            }
            std::cerr << "Variable not found: " << name << std::endl;
        }
        // Set a callback function for a Ui 'Event
        void setCallback(const std::string &name, std::function<void()> callback)
        {
            for (auto &[key, pair] : variables)
            {
                if (key == name)
                {
                    auto &[boolValue, callbackValue] = pair;
                    callbackValue = callback;
                    return;
                }
            }
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
        // Call all the callbacks methods that are set to true
        void doCallbacks()
        {

            for (auto &[key, pair] : variables)
            {
                auto &[boolValue, callbackValue] = pair;
                if (boolValue && callbackValue)
                {
                    callbackValue();
                }
            }
        }
    };
} // namespace Compressa