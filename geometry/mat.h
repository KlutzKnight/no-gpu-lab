#pragma once
#include "vec.h"
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <type_traits>
#include <utility>



template<int nRows, int nColumns, typename T = double>
struct mat {
    vec<nColumns, T> rows[nRows]{{}};
    constexpr std::pair<int, int> dimensions() const { return {nRows, nColumns}; }
    constexpr       vec<nColumns, T>& operator[](const int i)       { assert(i >= 0 && i < nRows); return rows[i]; }
    constexpr const vec<nColumns, T>& operator[](const int i) const { assert(i >= 0 && i < nRows); return rows[i]; }

    T trace() const requires (nRows == nColumns) {
        T result{};
        for(int i{}; i < nRows; i++) {
            result += rows[i][i];
        }
        return result;
    }

    T cofactor(const int r, const int c) const requires (nRows == nColumns && nRows > 0) {
        assert(r >= 0 && r < nRows);
        assert(c >= 0 && c < nColumns);

        if constexpr (nRows == 1) {
            return T{1};
        } else {
            mat<nRows - 1, nColumns - 1, T> subMatrix;
            for(int i{}; i < nRows - 1; i++) {
                for(int j{}; j < nColumns - 1; j++) {
                    subMatrix[i][j] = rows[i + int(i >= r)][j + int(j >= c)];
                }
            }
            return subMatrix.determinant() * ((r + c) % 2 ? -1 : 1);
        }
    }

    T determinant() const requires (nRows == nColumns) {
        if constexpr (nRows == 0) {
            return 1;
        } else if constexpr (nRows == 1) {
            return rows[0][0];
        } else {
            T det{};

            for(int i{}; i < nColumns; i++) {
                det += rows[0][i] * cofactor(0, i);
            }
            return det;
        }
    }

    mat<nColumns, nRows, T> transpose() const {
        mat<nColumns, nRows, T> result;
        for(int i{}; i < nColumns; i++) {
            for(int j{}; j < nRows; j++) {
                result[i][j] = rows[j][i];
            }
        }
        return result;
    }

    mat<nRows, nColumns> inverse() const requires (nRows == nColumns) {
        double det = static_cast<double>(determinant());
        if(det == 0) {
            throw std::domain_error("Matrix is singular.");
        }
        mat<nRows, nColumns> cofactorMatrix;
        for(int i{}; i < nRows; i++) {
            for(int j{}; j < nColumns; j++) {
                cofactorMatrix[i][j] = static_cast<double>(cofactor(i, j));
            }
        }

        return cofactorMatrix.transpose() / det;
    }
};


// Formatted Output of Matrix
template<typename T, int nRows, int nColumns>
std::ostream& operator<<(std::ostream& out, const mat<nRows, nColumns, T>& matrix) {
    for(int i{}; i < nRows; i++) {
        out << matrix[i] << "\n";
    }
    return out;
}


// Arithmetic Addition and Subtraction
template<typename T1, typename T2, int nRows, int nColumns>
auto operator+(const mat<nRows, nColumns, T1>& m1, const mat<nRows, nColumns, T2>& m2) {
    mat<nRows, nColumns, std::common_type_t<T1, T2>> result;

    for(int i{}; i < nRows; i++) {
        result[i] = m1[i] + m2[i];
    }
    return result;
}
template<typename T1, typename T2, int nRows, int nColumns>
auto operator-(const mat<nRows, nColumns, T1>& m1, const mat<nRows, nColumns, T2>& m2) {
    mat<nRows, nColumns, std::common_type_t<T1, T2>> result;

    for(int i{}; i < nRows; i++) {
        result[i] = m1[i] - m2[i];
    }
    return result;
}

// Unary
template<typename T, int nRows, int nColumns>
auto operator+(const mat<nRows, nColumns, T>& v) {
    using ResultType = decltype(-v[0][0]);
    mat<nRows, nColumns, ResultType> result;
    for(int i{}; i < nRows; i++) {
        result[i] = +v[i];
    }
    return result;
}
template<typename T, int nRows, int nColumns>
auto operator-(const mat<nRows, nColumns, T>& v) {
    using ResultType = decltype(-v[0][0]);
    mat<nRows, nColumns, ResultType> result;
    for(int i{}; i < nRows; i++) {
        result[i] = -v[i];
    }
    return result;
}

