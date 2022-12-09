#pragma once

constexpr float FPS = 60.0f;

constexpr int PARTICLE_NUM = 9;

constexpr float DECAY = 0.9999f;
constexpr float GRAVITY = 0.2f;
constexpr float SPRING = 100.0f;
constexpr float DAMPING = 8.0f;
constexpr float FRICTION = 0.05f;

constexpr float RESTITUION[4][4] = {
	0.9f, 0.9f, 0.7f, 0.8f,
	0.9f, 0.9f, 0.7f, 0.8f,
	0.7f, 0.7f, 0.4f, 0.5f,
	0.8f, 0.8f, 0.5f, 0.6f,
};

