/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** IGraphics
*/

#pragma once

class IGraphics {
    public:
        virtual ~IGraphics() = default;

        virtual void initWindow(int width, int height, const char* title) = 0;
        virtual void clearWindow() = 0;
        virtual void displayWindow() = 0;
        virtual bool isWindowOpen() const = 0;
        virtual void closeWindow() = 0;
};
