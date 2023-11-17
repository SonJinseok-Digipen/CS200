/*
 * Rudy Castan
 * CS200
 * Fall 2022
 */

#include "Tutorial_4.h"

#include "angles.h"
#include "color3.h"
#include "glCheck.h"
#include "vec2.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>
#include <algorithm>
#include <array>
#include <fstream>
#include <imgui.h>
#include <iostream>
#include <sstream>
#include <string>

util::owner<IProgram *> create_program(int viewport_width, int viewport_height)
{
    return new Tutorial_4(viewport_width, viewport_height);
}

Tutorial_4::Tutorial_4(int viewport_width, int viewport_height)
{
    // TODO
    caminfo.camera_view.SetFramebufferSize(viewport_width, viewport_height);
    load_scene("assets/scenes/tutorial-4.scn");
    timing.start_time = SDL_GetTicks64();
    timing.prev_time = timing.start_time;
}

void Tutorial_4::Update()
{
    update_timing();
    caminfo.camera.MoveUp(caminfo.move_speed * timing.DeltaTime * caminfo.move_scalar);
    caminfo.camera.MoveRight(caminfo.move_speed * timing.DeltaTime * caminfo.strafe_scalar);
    caminfo.camera.Rotate(caminfo.object.angle_speed * timing.DeltaTime * caminfo.turn_scalar);

    mat3 world_to_cam = caminfo.camera.BuildWorldToCamera();
    mat3 cam_to_ndc = caminfo.camera_view.BuildCameraToNDC();

    mat3 world_to_cam_to_ndc = cam_to_ndc * world_to_cam;


    // for each game object
    // update object's angle displacement based off delta time and it's angle speed
    // T <- build translation from object's position
    // R <- build rotation from object's angle displacement
    // H <- build scale from object's width height
    // model_to_word <- T * R * H
    // object's model_to_ndc <- world_to_ndc * model_to_word

    for (auto& object : objects)
    {
        object.angle_disp +=object.angle_speed* timing.DeltaTime;
        mat3 model_scale =mat3::build_scale(object.scaling.x, object.scaling.y);
        mat3 model_rotate = mat3::build_rotation(object.angle_disp);
        mat3 model_translation = mat3::build_translation(object.position.x,object.position.y);

        mat3 model_to_world = model_translation * model_rotate * model_scale;
        object.model_to_ndc = world_to_cam_to_ndc * model_to_world;


    }
    // TODO



    mat3 cam_model_scale= mat3::build_scale(caminfo.object.scaling.x, caminfo.object.scaling.y);
    mat3 cam_model_rotate = mat3::build_rotation(caminfo.camera.GetOrientation());
    mat3 cam_model_translation = mat3::build_translation(caminfo.camera.Position);

    mat3 cam_model_to_world = cam_model_translation * cam_model_rotate * cam_model_scale;

    caminfo.object.model_to_ndc = world_to_cam_to_ndc * cam_model_to_world;



    compute_mouse_coordinates();
}

namespace
{
    std::span<const float, 3 * 3> to_span(const mat3 &m)
    {
        return std::span<const float, 3 * 3>(&m.elements[0][0], 9);
    }
    std::span<const float, 3> to_span(const color3 &c)
    {
        return std::span<const float, 3>(&c.elements[0], 3);
    }
}

