#pragma once

#include <math.h>
#include <glm/glm.hpp>
#include <iostream>

// Get string hash
constexpr unsigned int str2hash(const char* str, int h = 0) {
	return !str[h] ? 5381 : (str2hash(str, h+1) * 33) ^ str[h];
}

// Quaternion class
template<typename T>
class quaternion
{
public:
	enum qtype {
		S = 0,
		I = 1,
		J = 2,
		K = 3
	};

	T q[4];

	quaternion operator + (quaternion const &obj) {
		quaternion res;
		res.q[S] = q[S] + obj.q[S];
		res.q[I] = q[I] + obj.q[I];
		res.q[J] = q[J] + obj.q[J];
		res.q[K] = q[K] + obj.q[K];
		return res;
	};
	quaternion operator - (quaternion const &obj) {
		quaternion res;
		res.q[S] = q[S] - obj.q[S];
		res.q[I] = q[I] - obj.q[I];
		res.q[J] = q[J] - obj.q[J];
		res.q[K] = q[K] - obj.q[K];
		return res;
	};
	quaternion operator * (quaternion const& obj) {
		quaternion res;
		res.q[S] = q[S]*obj.q[S] - q[I]*obj.q[I] - q[J]*obj.q[J] - q[K]*obj.q[K];
		res.q[I] = q[I]*obj.q[S] + q[S]*obj.q[I] - q[K]*obj.q[J] + q[J]*obj.q[K];
		res.q[J] = q[J]*obj.q[S] + q[K]*obj.q[I] + q[S]*obj.q[J] - q[I]*obj.q[K];
		res.q[K] = q[K]*obj.q[S] - q[J]*obj.q[I] + q[I]*obj.q[J] + q[S]*obj.q[K];
	return res;
	};
};

namespace math {
	const float m_pi = 3.1415927410125732421875f;
	const float m_pi_2 = 1.57079637050628662109375f;
	const float m_2_pi = 6.283185482025146484375f;

	template<typename T>
	// Credits to moka: https://stackoverflow.com/questions/2878076/what-is-different-about-c-math-h-abs-compared-to-my-abs
	T abs(T val) {
		return val < 0 ? -val : val;
	}

	// Series sine function
	float sin(float val, unsigned int iterations = 5)
	{
		val = remainder(val, m_2_pi);
		bool sign = 0;
		if (val >= m_pi_2) {
			val -= m_pi;
			sign = 1;
		}
		else if (val <= -m_pi_2) {
			val += m_pi;
			sign = 1;
		}
	
		float seno = 0;
		unsigned int fact = 1;
		for (unsigned int i = 0; i < iterations; i++) {
			fact = fact * (2 * i + 1);
			seno = seno + pow(-1, i)*pow(val, (2 * i + 1)) / fact;
			fact = fact * (2 * i + 2);
		}
		if (sign)
			seno = -seno;

		return seno;
	}

	float cos(float val, unsigned int iterations = 5)
	{
		val = remainder(val, m_2_pi);
		bool sign = 0;
		if (val >= m_pi_2) {
			val -= m_pi;
			sign = 1;
		}
		else if (val <= -m_pi_2) {
			val += m_pi;
			sign = 1;
		}
	
		float cos = 1;
		unsigned int fact = 1;
		for (unsigned int i = 1; i < iterations; i++) {
			fact = fact * (2 * i);
			cos = cos + pow(-1, i)*pow(val, (2 * i)) / fact;
			fact = fact * (2 * i + 1);
		}
		if (sign)
			cos = -cos;

		return cos;
	}

	template<typename T>
	T determinant(T* matrix, int n0) {
		T det = matrix[0] - matrix[0];
		T* submatrix = NULL;
		submatrix = new T[n0*n0];
		if (n0 == 1)
			det = matrix[0];
		if (n0 == 2)
			det = ((matrix[0] * matrix[3]) - (matrix[1] * matrix[2]));
		else {
			for (int x = 0; x < n0; x++) {
				int subi = 0;
				for (int i = 1; i < n0; i++) {
					int subj = 0;
					for (int j = 0; j < n0; j++) {
						if (j == x)
							continue;
						submatrix[subj+subi*(n0-1)] = matrix[j+i*n0];
						subj++;
					}
					subi++;
				}
				det = det + (matrix[x] * determinant(submatrix, n0 - 1)) * pow(-1, x);
			}
		}
		delete[] submatrix;
		submatrix = NULL;
		return det;
	}
	
	template<typename T>
	T* inverse(T* matrix, T* result, int n0){
	
		T one_temp = 1.0f;
		T zero_temp = 0.0f;
	
		T det_m = determinant(matrix, n0);
	
		if (det_m == zero_temp)
			return -1;
	
		T* submatrix = NULL;
		submatrix = new T[n0 * n0];
		T* cofmatrix = NULL;
		cofmatrix = new T[n0 * n0];
	
	
		for (int x = 0; x < n0; x++) {
			for (int y = 0; y < n0; y++) {
				int subi = 0;
				for (int i = 0; i < n0; i++) {
					int subj = 0;
					if(i == x)
						continue;
					for (int j = 0; j < n0; j++) {
						if (j == y)
							continue;
						submatrix[subj + subi * (n0 - 1)] = matrix[i + j * n0];
						subj++;
					}
					subi++;
				}
				cofmatrix[y + x * n0] = ((one_temp /det_m) * determinant(submatrix, n0-1)) * pow(-1, x + y);
			}
		}
	
		for (short i = 0; i < n0 * n0; i++) {
			result[i] = cofmatrix[i];
		}
	
		delete[] submatrix;
		submatrix = NULL;
		delete[] cofmatrix;
		cofmatrix = NULL;
		
		return result;
	}
	
