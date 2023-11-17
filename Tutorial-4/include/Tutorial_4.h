/*
 * Rudy Castan
 * CS200
 * Fall 2022
 */

#pragma once

#include "Camera.h"
#include "CameraView.h"
#include "GLShader.h"
#include "GLVertexArray.h"
#include "IProgram.h"
#include "angles.h"
#include "color3.h"
#include "mat3.h"

#include <unordered_map>
#include <vector>

class Tutorial_4 : public IProgram
{
public:
    Tutorial_4(int viewport_width, int viewport_height);

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
        float              DeltaTime{0};
        int                count;
    } timing;

    struct
    {
        int   mouseX           = 0;
        int   mouseY           = 0;
        float mouseCamSpaceX   = 0;
        float mouseCamSpaceY   = 0;
        float mouseWorldSpaceX = 0;
        float mouseWorldSpaceY = 0;
    } environment;

    // TODO
    struct Object
    {
        unsigned    model_index = 0;
        std::string name;
        unsigned    shader_index = 0;
        color3      color{};
        vec2        scaling{ 1.0f };
        float       angle_speed = 0;
        float       angle_disp = 0;
        vec2        position{};
        mat3        model_to_ndc{};
    };
    struct CamInfo
    {
        Object     object{};
        Camera     camera{};
        CameraView camera_view{};
        float      move_scalar = 0;
        float      turn_scalar = 0;
        float      strafe_scalar = 0;
        float      move_speed = 120.0f;
    } caminfo;








private:
    std::vector<Object>objects;
    std::vector<GLShader>shaders;
    std::vector<GLVertexArray>models;
    std::unordered_map<std::string, unsigned> asset_to_index;
    unsigned int asset_id = 0;

    void     on_window_resized(SDL_Window& sdl_window) noexcept;
    void     update_timing() noexcept;
    void     compute_mouse_coordinates() noexcept;
    void     load_scene(const std::filesystem::path& scene_file_path);
    unsigned load_mesh_and_get_id(const std::filesystem::path& mesh_file_path);
    unsigned load_shader_and_get_id(std::string_view name, const std::filesystem::path& vertex_filepath, const std::filesystem::path& fragment_filepath);
};
