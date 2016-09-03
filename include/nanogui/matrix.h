/*
    nanogui/matrix.h -- statically sized matrix class

    Written from scratch to be a minimal replacement for Eigen matrices.

    Copyright 2016 David Robillard <http://drobilla.net>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#pragma once

#include <nanogui/vector.h>

#include <algorithm>

namespace nanogui {

template<typename T, int R, int C>
class Matrix {
public:
	typedef T            Scalar;
	typedef Vector<T, C> Row;

	Matrix() {}

	template<typename... Args,
	         typename = typename std::enable_if<sizeof...(Args) == R * C>::type>
	Matrix(Args&&... args) {
		set<0>(args...);
	}

	T operator()(int r, int c) {
		return mRows[r][c];
	}

	void setZero() {
		for (int r = 0; r < R; ++r) {
			for (int c = 0; c < C; ++c) {
				mRows[r][c] = 0;
			}
		}
	}

	void setIdentity() {
		setZero();
		for (int i = 0; i < std::min(R, C); ++i) {
			mRows[i][i] = 1;
		}
	}

	template<int OR, int OC>
	Matrix<T, OR, OC> topLeftCorner() const {
		Matrix<T, OR, OC> out;
		for (int r = 0; r < std::min(R, OR); ++r) {
			for (int c = 0; c < std::min(C, OC); ++c) {
				out[r][c] = mRows[r][c];
			}
		}
		return out;
	}

	template<int D>
	void setColumn(int col, const Vector<T, D>& vals) {
		for (int r = 0; r < std::min(R, D); ++r) {
			mRows[r][col] = vals[r];
		}
	}

	template<typename O>
	Matrix<O, R, C> cast() const {
		Matrix<O, R, C> o;
		for (int r = 0; r < R; ++r) {
			for (int c = 0; c < C; ++c) {
				o.mRows[r][c] = static_cast<O>(mRows[r][c]);
			}
		}
		return o;
	}

	Matrix<T, R, C> operator*(T factor) const {
		Matrix<T, R, C> o;
		for (int r = 0; r < R; ++r) {
			for (int c = 0; c < C; ++c) {
				o.mRows[r][c] = mRows[r][c] * factor;
			}
		}
		return o;
	}

	template<int P>
	Matrix<T, R, P> operator*(const Matrix<T, C, P>& rhs) {
		Matrix<T, R, P> out;
		for (int i = 0; i < R; ++i) {
            for (int j = 0; j < P; ++j) {
                for (int k = 0; k < C; ++k) {
                    out[i][j] += mRows[i][k] * rhs[k][j];
                }
            }
		}
		return out;
	}

	template<int i, typename = typename std::enable_if<std::isgreater(R, i)>::type>
	const Row& row() const { return this->mRows[i]; }

	Vector<T, R> col(int c) const {
		Vector<T, R> out;
		for (int r = 0; r < R; ++r) {
			out[r] = mRows[r][c];
		}
		return out;
	}

	const T* data() const { return mRows[0].data(); }

	inline const Row& operator[](int i) const { return mRows[i]; }
	inline Row&       operator[](int i)       { return mRows[i]; }

	int size() const { return R * C; }
	int rows() const { return R; }
	int cols() const { return C; }

protected:
	template<int i, typename Arg, typename... Rest>
	void set(const Arg& first, Rest&&... rest) {
		set<i+1>(rest...);
		mRows[i / C][i % C] = first;
	}

	template<int i, typename Arg>
	void set(const Arg& first) {
		mRows[i / C][i % C] = first;
	}

	Row mRows[R];
};

template<typename T, int R, int C>
std::ostream&
operator<<(std::ostream& os, const Matrix<T, R, C>& m) {
	os << "[" << std::endl;
	for (int r = 0; r < R; ++r) {
		os << m[r] << std::endl;
	}
	return os << "]";
}

} // namespace nanogui
