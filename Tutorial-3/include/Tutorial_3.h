

#pragma once

#include "GLShader.h"
#include "GLVertexArray.h"
#include "IProgram.h"
#include "angles.h"
#include "color3.h"
#include "mat3.h"

#include <array>

class Tutorial_3 : public IProgram
{
public:
    Tutorial_3(int viewport_width, int viewport_height);

    void Update() override;
    void Draw() override;
    void ImGuiDraw() override;
    void HandleEvent(SDL_Window& sdl_window, const SDL_Event& event) override;



    enum ModelType
    {
        Rectangle,
        MysteryShape,
        Count
    };

    struct Object
    {
        float     rotation_speed = 0;
        float     rotation = 0;
        vec2      scale = vec2{ 1.0f };
        vec2      position = vec2{ 0.0f };
        ModelType which_model = ModelType::Rectangle;
    };

private:
    constexpr static unsigned MAX_NUMBER_OBJECTS = 32'768;
    constexpr static float    WORLD_SIZE_MIN = -5'000.0f;
    constexpr static float    WORLD_SIZE_MAX = 5'000.0f;
    constexpr static float    OBJ_SIZE_MIN = 50.0f;
    constexpr static float    OBJ_SIZE_MAX = 400.0f;
    constexpr static float    ROT_SPEED_MIN = util::to_radians(-30.0f);
    constexpr static float    ROT_SPEED_MAX = util::to_radians(30.0f);

    struct
    {
        unsigned long long prev_time = 0;
        unsigned long long start_time = 0;
        float              fps = 0;
        int                count = 0;
    } timing;

    enum PolyMode : GLenum
    {
        Point = GL_POINT,
        Line = GL_LINE,
        Fill = GL_FILL
    };

    struct
    {
        int      item_current_idx = 2;
        PolyMode type = PolyMode::Fill;
    } poly_mode;

    int number_of_mystery = 0;
    int number_of_boxes = 0;

    // TODO
    std::vector<Object> objects;
    GLVertexArray models[ModelType::Count];
    GLShader shader;
private:
    void          on_window_resized(SDL_Window& sdl_window) noexcept;
    void          update_timing() noexcept;
    void          create_models();
    void          increase_number_of_objects();
    void
        decrease_number_of_objects();

};