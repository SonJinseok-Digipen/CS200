/*
 * Rudy Castan
 * CS200
 * Fall 2022
 */

#include "Tutorial_2.h"

#include "Random.h"
#include "angles.h"
#include "color3.h"
#include "glCheck.h"
#include "vec2.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <cmath>
#include <imgui.h>
#include <iostream>
#include <vec2.h>
#include<array>
#include<cmath>
util::owner<IProgram*> create_program(int viewport_width, int viewport_height)
{
    return new Tutorial_2(viewport_width, viewport_height);
}

Tutorial_2::Tutorial_2(int viewport_width, int viewport_height)
{
    // TODO

    timing.start_time = SDL_GetTicks64();
    timing.prev_time  = timing.start_time;

    set_viewports(viewport_width, viewport_height);



    /*
     Create Shader Object linking and compile 
    
    */
    shader = GLShader("Basic Shader",
        { {GLShader::VERTEX, "assets/shaders/pass_thru_pos2d_clr.vert"},
        {GLShader::FRAGMENT, "assets/shaders/basic_vtx_clr_attribute.frag"} });

    /*
    Lets make test Rectangle for test our viesport work correctly      
    */



    constexpr std::array<vec2,4>position_data{ vec2{0.5f,-0.5f},vec2{0.5f,0.5f},vec2{-0.5f,0.5f},vec2{-0.5f,-0.5f} };
    GLint position_data_size = static_cast<unsigned int>(sizeof(vec2) * position_data.size());
    GLVertexBuffer position(position_data_size);
    position.SetData(std::span(position_data));

    GLAttributeLayout position_layout;
    position_layout.component_dimension = GLAttributeLayout::NumComponents::_2;
    position_layout.component_type = GLAttributeLayout::ComponentType::Float;
    position_layout.vertex_layout_location = 0;
    position_layout.normalized = GL_FALSE;
    position_layout.relative_offset = 0;
    position_layout.offset = 0;
    position_layout.stride = sizeof(vec2);

    constexpr std::array<vec3, 4>color_data{ vec3{0.0f,0.0f,1.0f},vec3{0.0f,1.0f,0.0f},vec3{1.0f,0.0f,0.0},vec3{0.0f,1.0f,0.0f} };
    GLuint color_data_size = static_cast<unsigned int>(sizeof(vec3) * color_data.size());
    GLVertexBuffer color(color_data_size);
    color.SetData(std::span(color_data));

    GLAttributeLayout color_layout;
    color_layout.component_dimension = GLAttributeLayout::NumComponents::_3;
    color_layout.component_type = GLAttributeLayout::ComponentType::Float;
    color_layout.vertex_layout_location = 1;
    color_layout.normalized = GL_FALSE;
    color_layout.relative_offset = 0;
    color_layout.offset = 0;
    color_layout.stride = sizeof(vec3);

    constexpr std::array<GLuint, 6>index_data{ 0,1,2,2,3,0 };
    GLuint index_data_size = static_cast<unsigned int>(sizeof(GLuint) * index_data.size());
    GLIndexBuffer index(index_data);

    test_rectangle.AddVertexBuffer(std::move(position), { position_layout }); 
    test_rectangle.AddVertexBuffer(std::move(color), { color_layout });
    test_rectangle.SetIndexBuffer(std::move(index));



    create_points_model();
    create_lines_model();
    create_trifan_model();
    create_tristrip_model();

}

void Tutorial_2::Update()
{
    update_timing();
}