// Compound Addition and Subtraction
template<typename T1, typename T2, int nRows, int nColumns>
mat<nRows, nColumns, T1>& operator+=(mat<nRows, nColumns, T1>& v1, const mat<nRows, nColumns, T2>& v2) {
    for(int i{}; i < nRows; i++) {
        v1[i] = v1[i] + v2[i];
    }
    return v1;
}
template<typename T1, typename T2, int nRows, int nColumns>
mat<nRows, nColumns, T1>& operator-=(mat<nRows, nColumns, T1>& v1, const mat<nRows, nColumns, T2>& v2) {
    for(int i{}; i < nRows; i++) {
        v1[i] = v1[i] - v2[i];
    }
    return v1;
}


// Multiplication and Division by Scalar
template<typename VecType, int nRows, int nColumns, typename T>
auto operator*(const mat<nRows, nColumns, VecType>& v, T scalar) {
    using ResultType = decltype(v[0][0] * scalar);
    mat<nRows, nColumns, ResultType> result;
    for(int i{}; i < nRows; i++) {
        result[i] = v[i] * scalar;
    }
    return result;
}
template<typename VecType, int nRows, int nColumns, typename T>
auto operator*(T scalar, const mat<nRows, nColumns, VecType>& v) {
    return v * scalar;
}
template<typename VecType, int nRows, int nColumns, typename T>
auto operator/(const mat<nRows, nColumns, VecType>& v, T scalar) {
    if(scalar == 0) {
        throw std::domain_error("Division by zero.");
    }
    using ResultType = decltype(v[0][0] / scalar);
    mat<nRows, nColumns, ResultType> result;
    for(int i{}; i < nRows; i++) {
        result[i] = v[i] / scalar;
    }
    return result;
}

// Compound Multiplication and Division by scalar
template<typename VecType, int nRows, int nColumns, typename T>
mat<nRows, nColumns, VecType>& operator*=(mat<nRows, nColumns, VecType>& v1, T scalar) {
    for(int i{}; i < nRows; i++) {
        v1[i] = v1[i] * scalar;
    }
    return v1;
}
template<typename VecType, int nRows, int nColumns, typename T>
mat<nRows, nColumns, VecType>& operator/=(mat<nRows, nColumns, VecType>& v1, T scalar) {
    if(scalar == 0) {
        throw std::domain_error("Division by zero.");
    }
    for(int i{}; i < nRows; i++) {
        v1[i] = v1[i] / scalar;
    }
    return v1;
}


// Matrix and Vector Multiplication
template<typename T1, typename T2, int nRows, int nColumns>
auto operator*(const mat<nRows, nColumns, T1>& m, const vec<nColumns, T2>& v) {
    using ResultType = decltype(dot(m[0], v));
    vec<nRows, ResultType> result{};
    for(int i{}; i < nRows; i++) {
        result[i] = dot(m[i], v);
    }
    return result;
}
template<typename T1, typename T2, int nRows, int nColumns>
auto operator*(const vec<nRows, T2>& v, const mat<nRows, nColumns, T1>& m) {
    auto temp = m.transpose();
    using ResultType = decltype(dot(temp[0], v));
    vec<nColumns, ResultType> result{};
    for(int i{}; i < nColumns; i++) {
        result[i] = dot(temp[i], v);
    }
    return result;
}
template<typename T1, typename T2, int nRows, int nColumns, int r>
auto operator*(const mat<nRows, r, T1>& m1, const mat<r, nColumns, T2>& m2) {
    auto transposed = m2.transpose();
    using ResultType = decltype(dot(m1[0], transposed[0]));
    mat<nRows, nColumns, ResultType> result;
    for(int i{}; i < nRows; i++) {
        for(int j{}; j < nColumns; j++) {
            result[i][j] = dot(m1[i], transposed[j]);
        }
    }
    return result;
}
