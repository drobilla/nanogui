/*
    nanogui/matrix.h -- statically sized vector class

    Written from scratch to be a minimal replacement for Eigen vectors.

    Copyright 2016 David Robillard <http://drobilla.net>

    All rights reserved. Use of this source code is governed by a
    BSD-style license that can be found in the LICENSE.txt file.
*/

#pragma once

#include <functional>
#include <iostream>

namespace nanogui {

template<typename T, int n>
class Vector {
public:
	explicit Vector(T val=0) {
		for (int i = 0; i < n; ++i) {
			mElems[i] = val;
		}
	}

	template <typename... Args,
	          typename = typename std::enable_if<sizeof...(Args) == n>::type>
	Vector(Args&&... values)
		: mElems{ static_cast<T>(std::forward<Args>(values))... }
	{}

	static Vector<T, n> Zero()          { return Vector(0); }
	static Vector<T, n> Constant(T val) { return Vector(val); }

	static Vector<T, n> UnitZ() {
		Vector<T, n> out;
		out.get(3) = 1;
		return out;
	}

	Vector<T, n>& operator=(const Vector<T, n>& rhs) {
		for (int i = 0; i < n; ++i) {
			mElems[i] = rhs[i];
		}
		return *this;
	}

	Vector<T, n>(const Vector<T, n>& copy) = default;

	inline T  operator[](int i) const { return mElems[i]; }
	inline T& operator[](int i)       { return mElems[i]; }
	inline T  operator()(int i) const { return mElems[i]; }
	inline T& operator()(int i)       { return mElems[i]; }

	template<typename Op>
	Vector<T, n> combine(const Vector<T, n>& rhs) const {
		Vector<T, n> out;
		for (int i = 0; i < n; ++i) {
			out.mElems[i] = Op()(mElems[i], rhs[i]);
		}
		return out;
	}

	Vector<T, n> operator+(const Vector<T, n>& rhs) const { return combine< std::plus<T> >(rhs); }
	Vector<T, n> operator-(const Vector<T, n>& rhs) const { return combine< std::minus<T> >(rhs); }
	Vector<T, n> operator*(const Vector<T, n>& rhs) const { return combine< std::multiplies<T> >(rhs); }
	Vector<T, n> operator/(const Vector<T, n>& rhs) const { return combine< std::divides<T> >(rhs); }

	template<typename Op>
	Vector<T, n> combine(T val) const {
		Vector<T, n> out;
		for (int i = 0; i < n; ++i) {
			out.mElems[i] = Op()(mElems[i], val);
		}
		return out;
	}

	Vector<T, n> operator+(T val) const { return combine< std::plus<T> >(val); }
	Vector<T, n> operator-(T val) const { return combine< std::minus<T> >(val); }
	Vector<T, n> operator*(T val) const { return combine< std::multiplies<T> >(val); }
	Vector<T, n> operator/(T val) const { return combine< std::divides<T> >(val); }

	template<typename Op>
	Vector<T, n>& apply(const Vector<T, n>& rhs) {
		for (int i = 0; i < n; ++i) {
			mElems[i] = Op()(mElems[i], rhs[i]);
		}
		return *this;
	}

	Vector<T, n> operator+=(const Vector<T, n>& rhs) { return apply< std::plus<T> >(rhs); }
	Vector<T, n> operator-=(const Vector<T, n>& rhs) { return apply< std::minus<T> >(rhs); }
	Vector<T, n> operator*=(const Vector<T, n>& rhs) { return apply< std::multiplies<T> >(rhs); }
	Vector<T, n> operator/=(const Vector<T, n>& rhs) { return apply< std::divides<T> >(rhs); }

	Vector<T, n> array() const { return *this; }

	bool any() const {
		for (int i = 0; i < n; ++i) {
			if (mElems[i]) {
				return true;
			}
		}
		return false;
	}

	bool all() const {
		for (int i = 0; i < n; ++i) {
			if (!mElems[i]) {
				return false;
			}
		}
		return true;
	}

	template<typename Comp>
	Vector<T, n> compare(T val) const {
		Vector<T, n> out;
		for (int i = 0; i < n; ++i) {
			out[i] = Comp()(mElems[i], val);
		}
		return out;
	}

	Vector<T, n> operator<(T val)  const { return compare< std::less<T> >(val); }
	Vector<T, n> operator<=(T val) const { return compare< std::less_equal<T> >(val); }
	Vector<T, n> operator>(T val)  const { return compare< std::greater<T> >(val); }
	Vector<T, n> operator>=(T val) const { return compare< std::greater_equal<T> >(val); }

	template<typename Comp>
	Vector<T, n> compare(const Vector<T, n>& rhs) const {
		Vector<T, n> out;
		for (int i = 0; i < n; ++i) {
			out[i] = Comp()(mElems[i], rhs[i]);
		}
		return out;
	}

