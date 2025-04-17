#pragma once

#include <glm\glm.hpp>
#include <vector>

class Camera {
public:
	Camera(int width, int height, float fontAspectRatio, float fovAngleYDegrees, float nearClip, float farClip);
	void Move(float dt, glm::vec3 dir) noexcept;
	void Rotate(float dt, float deltaX, float deltaY) noexcept;
	void Update() noexcept;
	void Resize(int width, int height, float aspectRatio) noexcept;
	void SetMoveSpeed(float speed) noexcept;
	void SetRotationSpeed(float speed) noexcept;
	const glm::vec3& GetPosition() const noexcept;
	const glm::vec3& GetDirection() const noexcept;
	const glm::mat4& GetProjection() const noexcept;
	const glm::mat4& GetInverseProjection() const noexcept;
	const glm::mat4& GetView() const noexcept;
	const glm::mat4& GetInverseView() const noexcept;
	const std::vector<glm::vec3>& GetRayDirections() const noexcept;
private:
	void RecalculateProjection() noexcept;
	void RecalculateView() noexcept;
	void RecalculateRayDirections() noexcept;
private:
	bool moved = false;
	int width_ = 0;
	int height_ = 0;
	float moveSpeed_ = 5.0f;
	float rotationSpeed_ = 0.09f;
	float aspectRatio_ = 1.0f;
	float nearClip_ = 0.1f;
	float farClip_ = 100.0f;
	float fovAngleY_ = 45.0f;
	glm::vec3 position_;
	glm::vec3 forwardDir_;
	glm::vec3 rightDir_;
	glm::mat4 view_;
	glm::mat4 projection_;
	glm::mat4 inverseView_;
	glm::mat4 inverseProjection_;
	std::vector<glm::vec3> rayDirections_;
};