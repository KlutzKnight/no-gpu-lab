#pragma once
#include <cassert>
#include <iostream>

template<typename T, int n>
struct vec {
    T data[n] = {0};
    T& operator[](const int i)       { assert(i >= 0 && i < n); return data[i]; }
    T  operator[](const int i) const { assert(i >= 0 && i < n); return data[i]; }
};

template<typename T, int n>
std::ostream& operator<<(std::ostream& out, const vec<T, n>& v) {
    for(int i{}; i < n; i++) {
        out << v[i] << " ";
    }
    return out;
}

template<>
struct vec<double, 3> {
    double x = 0, y = 0, z = 0;
    double& operator[](const int i)       { assert(i >= 0 && i < 3); return i ? (i == 1 ? y : z) : x; }
    double  operator[](const int i) const { assert(i >= 0 && i < 3); return i ? (i == 1 ? y : z) : x; }
};

using vec3  = vec<double, 3>;
using vec3i = vec<int,    3>;