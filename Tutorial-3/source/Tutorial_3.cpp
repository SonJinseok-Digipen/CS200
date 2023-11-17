

#include "Tutorial_3.h"

#include "Random.h"
#include "angles.h"
#include "color3.h"
#include "glCheck.h"
#include "vec2.h"

#include<random>
#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <algorithm>
#include <imgui.h>
#include <iostream>

util::owner<IProgram*> create_program(int viewport_width, int viewport_height)
{
    return new Tutorial_3(viewport_width, viewport_height);
}

Tutorial_3::Tutorial_3(int viewport_width, int viewport_height)
{
    // TODO
    glCheck(glViewport(0, 0, viewport_width, viewport_height));
    glCheck(glClearColor(1.0, 1.0, 1.0, 0.0));


    create_models();
    shader = GLShader("Basic Shader", { {GLShader::VERTEX, "assets/shaders/pass_thru_pos2d_clr.vert"}, {GLShader::FRAGMENT, "assets/shaders/basic_vtx_clr_attribute.frag"} });

    objects.reserve(MAX_NUMBER_OBJECTS);

    std::default_random_engine dre;
    // get numbers in range [-1, 1) - notice that interval is half-
    // open on the right. When we submit the tutorial, specify a
    // closed range [-1, 1]
    std::uniform_real_distribution<float> urdf(-1.0, 1.0);
    std::uniform_real_distribution<float> urdf1(0.0, 0.3f);
    std::uniform_real_distribution<float> model_index(0, 2);
    dre.seed((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
    constexpr float WorldRange{ 5000.0f };

    Object temp;
    temp.scale = { 400,400 };
    temp.rotation = urdf(dre) * util::TWO_PI<float>;     // current orientation
    temp.rotation_speed = urdf1(dre) * util::to_radians(100.0f); // radians per second
    temp.position = vec2(urdf(dre) * WorldRange,
        urdf(dre) * WorldRange);;
    temp.which_model = static_cast<Tutorial_3::ModelType>(model_index(dre));

    if (temp.which_model == Tutorial_3::MysteryShape)
    {
        number_of_mystery++;
    }
    if (temp.which_model == Tutorial_3::Rectangle)
    {
        number_of_boxes++;
    }

    objects.push_back(temp);


    timing.start_time = SDL_GetTicks64();
    timing.prev_time = timing.start_time;


}

void Tutorial_3::Update()
{
    update_timing();
    for (auto& object : objects)
    {

        object.rotation += object.rotation_speed;
    }
    // TODO
}

namespace
{
    std::span<const float, 3 * 3> to_span(const mat3& m)
    {
        return std::span<const float, 3 * 3>(&m.elements[0][0], 9);
    }
}


void Tutorial_3::Draw()
{
    // TODO
    glCheck(glClear(GL_COLOR_BUFFER_BIT));
    shader.Use();




    for (const auto& object : objects)
    {
        const mat3 scale =
        {
            vec3{object.scale.x ,   0,                      0},
            vec3{0,                 object.scale.y,         0},
            vec3{0,                 0,                      1},
        };


        const mat3 rotation =
        {
            vec3{cos(util::to_radians(object.rotation)) ,    sin(util::to_radians(object.rotation)),  0},
            vec3{-sin(util::to_radians(object.rotation)),     cos(util::to_radians(object.rotation)),  0},
            vec3{0,                       0,                      1},
        };

        const mat3 translate =
        {
            vec3{1 ,   0,  0},
            vec3{0,    1,  0},
            vec3{object.position.x,    object.position.y,  1 },
        };

        const mat3 world_to_ndc =
        {
            vec3{1 / 5000.f ,   0,  0},
            vec3{0,    1 / 5000.f,  0},
            vec3{0,    0,      1},

        };
        const mat3 model_to_world = translate * rotation * scale;

        const mat3 model_to_ndc = world_to_ndc * model_to_world;


        shader.SendUniform("UModleToNDC", to_span(model_to_ndc));
        models[object.which_model].Use();
        if (poly_mode.type == PolyMode::Point)
        {
            glCheck(glPolygonMode(GL_FRONT, GL_POINT));
        }

        if (poly_mode.type == PolyMode::Line)
        {
            glCheck(glPolygonMode(GL_FRONT, GL_LINE));
        }

        if (poly_mode.type == PolyMode::Fill)
        {
            glCheck(glPolygonMode(GL_FRONT, GL_FILL));
        }
        GLDrawIndexed(models[object.which_model]);
    }

    shader.Use(false);
}

void Tutorial_3::ImGuiDraw()
{
    ImGui::Begin("Program Info");
    {
        ImGui::LabelText("FPS", "%.1f", timing.fps);

        if (objects.size() == MAX_NUMBER_OBJECTS)
        {
            ImGui::BeginDisabled();
            ImGui::ArrowButton("+", ImGuiDir_Up);
            ImGui::EndDisabled();
        }
        else
        {
            if (const bool increase = ImGui::ArrowButton("+", ImGuiDir_Up); increase)
            {
                increase_number_of_objects();

            }
        }
        ImGui::SameLine();
        if (objects.size() == 1)
        {
            ImGui::BeginDisabled();
            ImGui::ArrowButton("-", ImGuiDir_Down);
            ImGui::EndDisabled();
        }
        else
        {
            if (const bool decrease = ImGui::ArrowButton("-", ImGuiDir_Down); decrease)
            {
                decrease_number_of_objects();
            }
        }
        ImGui::SameLine();
        ImGui::Text("%s %lu", "Size", static_cast<unsigned long>(objects.size()));
        ImGui::LabelText("# Boxes", "%d", number_of_boxes);
        ImGui::LabelText("# Mystery Shapes", "%d", number_of_mystery);


        constexpr const char* items[] = { "GL_POINT", "GL_LINE", "GL_FILL" };
        constexpr PolyMode    types[] = { PolyMode::Point, PolyMode::Line, PolyMode::Fill };
        const char* const     combo_preview_value = items[poly_mode.item_current_idx];
        if (ImGui::BeginCombo("PolygonMode", combo_preview_value, 0))
        {
            for (int n = 0; n < IM_ARRAYSIZE(items); n++)
            {
                const bool is_selected = (poly_mode.item_current_idx == n);
                if (ImGui::Selectable(items[n], is_selected))
                {
                    poly_mode.item_current_idx = n;
                    poly_mode.type = types[n];
                }
                if (is_selected) // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
    }
    ImGui::End();
}

void Tutorial_3::HandleEvent(SDL_Window& sdl_window, const SDL_Event& event)
{
    switch (event.type)
    {
    case SDL_WINDOWEVENT:
    {
        switch (event.window.event)
        {
        case SDL_WINDOWEVENT_RESIZED:
        {
            on_window_resized(sdl_window);
        }
        break;
        case SDL_WINDOWEVENT_SIZE_CHANGED:
        {
            on_window_resized(sdl_window);
        }
        break;
        }
    }
    }
}

void Tutorial_3::on_window_resized(SDL_Window& sdl_window) noexcept
{
    int width = 0, height = 0;
    SDL_GL_GetDrawableSize(&sdl_window, &width, &height);
    glCheck(glViewport(0, 0, width, height));
}

void Tutorial_3::update_timing() noexcept
{
    const auto current_time = SDL_GetTicks64();

    timing.prev_time = current_time;
    const auto elapsed_ms = current_time - timing.start_time;

    timing.count++;
    if (elapsed_ms > 1'000)
    {
        timing.fps = timing.count / (elapsed_ms / 1'000.0f);
        timing.start_time = current_time;
        timing.count = 0;
    }
}


void Tutorial_3::create_models()
{
    { //TODO: rectangle
        constexpr GLuint count = 4;

        std::vector<GLuint> idx_vtx;
        std::vector<vec2> pos_vtx =
        {
            vec2{-0.5,0.5},
            vec2{-0.5,-0.5},
            vec2{0.5,0.5},
            vec2{0.5,-0.5}
        };

        std::vector<color3>clr_vtx =
        {
            color3{1.0f,0.0f,1.0f},
            color3{0.0f,1.0f,1.0f},
            color3{0.0f,0.0f,1.0f},
            color3{0.3f,0.2f,1.0f}
        };

        for (int i = 0; i < count; i++)
        {
            idx_vtx.push_back(i);
        }
        idx_vtx.push_back(count - 1);


        GLAttributeLayout position;
        position.component_dimension = GLAttributeLayout::_2;
        position.component_type = GLAttributeLayout::Float;
        position.normalized = false;
        position.offset = 0;
        position.relative_offset = 0;
        position.stride = sizeof(vec2);
        position.vertex_layout_location = 0;


        GLAttributeLayout color;
        color.component_type = GLAttributeLayout::Float;
        color.component_dimension = GLAttributeLayout::_3;
        color.normalized = false;
        color.vertex_layout_location = 1; // 2nd field of Vertex
        color.stride = sizeof(color3);
        color.offset = 0; // starts after the position bytes
        color.relative_offset = 0;


        GLIndexBuffer index(idx_vtx);


        models[Rectangle].AddVertexBuffer(GLVertexBuffer(std::span{ pos_vtx }), { position });
        models[Rectangle].AddVertexBuffer(GLVertexBuffer(std::span{ clr_vtx }), { color });
        models[Rectangle].SetIndexBuffer(std::move(index));
        models[Rectangle].SetPrimitivePattern(GLPrimitive::TriangleStrip);
    }

    { //TODO: mystery shape



        std::vector<vec2> pos_vtx =
        {
            //Fisrt Triangle
            vec2{0.0f,1.0f},  //0
            vec2{-1.0f,-0.55f}, //1
            vec2{1.0f,-0.55f},  //2

            //Second Triangle
            vec2{-1.0f,0.55f}, //3
            vec2{1.0f,0.55f},  //4
            vec2{0.0f,-1.0f},  //5

        };
        std::vector<color3>clr_vtx =
        {
            color3{1.0f,0.0f,1.0f},
            color3{0.0f,1.0f,1.0f},
            color3{0.0f,0.0f,1.0f},

            color3{0.2f,1.0f,0.5f},
            color3{0.4f,0.6f,0.1f},
            color3{0.3f,0.2f,0.7f}
        };
        std::vector<GLuint>idx_vtx =
        {
            0,
            1,
            2,

            //Second Triangle
            3,
            4,
            5,
        };






        GLAttributeLayout position;
        position.component_dimension = GLAttributeLayout::_2;
        position.component_type = GLAttributeLayout::Float;
        position.normalized = false;
        position.offset = 0;
        position.relative_offset = 0;
        position.stride = sizeof(vec2);
        position.vertex_layout_location = 0;


        GLAttributeLayout color;
        color.component_type = GLAttributeLayout::Float;
        color.component_dimension = GLAttributeLayout::_3;
        color.normalized = false;
        color.vertex_layout_location = 1; // 2nd field of Vertex
        color.stride = sizeof(color3);
        color.offset = 0; // starts after the position bytes
        color.relative_offset = 0;

        GLIndexBuffer index(idx_vtx);

        models[MysteryShape].AddVertexBuffer(GLVertexBuffer(std::span{ pos_vtx }), { position });
        models[MysteryShape].AddVertexBuffer(GLVertexBuffer(std::span{ clr_vtx }), { color });
        models[MysteryShape].SetIndexBuffer(std::move(index));
        models[MysteryShape].SetPrimitivePattern(GLPrimitive::Triangles);
    }
}


void Tutorial_3::increase_number_of_objects()
{
    static int current_increase = 1;
    std::default_random_engine dre;
    // get numbers in range [-1, 1) - notice that interval is half-
    // open on the right. When we submit the tutorial, specify a
    // closed range [-1, 1]
    std::uniform_real_distribution<float> urdf(-1.0, 1.0);
    std::uniform_real_distribution<float> urdf1(0.0, 0.3f);
    std::uniform_real_distribution<float> model_index(0, 2);
    dre.seed((unsigned int)std::chrono::system_clock::now().time_since_epoch().count());
    constexpr float WorldRange{ 5000.0f };

    for (int i = 0; i < current_increase; i++)
    {
        Object temp;
        temp.scale = { 400,400 };
        temp.rotation = urdf(dre) * util::TWO_PI<float>;     // current orientation
        temp.rotation_speed = urdf1(dre) * util::to_radians(10.0f); // radians per second
        temp.position = vec2(urdf(dre) * WorldRange,
            urdf(dre) * WorldRange);;
        temp.which_model = static_cast<Tutorial_3::ModelType>(model_index(dre));
        objects.push_back(temp);
        if (temp.which_model == Tutorial_3::MysteryShape)
        {
            number_of_mystery++;
        }
        if (temp.which_model == Tutorial_3::Rectangle)
        {
            number_of_boxes++;
        }

    }
    current_increase *= 2;



}
void Tutorial_3::decrease_number_of_objects()
{
    int mystery_num = 0;
    int rectangle_num = 0;
    int size = static_cast<int>(objects.size() / 2);

    for (int i = 0; i < size; i++)
    {
        if (objects[i].which_model == Tutorial_3::MysteryShape)
        {
            mystery_num++;
        }

        else
        {
            rectangle_num++;
        }

    }
    number_of_boxes -= rectangle_num;
    number_of_mystery -= mystery_num;

    objects.erase(objects.begin(), objects.begin() + objects.size() / 2);
}