/*
 * Rudy Castan
 * CS200
 * Fall 2022
 */
#pragma once

#include "mat3.h"
#include "vec2.h"


class [[nodiscard]] Camera
{
public:
    vec2 Position{};
    bool IsFirstPerson{false};

public:
    Camera() noexcept = default;
    Camera(vec2 camera_position, vec2 camera_up) noexcept;


    [[nodiscard]] vec2 GetUp() const noexcept
    {
        return up;
    }

    [[nodiscard]] vec2 GetRight() const noexcept
    {
        return right;
    }

    [[nodiscard]] float GetOrientation() const noexcept
    {
        return orientation;
    }

    void SetUp(vec2 camera_up = {0, 1}) noexcept;
    void SetOrientation(float angle_radians) noexcept;

    void MoveUp(float distance) noexcept;
    void MoveRight(float distance) noexcept;
    void Rotate(float angle_radians) noexcept;

    mat3 BuildCameraToWorld() const noexcept;

    mat3 BuildWorldToCamera() const noexcept;

private:
    vec2  up{0, 1};
    vec2  right{1, 0};
    float orientation{0};
};
