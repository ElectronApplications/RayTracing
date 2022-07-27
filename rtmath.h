#ifndef RTMATH_H
#define RTMATH_H

#include <utility>

class vec2 {
    public:
        double x;
        double y;

        vec2();
        vec2(double x, double y);
        vec2(double angle);
        vec2 normalize();
        vec2 rotate(double angle);
        void operator+=(const vec2 &a);
        void operator-=(const vec2 &a);
        void operator*=(double value);
};

vec2 operator+(const vec2 &a, const vec2 &b);
vec2 operator-(const vec2 &a, const vec2 &b);
vec2 operator*(const vec2 &a, double value);
double dotProduct2(const vec2 &a, const vec2 &b);
double operator*(const vec2 &a, const vec2 &b);

class vec3 {
    public:
        double x;
        double y;
        double z;

        vec3();
        vec3(double x, double y, double z);
        vec3(double angleXY, double angleZ);
        vec3 normalize();
        vec3 rotateX(double angle);
        vec3 rotateY(double angle);
        vec3 rotateZ(double angle);
        void operator+=(const vec3 &a);
        void operator-=(const vec3 &a);
        void operator*=(double value);
};

vec3 operator+(const vec3 &a, const vec3 &b);
vec3 operator-(const vec3 &a, const vec3 &b);
vec3 operator*(const vec3 &a, double value);
double dotProduct3(const vec3 &a, const vec3 &b);
double operator*(const vec3 &a, const vec3 &b);
vec3 crossProduct3(const vec3 &a, const vec3 &b);
vec3 operator^(const vec3 &a, const vec3 &b);

#endif