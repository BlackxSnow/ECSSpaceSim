#pragma once

#include <flecs.h>
#include <utility/CCXShader.h>
#include <Engine.h>
#include <modules/rendering/Rendering.h>

struct Player {};
struct Opponent {};
struct PongBall {};

inline flecs::entity LeftPaddle;
inline flecs::entity RightPaddle;
inline flecs::entity Ball;

void CreateScene(flecs::world& world);