void Tutorial_4::Draw()
{
    glClearColor(0.0, 0.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    if (!objects.empty())
    {
        unsigned current_shader = 0;
        unsigned current_model = 0;
        
        for (int i=0; i<objects.size();i++)
        {
            
          
;           shaders.at(objects[i].shader_index).Use();
            models.at(objects[i].model_index).Use();
            shaders[objects[i].shader_index].SendUniform("uModelToNDC", to_span(objects[i].model_to_ndc));
            shaders[objects[i].shader_index].SendUniform("uFillColor",  to_span(objects[i].color));


            
            GLDrawIndexed(models[objects[i].model_index]);
         
            models.at(objects[i].model_index).Use(false);
            shaders.at(objects[i].shader_index).Use(false);


        }
        shaders.at(caminfo.object.shader_index).Use();
        models.at(caminfo.object.model_index).Use();

        shaders.at(caminfo.object.shader_index).SendUniform("uModelToNDC", to_span(caminfo.object.model_to_ndc));
        shaders.at(caminfo.object.shader_index).SendUniform("uFillColor", to_span(caminfo.object.color));

        GLDrawIndexed(models[caminfo.object.model_index]);

        models.at(caminfo.object.model_index).Use(false);
        shaders.at(caminfo.object.shader_index).Use(false);
       



       
        // for each object
            // if object shader doesn't match current one
                // save index to object's shader
                // use object's shader
            // if object's model doesn't match current one
                // save index to object's model
                // use object's model
            // Send object's model_to_ndc via the "uModelToNDC" uniform
            // Send object's color via the "uFillColor" uniform
            // Draw the current indexed model
        // Draw the camera's object
    }


}

void Tutorial_4::ImGuiDraw()
{
    ImGui::Begin("Program Info");
    {
        ImGui::LabelText("FPS", "%.1f", static_cast<double>(timing.fps));
        ImGui::LabelText("Delta time", "%.3f seconds", static_cast<double>(timing.DeltaTime));
        ImGui::LabelText("Mouse Positions", "Device(%4.0f,%4.0f)\nCamera(%4.0f,%4.0f)\nWorld (%4.0f,%4.0f)", static_cast<double>(environment.mouseX),
                         static_cast<double>(environment.mouseY),                                                             // device space
                         static_cast<double>(environment.mouseCamSpaceX), static_cast<double>(environment.mouseCamSpaceY),    // in Camera Space
                         static_cast<double>(environment.mouseWorldSpaceX), static_cast<double>(environment.mouseWorldSpaceY) // in World Space
        );
        const auto cam_pos = caminfo.camera.Position;
        ImGui::LabelText("Camera World Position", "(%.1f,%.1f)", static_cast<double>(cam_pos.x), static_cast<double>(cam_pos.y));
        const auto cam_orientation = int(util::to_degrees(caminfo.camera.GetOrientation()) + 360) % 360;
        ImGui::LabelText("Camera World Orientation", "%d degrees", cam_orientation);
        const auto cam_size = caminfo.camera_view.CalcViewSizeWithZoom();
        ImGui::LabelText("Camera View Size", "%.1f x %.1f", static_cast<double>(cam_size.width), static_cast<double>(cam_size.height));
        auto zoom = caminfo.camera_view.GetZoom() * 100.0f;
        if (ImGui::SliderFloat("Zoom", &zoom, 25.0f, 400.0f, "%.0f%%"))
        {
            caminfo.camera_view.SetZoom(zoom / 100.0f);
        }

        ImGui::Checkbox("First Person Camera", &caminfo.camera.IsFirstPerson);

        if (ImGui::Button("See Camera Controls?"))
            ImGui::OpenPopup("Camera Controls");
        const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        if (ImGui::BeginPopup("Camera Controls", ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::Text("Left/Right Arrows to turn camera left & right");
            ImGui::Separator();
            ImGui::Text("Up/Down Arrows to turn move camera forward & backward");
            ImGui::Separator();
            ImGui::Text("A/D keys to strafe left & right");
            ImGui::Separator();
            ImGui::SetItemDefaultFocus();
            if (ImGui::Button("Okay"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }
    ImGui::End();
}

void Tutorial_4::HandleEvent(SDL_Window &sdl_window, const SDL_Event &event)
{
    switch (event.type)
    {
    case SDL_MOUSEMOTION:
        if (event.motion.windowID == SDL_GetWindowID(&sdl_window))
        {
            // TODO save to environment.mouseX/mouseY
            environment.mouseX = event.motion.x;
            environment.mouseY = event.motion.y;
        }
        break;
    case SDL_KEYDOWN:
    case SDL_KEYUP:
    {
        // TODO: handle keyboard
        float value = 1.0f;
        if (event.key.state == SDL_PRESSED)
        {
            value = 1.f;
          
        }
        else
        { // SDL_RELEASED
          // release
            value = 0.0f;
        }
        switch (event.key.keysym.sym)
        {
        case SDLK_UP:
            caminfo.move_scalar = value;
            break;
        case SDLK_DOWN:
            caminfo.move_scalar = -1*value;
            // TODO
            break;
        case SDLK_LEFT:
            caminfo.turn_scalar = value;
            // TODO
            break;
        case SDLK_RIGHT:
            caminfo.turn_scalar = -1*value;
            // TODO
            break;
        case SDLK_a:
          //  caminfo.strafe_scalar = -1 * value; 
            // TODO
            break;
        case SDLK_d:
            //caminfo.strafe_scalar = 1 * value;
            // TODO
            break;
        }
    }
    break;
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

void Tutorial_4::on_window_resized(SDL_Window &sdl_window) noexcept
{
    int width = 0, height = 0;
    SDL_GL_GetDrawableSize(&sdl_window, &width, &height);
    glCheck(glViewport(0, 0, width, height));
    // TODO update Camera view framebuffer size
    caminfo.camera_view.SetFramebufferSize(width,height);
    
}

void Tutorial_4::update_timing() noexcept
{
    const auto current_time = SDL_GetTicks64();
    const auto delta_ms = current_time - timing.prev_time;
    timing.DeltaTime = delta_ms / 1'000.0f;

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

void Tutorial_4::compute_mouse_coordinates() noexcept
{
   

   mat3 WindowDevice_to_Camera=caminfo.camera_view.BuildWindowDeviceToCamera();
   vec3 DevicePosition{ static_cast<float>(environment.mouseX) , static_cast<float>(environment.mouseY),1 };

   vec3 mouseCamSpace = WindowDevice_to_Camera * DevicePosition;
   //std::cout << mouseCamSpace.x << '\n';
   environment.mouseCamSpaceX = mouseCamSpace.x;
   environment.mouseCamSpaceY = mouseCamSpace.y;





    //caminfo.camera.BuildCameraToWorld();



}

void Tutorial_4::load_scene(const std::filesystem::path &scene_file_path)
{
   
    
    std::ifstream sceneFile{ scene_file_path };
    if (!sceneFile)
    {
        throw std::runtime_error(std::string("ERROR: Unable to open scene file: ") + scene_file_path.string());
    }
    



    std::string line;
    std::getline(sceneFile, line);

    int objectCount;
    std::istringstream{ line } >> objectCount;
    objects.reserve(static_cast<unsigned>(objectCount));

    std::string           modelName;
    std::string           objectName;
    std::string           shaderProgramName;
    float red, green, blue;
    float width, height;
    float positionx, positiony;
    std::filesystem::path vertexShaderPath, fragmentShaderPath;
    [[maybe_unused]] float                 rotationAngular;
   [[maybe_unused]]  float  rotationSpeedAngular;


    for (int i = 0; i < objectCount; ++i)
    {
        Object object;
        std::getline(sceneFile, line);
        std::istringstream{ line } >> modelName;
       

        std::getline(sceneFile, line);
        std::istringstream{ line } >> object.name;

        object.model_index = load_mesh_and_get_id(std::filesystem::path("assets") / "meshes" / (modelName + ".msh"));

        std::getline(sceneFile, line);
        std::istringstream{ line } >> shaderProgramName >> vertexShaderPath >> fragmentShaderPath;
        object.shader_index = load_shader_and_get_id(shaderProgramName, vertexShaderPath, fragmentShaderPath);

        //load color
        std::getline(sceneFile, line);
        std::istringstream{ line } >> red >> green >> blue;
        object.color = color3{ red,green,blue };

        std::getline(sceneFile, line);
        std::istringstream{ line } >> width >> height;
        object.scaling = vec2{ width,height };

        std::getline(sceneFile, line);
        std::istringstream{ line } >> rotationAngular >> rotationSpeedAngular;
        object.angle_disp =  util::to_radians(rotationAngular);
        object.angle_speed = util::to_radians(rotationSpeedAngular);

  

        std::getline(sceneFile, line);
        std::istringstream{ line } >> positionx >> positiony;
        object.position.x = positionx;
        object.position.y = positiony;
       
       
        if (object.name != "Camera")
        {
            objects.emplace_back(std::move(object));
        }
        else
        {
            caminfo.object = object;
            caminfo.camera.SetOrientation(object.angle_disp);
            caminfo.camera.Position = object.position;

        }
       
    }






}

unsigned Tutorial_4::load_mesh_and_get_id(const std::filesystem::path &mesh_file_path)
{

     
    std::ifstream meshFile{ mesh_file_path };
    if (!meshFile)
    {
        throw std::runtime_error{ std::string("ERROR: Unable to open mesh file: ") + mesh_file_path.string() };
    }

    auto [iter, is_new] = asset_to_index.insert({ std::string(mesh_file_path.string()), 0 });
    auto& location = iter->second;

    if (!is_new)
    {
        return location;
    }


    else
    {




        std::vector<vec2>           vertex_positions;
        std::vector<unsigned short> vertex_indices;

        std::string       modelName;
        float                           x = 0, y = 0;
        GLushort          index = 0;
        std::string       line;
        char              mode = 0;

        GLPrimitive::Type primitive_type = GLPrimitive::Points;

        while (meshFile)
        {
            std::getline(meshFile, line);




            if (line.empty())
            {
                continue;
            }

            std::istringstream sstr{ line };
            sstr >> mode;

            switch (mode)
            {
            case 'n':
                sstr >> modelName;

                break;
            case 'v':
                sstr >> x >> y;
                vertex_positions.push_back(vec2{ x,y });
                break;
            case 'f':
                if (vertex_indices.empty())
                {

                    primitive_type = GLPrimitive::TriangleFan;
                    for (int i = 0; i < 3; i++)
                    {
                        sstr >> index;
                        vertex_indices.push_back(index);
                    }

                }
                else
                {
                    sstr >> index;
                    vertex_indices.push_back(index);
                }
                break;

            case 't':
                if (vertex_indices.empty())
                {

                    primitive_type = GLPrimitive::Triangles;

                    for (int i = 0; i < 3; i++)
                    {
                        sstr >> index;
                        vertex_indices.push_back(index);
                    }
                }
                else
                {
                    for (int i = 0; i < 3; i++)
                    {
                        sstr >> index;
                        vertex_indices.push_back(index);
                    }
                }
                break;
            default:
                break;


            }

        }






        unsigned int position_buffer_size = static_cast<unsigned int>(sizeof(vec2) * vertex_positions.size());
        GLVertexBuffer position_buffer(position_buffer_size);
        position_buffer.SetData(std::span{ vertex_positions });

        GLAttributeLayout position_layout;
        position_layout.component_dimension = GLAttributeLayout::_2;
        position_layout.component_type = GLAttributeLayout::Float;
        position_layout.normalized = false;
        position_layout.offset = 0;
        position_layout.relative_offset = 0;
        position_layout.stride = sizeof(vec2);
        position_layout.vertex_layout_location = 0;


        GLIndexBuffer index_buffer(vertex_indices);
        GLVertexArray model;
        model.AddVertexBuffer(std::move(position_buffer), { position_layout });
        model.SetIndexBuffer(std::move(index_buffer));
        model.SetPrimitivePattern(std::move(primitive_type));



        int location = static_cast<int>(models.size());
        models.push_back(std::move(model));
         return location;
    }


       


}

unsigned Tutorial_4::load_shader_and_get_id(std::string_view name, const std::filesystem::path &vertex_filepath, const std::filesystem::path &fragment_filepath)
{
    // Insert if 1st time otherwise get what is there
    auto [iter, is_new] = asset_to_index.insert({ std::string(name), 0 });
    auto& location = iter->second;
    
    if (!is_new)
    {
        return location;
    }
    location = static_cast<unsigned>(shaders.size());
    shaders.emplace_back(GLShader(name, { {GLShader::VERTEX, vertex_filepath}, {GLShader::FRAGMENT, fragment_filepath} }));
    return location;
}



//unsigned Tutorial_4::load_shader_and_get_id(std::string_view name, const std::filesystem::path& vertex_filepath, const std::filesystem::path& fragment_filepath)
//{
//    // Insert if 1st time otherwise get what is there
//    auto [iter, is_new] = asset_to_index.insert({ std::string(name), 0 });
//    auto& location = iter->second;
//    if (!is_new)
//    {
//        return location;
//    }
//    location = static_cast<unsigned>(shaders.size());
//    shaders.emplace_back(GLShader(name, { {GLShader::VERTEX, vertex_filepath}, {GLShader::FRAGMENT, fragment_filepath} }));
//    return location;
//}