#include <iostream>
#include <cmath>
#include <utility>

#include "rtmath.h"

using namespace std;

vec2::vec2() {
    this->x = 0;
    this->y = 0;
}

vec2::vec2(double x, double y) {
    this->x = x;
    this->y = y;
}

vec2::vec2(double angle) {
    this->x = std::cos(angle);
    this->y = std::sin(angle);
}

vec2 vec2::normalize() {
    double len = sqrt(this->x*this->x + this->y*this->y);
    return vec2(this->x / len, this->y / len);
}

vec2 vec2::rotate(double angle) {
    return vec2(this->x*cos(angle) + this->y*sin(angle), this->x*(-sin(angle)) + this->y*cos(angle));
}

void vec2::operator+=(const vec2 &a) {
    this->x += a.x;
    this->y += a.y;
}

void vec2::operator-=(const vec2 &a) {
    this->x -= a.x;
    this->y -= a.y;
}

void vec2::operator*=(double value) {
    this->x *= value;
    this->y *= value;
}

vec2 operator+(const vec2 &a, const vec2 &b) {
    return vec2(a.x + b.x, a.y + b.y);
}

vec2 operator-(const vec2 &a, const vec2 &b) {
    return vec2(a.x - b.x, a.y - b.y);
}

vec2 operator*(const vec2 &a, double value) {
    return vec2(a.x*value, a.y*value);
}

double dotProduct2(const vec2 &a, const vec2 &b) {
    return a.x * b.x + a.y * b.y;
}

double operator*(const vec2 &a, const vec2 &b) {
    return dotProduct2(a, b);
}


vec3::vec3() {
    this->x = 0;
    this->y = 0;
    this->z = 0;
}

vec3::vec3(double x, double y, double z) {
    this->x = x;
    this->y = y;
    this->z = z;
}

vec3::vec3(double angleXY, double angleZ) {
    this->x = cos(angleXY)*cos(angleZ);
    this->y = sin(angleXY)*cos(angleZ);
    this->z = sin(angleZ);
}

vec3 vec3::normalize() {
    double len = sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
    return vec3(this->x / len, this->y / len, this->z / len);
}

vec3 vec3::rotateX(double angle) {
    return vec3(this->x, this->y*cos(angle) - this->z*sin(angle), this->y*sin(angle) + this->z*cos(angle));
}

vec3 vec3::rotateY(double angle) {
    return vec3(this->x*cos(angle) + this->z*sin(angle), this->y, this->x*(-sin(angle) + this->z*cos(angle)));
}

vec3 vec3::rotateZ(double angle) {
    return vec3(this->x*cos(angle) - this->y*sin(angle), this->x*sin(angle) + this->y*cos(angle), this->z);
}

void vec3::operator+=(const vec3 &a) {
    this->x += a.x;
    this->y += a.y;
    this->z += a.z;
}

void vec3::operator-=(const vec3 &a) {
    this->x -= a.x;
    this->y -= a.y;
    this->z -= a.z;
}

void vec3::operator*=(double value) {
    this->x *= value;
    this->y *= value;
    this->z *= value;
}

vec3 operator+(const vec3 &a, const vec3 &b) {
    return vec3(a.x+b.x, a.y+b.y, a.z+b.z);
}

vec3 operator-(const vec3 &a, const vec3 &b) {
    return vec3(a.x-b.x, a.y-b.y, a.z-b.z);
}

vec3 operator*(const vec3 &a, double value) {
    return vec3(a.x*value, a.y*value, a.z*value);
}

double dotProduct3(const vec3 &a, const vec3 &b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

double operator*(const vec3 &a, const vec3 &b) {
    return dotProduct3(a, b);
}

vec3 crossProduct3(const vec3 &a, const vec3 &b) {
    return vec3(a.y*b.z - a.z-b.y, a.z*b.x - a.x*b.z, a.x*b.y - a.y*b.x);
}

vec3 operator^(const vec3 &a, const vec3 &b) {
    return crossProduct3(a, b);
}