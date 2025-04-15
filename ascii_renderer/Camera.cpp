#include "Camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\quaternion.hpp>
#include <glm\gtx\quaternion.hpp>

Camera::Camera(int width, int height, float fontAspectRatio, float fovAngleYDegrees, float nearClip, float farClip)
	:
	width_(width),
	height_(height),
	aspectRatio_((float(width) / (float)height) * fontAspectRatio),
	fovAngleY_(fovAngleYDegrees),
	nearClip_(nearClip),
	farClip_(farClip),
	position_(0, 0, 2),
	forwardDir_(0, 0, -1)
{

	constexpr glm::vec3 upDir = { 0, 1, 0 };
	rightDir_ = glm::normalize(glm::cross(forwardDir_, upDir));

	RecalculateView();
	RecalculateProjection();
	RecalculateRayDirections();
}

void Camera::Move(float dt, glm::vec3 dir) noexcept
{
	if (dir == glm::vec3{ 0 }) {
		return;
	}
	
	dir *= moveSpeed_;

	if (dir.x != 0) {
		position_ += rightDir_ * dt * dir.x;
	}
	if (dir.y != 0) {
		position_ += glm::vec3{ 0, 1, 0 } *dt * dir.y;
	}
	if (dir.z != 0) {
		position_ += forwardDir_ * dt * dir.z;
	}

	moved = true;
}

void Camera::Rotate(float dt, float deltaX, float deltaY) noexcept
{
	if (deltaX != 0 && deltaY != 0) {
		float pitchDelta = deltaY * dt * rotationSpeed_;
		float yawDelta = deltaX * dt * rotationSpeed_;

		glm::quat q = glm::normalize(glm::angleAxis(-pitchDelta, rightDir_) * glm::angleAxis(-yawDelta, glm::vec3{ 0, 1, 0 }));
		forwardDir_ = glm::rotate(q, forwardDir_);

		constexpr glm::vec3 upDir = { 0, 1, 0 };

		rightDir_ = glm::normalize(glm::cross(forwardDir_, upDir));

		moved = true;
	}
}

void Camera::Update() noexcept
{
	if (moved) {
		RecalculateView();
		RecalculateRayDirections();
		moved = false;
	}
}

void Camera::Resize(int width, int height, float aspectRatio) noexcept
{
	if (width_ == width && height_ == height && aspectRatio_ == aspectRatio) {
		return;
	}

	width_ = width;
	height_ = height;
	aspectRatio_ = aspectRatio;
	RecalculateProjection();
	RecalculateRayDirections();
}

void Camera::SetMoveSpeed(float speed) noexcept
{
	moveSpeed_ = speed;
}

void Camera::SetRotationSpeed(float speed) noexcept
{
	rotationSpeed_ = speed;
}

const glm::vec3& Camera::GetPosition() const noexcept
{
	return position_;
}

const glm::vec3& Camera::GetDirection() const noexcept
{
	return forwardDir_;
}

const glm::mat4& Camera::GetProjection() const noexcept
{
	return projection_;
}

const glm::mat4& Camera::GetInverseProjection() const noexcept
{
	return inverseProjection_;
}

const glm::mat4& Camera::GetView() const noexcept
{
	return view_;
}

const glm::mat4& Camera::GetInverseView() const noexcept
{
	return inverseView_;
}

const std::vector<glm::vec3>& Camera::GetRayDirections() const noexcept
{
	return rayDirections_;
}

void Camera::RecalculateProjection() noexcept
{
	projection_ = glm::perspective(glm::radians(fovAngleY_), aspectRatio_, nearClip_, farClip_);
	inverseProjection_ = glm::inverse(projection_);
}

void Camera::RecalculateView() noexcept
{
	view_ = glm::lookAt(position_, position_ + forwardDir_, glm::vec3{ 0, 1, 0 });
	inverseView_ = glm::inverse(view_);
}

void Camera::RecalculateRayDirections() noexcept
{
	rayDirections_.resize(width_ * height_);

	for (int y = 0; y < height_; ++y) {
		for (int x = 0; x < width_; ++x) {
			glm::vec2 coord = { float(x) / (float)width_, float(y) / float(height_) };
			coord = coord * 2.0f - 1.0f;

			glm::vec4 target = inverseProjection_ * glm::vec4(coord, 1, 1);
			glm::vec3 rayDirection = glm::vec3(inverseView_ * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0));
			rayDirections_[y * width_ + x] = std::move(rayDirection);
		}
	}
}