	template<typename T>
	T* mat_mult(T* A, T* B, T* result, int n0, int n1, int n2) 
	// Result[n0][n2]=A[n0][n1]*B[n1][n2]
	{
		T* submatrix = NULL;
		submatrix = new T[n0 * n2];
	
		for (char i = 0; i < n0; ++i)
		{
			for (char j = 0; j < n2; ++j)
			{
				for (char k = 0; k < n1; ++k)
				{
					if (k == 0)
						submatrix[j + i * n2] = A[k + i * n1] * B[j + k * n2];
					else
						submatrix[j + i * n2] = submatrix[j + i * n2] + A[k + i * n1] * B[j + k * n2];
				}
			}
		}
		for (short i = 0; i < n0 * n2; i++) {
			result[i] = submatrix[i];
		}
		delete[] submatrix;
		submatrix = NULL;
	
		return result;
	}
	
	template<typename T>
	T* transpose(T* matrix, T* result, int n0, int n1)
	//Result[N1][N0] = A[N0][N1];
	{
		T* submatrix = NULL;
		submatrix = new T[n0 * n1];
	
		for (int x = 0; x < n0; x++) {
			for (int y = 0; y < n1; y++) {
				submatrix[x + y * n0] = matrix[y + x * n1];
			}
		}
	
		for (short i = 0; i < n0 * n1; i++) {
			result[i] = submatrix[i];
		}
	
		delete[] submatrix;
		submatrix = NULL;
	
		return result;
	}

	/*glm::mat4 rotate(glm::mat4* basemat, glm::vec3* axis, float rotation)
	{
		glm::vec4 basevector;
		//TODO :: implement custom trigonometric functions for less iterations on cos and sin
		quaternion<float> q1;
		quaternion<float> q2;
		quaternion<float> p;
	
		//Normalize
		{
			if (glm::length(*axis) != 1.0f)
				(*axis) = (*axis)*(1.0f / (axis->length()));
		}
	
		//Set quartenions
		{
			float cosV = cos(rotation / 2);
			float senV = sin(rotation / 2);
	
			q1.q[q1.S] = cosV;
			q1.q[q1.I] = senV * axis->x;
			q1.q[q1.J] = senV * axis->y;
			q1.q[q1.K] = senV * axis->z;
	
			q2.q[q2.S] = cosV;
			q2.q[q2.I] = -senV * axis->x;
			q2.q[q2.J] = -senV * axis->y;
			q2.q[q2.K] = -senV * axis->z;
	
			p.q[p.S] = 0;
			p.q[p.I] = basevector.x;
			p.q[p.J] = basevector.y;
			p.q[p.K] = basevector.z;
		}
	
		p = q1 * p;
		p = p * q2;
		//p = quartenionmult(&q1, &p);
		//p = quartenionmult(&p, &q2);
	
		//Reassign values
		{
			basevector->x = p.q[p.I];
			basevector->y = p.q[p.J];
			basevector->z = p.q[p.K];
		}
	
		return *basevector;
	}*/
}

template<typename T>
class cplx {
public:
	T imag;
	T real;

	float abs() {
		return std::sqrt(real*real+imag*imag);
	}
	//float angle();//TODO

	cplx operator + (cplx const &obj) {
		cplx res;
		res.real = real + obj.real;
		res.imag = imag + obj.imag;
		return res;
	};
	cplx operator - (cplx const& obj) {
		cplx res;
		res.real = real - obj.real;
		res.imag = imag - obj.imag;
		return res;
	};
	cplx operator * (cplx const& obj) {
		cplx res;
		res.real = real * obj.real - imag * obj.imag;
		res.imag = real * obj.imag + imag * obj.real;
		return res;
	};
	cplx operator * (int const& obj) {
		cplx res;
		res.real = real * obj;
		res.imag = imag * obj;
		return res;
	};
	cplx operator * (float const& obj) {
		cplx res;
		res.real = real * obj;
		res.imag = imag * obj;
		return res;
	};
	cplx operator * (double const& obj) {
		cplx res;
		res.real = real * obj;
		res.imag = imag * obj;
		return res;
	};
	cplx operator / (cplx const& obj) {
		cplx res;
		res.real = (real * obj.real + imag * obj.imag) / (obj.real * obj.real + obj.imag * obj.imag);
		res.imag = (- real * obj.imag + imag * obj.real) / (obj.real * obj.real + obj.imag * obj.imag);
		return res;
	};
	bool operator == (cplx const& obj) {
		if ((obj.real == real) && (obj.imag == imag))
			return true;
		else
			return false;
	};
	float& operator[] (int x){
		imag[x] = real[x];
		return real[x];
	}

	cplx<T> operator [] (int idx) const
	{
		cplx<T> ret;
		ret.real = real[idx];
		ret.imag = imag[idx];
		return ret;
	}
	void operator = (float const& obj) {
		real = obj;
		imag = obj;
	}

	cplx();
	cplx(T x) {
		real = x;
		imag = x - x;
	}
};
