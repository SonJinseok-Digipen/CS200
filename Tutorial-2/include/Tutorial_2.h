/*
 * Rudy Castan
 * CS200
 * Fall 2022
 */

#pragma once

#include "GLShader.h"
#include "GLVertexArray.h"
#include "IProgram.h"
#include <array>
class Tutorial_2 : public IProgram
{
public:
    Tutorial_2(int viewport_width, int viewport_height);

    void Update() override;
    void Draw() override;
    void ImGuiDraw() override;
    void HandleEvent(SDL_Window& sdl_window, const SDL_Event& event) override;

private:
    struct
    {
        unsigned long long prev_time  = 0;
        unsigned long long start_time = 0;
        float              fps        = 0;
        int                count      = 0;
    } timing;

   // TODO

private:

    struct ViewPort
    {
        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;
    };


    enum class ViewPortPosition
    {
        TopLeft,
        TopRight,
        BottomLeft,
        BottomRight,
    };



    std::array<GLVertexArray,4>models;
    std::array<ViewPort,4>ViewPorts;
    GLVertexArray test_rectangle;

    GLShader shader;
    void on_window_resized(SDL_Window& sdl_window) noexcept;
    void update_timing() noexcept;
    void set_viewports(int width, int height);
    void create_points_model();
    void create_lines_model();
    void create_trifan_model();
    void create_tristrip_model();
};