	Vector<T, n> operator<(const Vector<T, n>& rhs)  const { return compare< std::less<T> >(rhs); }
	Vector<T, n> operator<=(const Vector<T, n>& rhs) const { return compare< std::less_equal<T> >(rhs); }
	Vector<T, n> operator>(const Vector<T, n>& rhs)  const { return compare< std::greater<T> >(rhs); }
	Vector<T, n> operator>=(const Vector<T, n>& rhs) const { return compare< std::greater_equal<T> >(rhs); }

	template<const T& (*Op)(const T&, const T&)>
	Vector<T, n> binop(const Vector<T, n>& rhs) const {
		Vector<T, n> out;
		for (int i = 0; i < n; ++i) {
			out.mElems[i] = Op(mElems[i], rhs[i]);
		}
		return out;
	}

	Vector<T, n> cwiseMax(const Vector<T, n>& rhs) const { return binop< std::max<T> >(rhs); }
	Vector<T, n> cwiseMin(const Vector<T, n>& rhs) const { return binop< std::min<T> >(rhs); }
	Vector<T, n> max(const Vector<T, n>& rhs) const { return binop< std::max<T> >(rhs); }
	Vector<T, n> min(const Vector<T, n>& rhs) const { return binop< std::min<T> >(rhs); }

	Vector<T, n> cwiseQuotient(const Vector<T, n>& rhs) const { return operator/(rhs); }
	Vector<T, n> cwiseProduct(const Vector<T, n>& rhs) const { return operator*(rhs); }

	template<const T& (*Op)(const T&, const T&)>
	T reduce() const {
		T out = mElems[0];
		for (int i = 1; i < n; ++i) {
			out = Op(out, mElems[i]);
		}
		return out;
	}

	template<typename Op>
	T reduce(Op op) const {
		T out = mElems[0];
		for (int i = 1; i < n; ++i) {
			out = op(out, mElems[i]);
		}
		return out;
	}

	T minCoeff() const { return reduce< std::min<T> >(); }
	T maxCoeff() const { return reduce< std::max<T> >(); }
	T sum()      const { return reduce(std::plus<T>()); }
	T prod()     const { return reduce(std::multiplies<T>()); }

	T dot(const Vector<T, n>& rhs) const {
		T out = 0;
		for (size_t i = 0; i < n; ++i) {
			out += mElems[i] * rhs[i];
		}
		return out;
	}

	template<typename Op>
	Vector<T, n> unaryExpr(Op op) const {
		Vector<T, n> out;
		for (int i = 1; i < n; ++i) {
			out[i] = op(mElems[i]);
		}
		return out;
	}

	bool operator==(const Vector<T, n>& rhs) const {
		for (int i = 0; i < n; ++i) {
			if (mElems[i] != rhs[i]) {
				return false;
			}
		}
		return true;
	}

	bool operator!=(const Vector<T, n>& rhs) const { return !operator==(rhs); }

	Vector<T, n> operator-() const { return operator*(-1); }

	template<typename O>
	Vector<O, n> cast() const {
		Vector<O, n> o;
		for (int i = 0; i < n; ++i) {
			o[i] = static_cast<O>(mElems[i]);
		}
		return o;
	}

	template<int i, typename = typename std::enable_if<std::isgreater(n, i)>::type>
	const T& get() const { return this->mElems[i]; }

	template<int i, typename = typename std::enable_if<std::isgreater(n, i)>::type>
	T& get() { return this->mElems[i]; }

	template<int i=0> const T& x() const { return get<0>(); }
	template<int i=0> T&       x()       { return get<0>(); }
	template<int i=1> const T& y() const { return get<1>(); }
	template<int i=1> T&       y()       { return get<1>(); }
	template<int i=2> const T& z() const { return get<2>(); }
	template<int i=2> T&       z()       { return get<2>(); }
	template<int i=3> const T& w() const { return get<2>(); }
	template<int i=3> T&       w()       { return get<2>(); }

	template<int m>
	Vector<T, m> head() const {
		Vector<T, m> out;
		for (int i = 0; i < std::min(m, n); ++i) {
			out[i] = mElems[i];
		}
		return out;
	}

	const T* data() const { return mElems; }
	T*       data()       { return mElems; }

protected:
	T mElems[n];
};

template<typename T, int n>
Vector<T, n> operator*(T val, const Vector<T, n>& v) { return v * val; }

template<typename T, int n>
Vector<T, n> operator/(T val, const Vector<T, n>& v) { return v / val; }

template<typename T, int n>
std::ostream&
operator<<(std::ostream& os, const Vector<T, n>& v) {
	os << "[ ";
	for (int i = 0; i < n; ++i) {
		os << v[i] << " ";
	}
	return os << "]";
}

} // namespace nanogui
