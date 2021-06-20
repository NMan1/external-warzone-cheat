#include "vectors.h"
#include <cstdint>
#include <algorithm>
#include <iomanip>
#include <random>

float bits_to_float(std::uint32_t i) {
	union convertor_t
	{
		float f;
		unsigned long ul;
	}tmp;

	tmp.ul = i;
	return tmp.f;
}

vec3_t::vec3_t(void) {
	x = y = z = 0.0f;
}

vec3_t::vec3_t(float fx, float fy, float fz) {
	x = fx;
	y = fy;
	z = fz;
}

vec3_t::~vec3_t(void) {
};

void vec3_t::init(float ix, float iy, float iz) {
	x = ix; y = iy; z = iz;
}

vec3_t vec3_t::clamp(void) {
	x = std::clamp(x, -89.0f, 89.0f);
	y = std::clamp(std::remainder(y, 360.0f), -180.0f, 180.0f);
	z = std::clamp(z, -50.0f, 50.0f);
	return vec3_t(x, y, z);
}

vec3_t vec3_t::clamped() {
	vec3_t clamped = *this;
	clamped.clamp();
	return clamped;
}

float vec3_t::normalize_float() {
	vec3_t res = *this;
	float l = res.length();
	if (l != 0.0f) {
		res /= l;
	}
	else {
		res.x = res.y = res.z = 0.0f;
	}
	return l;
}

float vec3_t::distance_to(const vec3_t& other) {
	vec3_t delta;
	delta.x = x - other.x;
	delta.y = y - other.y;
	delta.z = z - other.z;

	return delta.length();
}

void vec3_t::normalize(void) {
	auto vec_normalize = [&](vec3_t& v) {
		auto l = v.length();

		if (l != 0.0f) {
			v.x /= l;
			v.y /= l;
			v.z /= l;
		}
		else {
			v.x = v.y = 0.0f; v.z = 1.0f;
		}

		return l;
	};

	vec_normalize(*this);
}

vec3_t vec3_t::normalized(void) {
	vec3_t vec(*this);
	vec.normalize();

	return vec;
}

float vec3_t::length(void) {
	auto sqr = [](float n) {
		return static_cast<float>(n * n);
	};

	return sqrt(sqr(x) + sqr(y) + sqr(z));
}

float vec3_t::length_2d_sqr(void) const {
	return (x * x + y * y);
}

float vec3_t::dot(const vec3_t other) {
	return (x * other.x + y * other.y + z * other.z);
}

float vec3_t::dot(const float* other) {
	const vec3_t& a = *this;

	return(a.x * other[0] + a.y * other[1] + a.z * other[2]);
}

D2D1_RECT_F vec2_t::rect() {
	return D2D1::RectF(static_cast<FLOAT>(x), static_cast<FLOAT>(y), static_cast<FLOAT>(INT_MAX), static_cast<FLOAT>(INT_MAX));
}