void Tutorial_2::Draw()
{

    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

   /* shader.Use();
    test_rectangle.Use();
    glViewport(ViewPorts[0].x, ViewPorts[0].y, ViewPorts[0].width, ViewPorts[0].height);
    GLDrawIndexed(test_rectangle);
    test_rectangle.Use(false);

 
    test_rectangle.Use();
    glViewport(ViewPorts[1].x, ViewPorts[1].y, ViewPorts[1].width, ViewPorts[1].height);
    GLDrawIndexed(test_rectangle);
    test_rectangle.Use(false);

    test_rectangle.Use();
    glViewport(ViewPorts[2].x, ViewPorts[2].y, ViewPorts[2].width, ViewPorts[2].height);
    GLDrawIndexed(test_rectangle);
    test_rectangle.Use(false);

    test_rectangle.Use();
    glViewport(ViewPorts[3].x, ViewPorts[3].y, ViewPorts[3].width, ViewPorts[3].height);
    GLDrawIndexed(test_rectangle);
    test_rectangle.Use(false);*/


    

    // Use Shader
    shader.Use();

    // Part 2: Render distinct geometry to four viewports:
    // Use OpenGL function glViewport to specify appropriate top left portion of display screen
    glViewport(ViewPorts[0].x, ViewPorts[0].y, ViewPorts[0].width, ViewPorts[0].height);
    {
        glPointSize(10);
        // Use model
        models[0].Use();
        // Set same color for color attributes for shader program
        constexpr int layout_location = 1;
        
        glCheck(glVertexAttrib3f(layout_location, 1.f, 0.0f, 0.f)); // red color
        // Draw Model
        GLDrawVertices(models[0]);
        models[0].Use(false);
    }
    // TODO: Later draw other three parts
     // Use OpenGL function glViewport to specify appropriate top left portion of display screen
    glViewport(ViewPorts[1].x, ViewPorts[1].y, ViewPorts[1].width, ViewPorts[1].height);
    {
        glLineWidth(2);
        // Use model
        models[1].Use();
        // Set same color for color attributes for shader program
        constexpr int layout_location = 1;

        glCheck(glVertexAttrib3f(layout_location, 0.f, 0.0f, 1.f)); // red color
        // Draw Model
        GLDrawVertices(models[1]);
        models[1].Use(false);
    }

    glViewport(ViewPorts[2].x, ViewPorts[2].y, ViewPorts[2].width, ViewPorts[2].height);
    {
       
        // Use model
        models[2].Use();
        // Set same color for color attributes for shader program
        constexpr int layout_location = 1;

        glCheck(glVertexAttrib3f(layout_location, 0.f, 0.0f, 1.f)); // red color
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // Draw Model
        GLDrawIndexed(models[2]);
        models[2].Use(false);
    }


    glViewport(ViewPorts[3].x, ViewPorts[3].y, ViewPorts[3].width, ViewPorts[3].height);
    {

        // Use model
        models[3].Use();
        // Set same color for color attributes for shader program
        constexpr int layout_location = 1;

        glCheck(glVertexAttrib3f(layout_location, 0.f, 0.0f, 1.f)); // red color
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        // Draw Model
        GLDrawIndexed(models[3]);
        models[3].Use(false);
    }




    shader.Use(false);



}

void Tutorial_2::ImGuiDraw()
{
    ImGui::Begin("Program Info");
    {
        ImGui::LabelText("FPS", "%.1f", timing.fps);
        ImGui::LabelText("GL_POINTS", "# Points    %4d", models[0].GetVertexCount());
        ImGui::LabelText("GL_LINES", "# Lines     %4d # Verts   %4d", models[1].GetVertexCount() / 2, models[1].GetVertexCount());
        
        ImGui::LabelText("GL_TRIANGLE_FAN", "# Triangles %4d # Indices %4d", models[2].GetVertexCount() - 1, models[2].GetIndicesCount());
        ImGui::LabelText("GL_TRIANGLE_STRIP", "# Triangles %4d # Indices %4d", 2 * 5 * 10, models[3].GetIndicesCount()); 
    }
    ImGui::End();
}

void Tutorial_2::HandleEvent(SDL_Window& sdl_window, const SDL_Event& event)
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

void Tutorial_2::on_window_resized(SDL_Window& sdl_window) noexcept
{
    int width = 0, height = 0;
    SDL_GL_GetDrawableSize(&sdl_window, &width, &height);
    set_viewports(width, height);
}

