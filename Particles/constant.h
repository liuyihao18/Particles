#pragma once

constexpr float PI = 3.14159265358979323846f;
constexpr float E = 2.71828182845904523536f;

constexpr int PARTICLE_NUM = 10;

constexpr int PROTO_NUM = 4;

constexpr int GRID_SIZE = 64;

constexpr float DECAY = 0.9999f;
constexpr float GRAVITY = 0.2f;
constexpr float SPRING = 1000.0f;
constexpr float DAMPING = 8.0f;
constexpr float FRICTION = 0.05f;
constexpr float DELTA_T = 0.02f;

constexpr float RESTITUION[4][4] = {
	0.9f, 0.9f, 0.7f, 0.8f,
	0.9f, 0.9f, 0.7f, 0.8f,
	0.7f, 0.7f, 0.4f, 0.5f,
	0.8f, 0.8f, 0.5f, 0.6f,
};
