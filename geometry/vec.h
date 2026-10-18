#pragma once
#include <cassert>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <type_traits>

template<int n, typename T = double>
struct vec {
    T data[n]{};
    constexpr int dimensions() const { return n; }
    constexpr       T& operator[](const int i)       { assert(i >= 0 && i < n); return data[i]; }
    constexpr const T& operator[](const int i) const { assert(i >= 0 && i < n); return data[i]; }
    vec<2, T> xy()  const requires (n > 2) { return {data[0], data[1]}; }
    vec<3, T> xyz() const requires (n > 3) { return {data[0], data[1], data[2]}; }
};

// Formatted output of vector
template<typename T, int n>
std::ostream& operator<<(std::ostream& out, const vec<n, T>& v) {
    for(int i{}; i < n; i++) {
        out << v[i] << " ";
    }
    return out;
}

// Arithmetic Addition and Subtraction
template<typename T1, typename T2, int n>
auto operator+(const vec<n, T1>& v1, const vec<n, T2>& v2) {
    vec<n, std::common_type_t<T1, T2>> result;

    for(int i{}; i < n; i++) {
        result[i] = v1[i] + v2[i];
    }
    return result;
}
template<typename T1, typename T2, int n>
auto operator-(const vec<n, T1>& v1, const vec<n, T2>& v2) {
    vec<n, std::common_type_t<T1, T2>> result;

    for(int i{}; i < n; i++) {
        result[i] = v1[i] - v2[i];
    }
    return result;
}
// Unary
template<typename T, int n>
auto operator+(const vec<n, T>& v) {
    using ResultType = decltype(-v[0]);
    vec<n, ResultType> result;
    for(int i{}; i < n; i++) {
        result[i] = +v[i];
    }
    return result;
}
template<typename T, int n>
auto operator-(const vec<n, T>& v) {
    using ResultType = decltype(-v[0]);
    vec<n, ResultType> result;
    for(int i{}; i < n; i++) {
        result[i] = -v[i];
    }
    return result;
}
// Compound Addition and Subtraction
template<typename T1, typename T2, int n>
vec<n, T1>& operator+=(vec<n, T1>& v1, const vec<n, T2>& v2) {
    for(int i{}; i < n; i++) {
        v1[i] = v1[i] + v2[i];
    }
    return v1;
}
template<typename T1, typename T2, int n>
vec<n, T1>& operator-=(vec<n, T1>& v1, const vec<n, T2>& v2) {
    for(int i{}; i < n; i++) {
        v1[i] = v1[i] - v2[i];
    }
    return v1;
}

// Multiplication and Division by Scalar 
template<typename VecType, int n, typename T>
auto operator*(const vec<n, VecType>& v, T scalar) {
    using ResultType = decltype(v[0] * scalar);
    vec<n, ResultType> result;
    for(int i{}; i < n; i++) {
        result[i] = v[i] * scalar;
    }
    return result;
}
template<typename VecType, int n, typename T>
auto operator*(T scalar, const vec<n, VecType>& v) {
    return v * scalar;
}
template<typename VecType, int n, typename T>
auto operator/(const vec<n, VecType>& v, T scalar) {
    if(scalar == 0) {
        throw std::domain_error("Division by zero.");
    }
    using ResultType = decltype(v[0] / scalar);
    vec<n, ResultType> result;
    for(int i{}; i < n; i++) {
        result[i] = v[i] / scalar;
    }
    return result;
}

// Compound Multiplication and Division by scalar
template<typename VecType, int n, typename T>
vec<n, VecType>& operator*=(vec<n, VecType>& v1, T scalar) {
    for(int i{}; i < n; i++) {
        v1[i] = v1[i] * scalar;
    }
    return v1;
}
template<typename VecType, int n, typename T>
vec<n, VecType>& operator/=(vec<n, VecType>& v1, T scalar) {
    if(scalar == 0) {
        throw std::domain_error("Division by zero.");
    }
    for(int i{}; i < n; i++) {
        v1[i] = v1[i] / scalar;
    }
    return v1;
}

// Dot and Cross(3D) Product
template<typename T1, typename T2, int n>
auto dot(const vec<n, T1>& v1, const vec<n, T2>& v2) {
    using ResultType = decltype(v1[0] * v2[0]);
    ResultType result{};
    for(int i{}; i < n; i++) {
        result += v1[i] * v2[i];
    }
    return result;
}
template<typename T1, typename T2>
auto cross(const vec<3, T1>& v1, const vec<3, T2>& v2) {
    using ResultType = decltype(
        v1[1] * v2[2] - v1[2] * v2[1]
    );
    vec<3, ResultType> result;

    result[0] = v1[1] * v2[2] - v1[2] * v2[1];
    result[1] = v1[2] * v2[0] - v1[0] * v2[2];
    result[2] = v1[0] * v2[1] - v1[1] * v2[0];

    return result;
}

// Normalization functions
template<typename T, int n>
double norm(const vec<n, T>& v) {
    double magnitude{};
    for(int i{}; i < n; i++) {
        magnitude = std::hypot(magnitude, static_cast<double>(v[i]));
    }
    return magnitude;
}
template<typename T, int n>
auto normalize(const vec<n, T>& v) {
    const double magnitude = norm(v);
    if(magnitude == 0) {
        throw std::domain_error("Can't normalize a zero vector.");
    }
    return v / magnitude;
}

template<>
struct vec<2, double> {
    double x = 0, y = 0;
    constexpr int dimensions() const { return 2;}
    constexpr double& operator[](const int i)       { assert(i >= 0 && i < 2); return i ? y : x; }
    constexpr double  operator[](const int i) const { assert(i >= 0 && i < 2); return i ? y : x; }
};

template<>
struct vec<3, double> {
    double x = 0, y = 0, z = 0;
    constexpr int dimensions() const { return 3;}
    constexpr double& operator[](const int i) {
        assert(i >= 0 && i < 3);
        switch(i) {
            case 0:  return x;
            case 1:  return y;
            default: return z;
        }
    }
    constexpr double operator[](const int i) const {
        assert(i >= 0 && i < 3);
        switch(i) {
            case 0:  return x;
            case 1:  return y;
            default: return z;
        }
    }
    vec<2> xy() const { return {x, y}; }
};

template<>
struct vec<4, double> {
    double x = 0, y = 0, z = 0, w = 0;
    constexpr int dimensions() const { return 4;}
    constexpr double& operator[](const int i) {
        assert(i >= 0 && i < 4);
        switch(i) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: return w;
        }
    }
    constexpr double operator[](const int i) const {
        assert(i >= 0 && i < 4);
        switch(i) {
            case 0: return x;
            case 1: return y;
            case 2: return z;
            default: return w;
        }
    }
    vec<2> xy()  const { return {x, y}; }
    vec<3> xyz() const { return {x, y, z}; }
};

using vec2  = vec<2>;
using vec3  = vec<3>;
using vec4  = vec<4>;
using vec3i = vec<3, int>;