void Tutorial_2::update_timing() noexcept
{
    const auto current_time = SDL_GetTicks64();

    timing.prev_time      = current_time;
    const auto elapsed_ms = current_time - timing.start_time;

    timing.count++;
    if (elapsed_ms > 1'000)
    {
        timing.fps        = timing.count / (elapsed_ms / 1'000.0f);
        timing.start_time = current_time;
        timing.count      = 0;
    }
}

void Tutorial_2::set_viewports(int width, int height)
{
    glViewport(0,0,width,height);

    //Upper Left
    ViewPorts[0].x = 0;
    ViewPorts[0].y = height / 2;
    ViewPorts[0].width = width / 2;
    ViewPorts[0].height = height / 2;

    //Upper Right
    ViewPorts[1].x = width/2;
    ViewPorts[1].y = height / 2;
    ViewPorts[1].width = width / 2;
    ViewPorts[1].height = height / 2;
    //Down Left;
    ViewPorts[2].x = 0;
    ViewPorts[2].y = 0;
    ViewPorts[2].width = width / 2;
    ViewPorts[2].height = height / 2;

    //Down Right;
    ViewPorts[3].x = width/2;
    ViewPorts[3].y = 0;
    ViewPorts[3].width = width / 2;
    ViewPorts[3].height = height / 2;




}

void Tutorial_2::create_points_model()
{
    std::vector<vec2>points;
    constexpr int     slices = 20;
    constexpr int     stacks = 20;
    std::vector<vec2> pos_vtx((slices + 1)*(stacks+1));
    float vertical_interval = 2.0f / slices;

    for (int j = 0; j < stacks; j++)
    {
        for (int i = 0; i < slices; i++)
        {
            pos_vtx[(j*slices)+i] = vec2(-1.0f + (vertical_interval * i), -1.0f + (vertical_interval * j));
        }
    }

    GLuint position_buffer_size = static_cast<GLuint>(sizeof(vec2) * pos_vtx.size());
    GLVertexBuffer position_buffer(position_buffer_size);
    position_buffer.SetData(std::span(pos_vtx));

    GLAttributeLayout position;
    position.component_dimension = GLAttributeLayout::_2;
    position.component_type = GLAttributeLayout::Float;
    position.normalized = false;
    position.offset = 0;
    position.relative_offset = 0;
    position.stride = sizeof(vec2);
    position.vertex_layout_location = 0;
     

    GLVertexArray temp;
    temp.AddVertexBuffer(std::move(position_buffer), { position });
    temp.SetVertexCount(static_cast<int>(pos_vtx.size()));
    temp.SetPrimitivePattern(GLPrimitive::Points);

    models[0] = std::move(temp);



    






    // TODO
}

void Tutorial_2::create_lines_model()
{

    std::vector<vec2>points;
    constexpr int     slices = 50;
    constexpr int     stacks = 50;
    std::vector<vec2> pos_vtx((slices + 1)*2);
    float vertical_interval = 2.0f / slices;

        //draw horizental line first
        for (int i = 0; i < pos_vtx.size()/2; i+=2)
        {
            std::cout << i << '\n';
            std::cout << i + 1 << '\n';
            pos_vtx[i] = vec2(-1.0f,1.0f-(vertical_interval*i));
            pos_vtx[i+1] = vec2(1.0f, 1.0f - (vertical_interval * i));
        }
      
        // 9 10 




        //draw vertical line first
        for (int i = static_cast<int>((pos_vtx.size()/2)+1); i < pos_vtx.size(); i += 2)
        {
            std::cout << i << '\n';
            std::cout << i + 1 << '\n';
            pos_vtx[i] = vec2(-1.0f + (vertical_interval * (i-(pos_vtx.size()/2+1))), 1.0f);
            pos_vtx[i +1] = vec2(-1.0f + (vertical_interval * (i - (pos_vtx.size() / 2 + 1))), -1.0f);
        }


    
    GLuint position_buffer_size = static_cast<GLuint>(sizeof(vec2) * pos_vtx.size());
    GLVertexBuffer position_buffer(position_buffer_size);
    position_buffer.SetData(std::span(pos_vtx));

    GLAttributeLayout position;
    position.component_dimension = GLAttributeLayout::_2;
    position.component_type = GLAttributeLayout::Float;
    position.normalized = false;
    position.offset = 0;
    position.relative_offset = 0;
    position.stride = sizeof(vec2);
    position.vertex_layout_location = 0;


    GLVertexArray temp;
    temp.AddVertexBuffer(std::move(position_buffer), { position });
    temp.SetVertexCount(static_cast<int>(pos_vtx.size()));
    temp.SetPrimitivePattern(GLPrimitive::Lines);

    models[1] = std::move(temp);
    // TODO
}

void Tutorial_2::create_trifan_model()
{
    constexpr int slices = 30;
    // Step 1: Generate the (slices+2) count of vertices required to
    // render a triangle fan parameterization of a circle with unit
    // radius and centered at (0, 0)
    constexpr int vertex_count = slices + 2;
    constexpr float angle_interval = 360 / slices;
    std::array<vec2,vertex_count> pos_vtx;           //32
    std::array<unsigned int, vertex_count>index; //31
    
    pos_vtx[0] = vec2(0, 0);
    index[0] = 0;

    for (int i = 1; i < vertex_count; i++)
    {
        pos_vtx[i] = vec2( cos(static_cast<float>(util::to_radians(angle_interval * i))), sin(static_cast<float>(util::to_radians(angle_interval * i)))  );
          
    }

    for (int i = 1; i < vertex_count; i++)
    {
        index[i]=i;
    }
    
    






    GLuint position_buffer_size = static_cast<GLuint>(sizeof(vec2) * pos_vtx.size());
    GLVertexBuffer position_buffer(position_buffer_size);
    position_buffer.SetData(std::span(pos_vtx));

    GLAttributeLayout position;
    position.component_dimension = GLAttributeLayout::_2;
    position.component_type = GLAttributeLayout::Float;
    position.normalized = false;
    position.offset = 0;
    position.relative_offset = 0;
    position.stride = sizeof(vec2);
    position.vertex_layout_location = 0;

    GLIndexBuffer index_buffer(index);

    GLVertexArray temp;

    temp.AddVertexBuffer(std::move(position_buffer), { position });
    temp.SetPrimitivePattern(GLPrimitive::TriangleFan);
    temp.SetVertexCount(vertex_count);
    temp.SetIndexBuffer(std::move(index_buffer));
    models[2] = std::move(temp);



    // Step 2: In addition to vertex position coordinates, compute
    // (slices+2) count of vertex color coordinates.
    // Each RGB component must be randomly computed.
    // The last color should match the 2nd color, so that the 1st and last triangle match up

    // Step 3: Generate one or two Vertex Buffers to store the position and color attributes
    
    // Step 4: 
    // 
        // Set Vertex count for BottomLeft Model
        // Describe the layout of our array of 2d positions and rgb colors
        // Provide the vertices as a GLVertexBuffer and the layout of it for BottomLeft Model
        // Make sure we set the primitive pattern to GLPrimitive::TriangleFan
}

void Tutorial_2::create_tristrip_model()
{
    constexpr int stacks = 20;
    constexpr int slices = 20;

    constexpr int vertices_number = (stacks + 1) * (slices + 1);
    std::vector<vec2>pos_vtx;
    std::vector<unsigned int >index;
    for (int i = 0; i <= stacks; i++)
    {
        float row = static_cast<float>(static_cast<float>( i*2) / static_cast<float>(stacks));
        for (int j = 0; j <= slices; j++)
        {
            float col = static_cast<float>(static_cast<float>(j*2) / static_cast<float>(slices));
          
            pos_vtx.push_back(vec2(col-1.0f, row-1.0f));
        }
    }


    int stride = slices + 1;
    for (int i = 0; i < stacks; i++)
    {
        int curr_row = i * stride;
        for (int j = 0; j <slices; j++)
        {
            int P0 = curr_row + j;
            int P1 = P0 + 1;
            int P2 = P1 + stride;

            int P3 = P2;
            int P4 = P3 - 1;
            int P5 = P0;

            std::cout << P0 << '\n';
            std::cout << P1 << '\n';
            std::cout << P2 << '\n';
            std::cout << P3 << '\n';
            std::cout << P4 << '\n';
            std::cout << P5 << '\n';

           index.push_back(P0);
            index.push_back(P1);
            index.push_back(P2);

            index.push_back(P3);
            index.push_back(P4);
            index.push_back(P5);

            



        }
    }



    GLuint position_buffer_size = static_cast<GLuint>(sizeof(vec2) * pos_vtx.size());
    GLVertexBuffer position_buffer(position_buffer_size);
    position_buffer.SetData(std::span(pos_vtx));

    GLAttributeLayout position;
    position.component_dimension = GLAttributeLayout::_2;
    position.component_type = GLAttributeLayout::Float;
    position.normalized = false;
    position.offset = 0;
    position.relative_offset = 0;
    position.stride = sizeof(vec2);
    position.vertex_layout_location = 0;

    GLIndexBuffer index_buffer(index);

    GLVertexArray temp;

    temp.AddVertexBuffer(std::move(position_buffer), { position });
    //
    temp.SetPrimitivePattern(GLPrimitive::Triangles);
    temp.SetIndexBuffer(std::move(index_buffer));
    models[3] = std::move(temp);



}
