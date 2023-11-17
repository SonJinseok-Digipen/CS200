/*
 * Rudy Castan
 * CS200
 * Fall 2022
 */
#pragma once
#include "mat3.h"
#include "vec2.h"

class [[nodiscard]] CameraView
{
public:
    void SetFramebufferSize(vec2 the_framebuffer_size) noexcept;
    void SetFramebufferSize(int new_framebuffer_width, int new_framebuffer_height) noexcept;
    vec2 GetViewSize() const;
    void SetZoom(float new_zoom) noexcept;

    vec2 CalcViewSizeWithZoom() const noexcept
    {
        return framebuffer_size / zoom;
    }

    [[nodiscard]] float GetZoom() const noexcept
    {
        return zoom;
    }

    mat3 BuildCameraToNDC() const noexcept;

    mat3 BuildWindowDeviceToCamera() const noexcept;

private:
    vec2  framebuffer_size{};
    float zoom{1.0f};
};
