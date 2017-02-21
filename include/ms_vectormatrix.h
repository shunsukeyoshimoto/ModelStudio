/* vectorMatrix.h
 *
 * Copyright (c) 2013, Shunsuke YOSHIMOTO.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once;

#include <math.h>
#include <iostream>
#include <fstream>
#include <string>

/****************************************************************************
** Mathematic Calculation Method
****************************************************************************/
namespace ModelStudio{

#ifndef M_PI
	#define M_PI 3.141592653589793238
#endif

	template<class T> static inline T Rad2Deg(T rad) { return ( (rad)*(180.0/M_PI) ); }
	template<class T> static inline T Deg2Rad(T deg) { return ( (deg)*(M_PI/180.0) ); }

	template<class T>T lu(int n, T *a, int *ip)
	{
		int i, j, k, ii, ik;
		T t, u, det;
		T *weight = new T[n];

		det = 0;
		for (k = 0; k < n; k++) {
			ip[k] = k;
			u = 0;  
			for (j = 0; j < n; j++) {
				t = fabs(a[n*k+j]);  if (t > u) u = t;
			}
			if (u == 0) goto ENDPOINT;
			weight[k] = 1 / u;
		}
		det = 1;
		for (k = 0; k < n; k++) {
			u = -1;
			for (i = k; i < n; i++) {
				ii = ip[i]; 
				t = fabs(a[n*ii+k]) * weight[ii];
				if (t > u) {  u = t;  j = i;  }
			}
			ik = ip[j];
			if (j != k) {
				ip[j] = ip[k];  ip[k] = ik;
				det = -det;
			}
			u = a[n*ik+k];  det *= u;
			if (u == 0) goto ENDPOINT;
			for (i = k + 1; i < n; i++) {
				ii = ip[i];
				t = (a[n*ii+k] /= u);
				for (j = k + 1; j < n; j++)
					a[n*ii+j] -= t * a[n*ik+j];
			}
		}
	ENDPOINT:
		delete []weight;
		return det;
	}
	template<class T>T Inverse_LU(T *a_inv, T *a,int n)
	{
		int i, j, k, ii;
		T t, det;
		int *ip = new int[n];
		det = lu(n, a, ip);
		if (det != 0)
			for (k = 0; k < n; k++) {
				for (i = 0; i < n; i++) {
					ii = ip[i];  t = (ii == k);
					for (j = 0; j < i; j++)
						t -= a[n*ii+j] * a_inv[n*j+k];
					a_inv[n*i+k] = t;
				}
				for (i = n - 1; i >= 0; i--) {
					t = a_inv[n*i+k];  ii = ip[i];
					for (j = i + 1; j < n; j++)
						t -= a[n*ii+j] * a_inv[n*j+k];
					a_inv[n*i+k] = t / a[n*ii+i];
				}
			}
		delete []ip;
		return det;
	}
}

namespace ModelStudio{
/****************************************************************************
** 2D Vector Class
****************************************************************************/
	template <class T>
	class Vector2{

	public:
		union { struct { T x, y; }; T X[2];};

		Vector2():x(0),y(0){}
		Vector2(T _x, T _y):x(_x),y(_y){}
		Vector2(T _X[2]):x(_X[0]),y(_X[1]){}

		Vector2 &operator=(const Vector2 &v){this->x=v.x;this->y=v.y;return(*this);}
		Vector2 &operator=(const T v){this->x=v;this->y=v;return(*this);}
		Vector2 &operator+=(const Vector2 &u){x+=u.x;y+=u.y;return(*this);}
		Vector2 &operator-=(const Vector2 &u){x-=u.x;y-=u.y;return(*this);}
		Vector2 &operator*=(const Vector2 &u){x*=u.x;y*=u.y;return(*this);}
		Vector2 &operator*=(const T a){x*=a;y*=a;return(*this);}
		Vector2 &operator/=(const T a){x/=a;y/=a;return(*this);}
		bool operator==(const Vector2 &u){return((x==u.x)&&(y==u.y));}
		bool operator!=(const Vector2 &u){return((x!=u.x)||(y!=u.y));}

		Vector2 operator +(){return *this;}
		Vector2 operator -(){return Vector2(-x,-y);}
		T &operator[](const int _elem){return X[_elem];}
		T &operator()(const int _elem){return X[_elem];}
		T abs(){T result=0;for(int i=0;i<2;i++)result+=X[i]*X[i];return sqrt((double)result);}

		template <class T>friend Vector2 operator *(const Vector2<T> &u, const T a){Vector2<T> result(u);result*=a;return result;}
		template <class T>friend Vector2 operator *(const T a, const Vector2<T> &u){Vector2<T> result(u);result*=a;return result;}
		template <class T>friend Vector2 operator +(const Vector2<T> &u, const Vector2<T> &v){Vector2 result(u);result+=v;return result;}
		template <class T>friend Vector2 operator -(const Vector2<T> &u, const Vector2<T> &v){Vector2 result(u);result-=v;return result;}
		template <class T>friend Vector2 operator /(const Vector2<T> &u, const T a){Vector2 result(u);result/=a;return result;}
	};
	template <class T>T operator *(const Vector2<T> &u, const Vector2<T> &v){T result=u.x*v.x+u.y*v.y; return result;}
	template <class T>std::ostream& operator<<(std::ostream &stream, Vector2<T> &v){return stream <<v.x<<","<<v.y;}
}

/****************************************************************************
** 3D double Vector Class
****************************************************************************/
namespace ModelStudio{
	template <class T>
	class Vector3{
	public:
		union { struct { T x, y, z; }; T X[3];};
		Vector3():x(0),y(0),z(0){}
		Vector3(T _x, T _y, T _z):x(_x),y(_y),z(_z){}
		Vector3(T _X[3]):x(_X[0]),y(_X[1]),z(_X[2]){}
		Vector3 &operator=(T v){x=v;y=v;z=v;return(*this);}
		Vector3 &operator=(const Vector3 &v){x=v.x;y=v.y;z=v.z;return(*this);}

		Vector3 &operator+=(const Vector3 &u){x+=u.x;y+=u.y;z+=u.z;return(*this);}
		Vector3 &operator-=(const Vector3 &u){x-=u.x;y-=u.y;z-=u.z;return(*this);}
		Vector3 &operator*=(const Vector3 &u){x*=u.x;y*=u.y;z*=u.z;return(*this);}
		Vector3 &operator*=(T a){x*=a;y*=a;z*=a;return(*this);}
		Vector3 &operator/=(T a){x/=a;y/=a;z/=a;return(*this);}
		bool operator==(const Vector3 &u){return((x==u.x)&&(y==u.y)&&(z==u.z));}
		bool operator!=(const Vector3 &u){return((x!=u.x)||(y!=u.y)||(z!=u.z));}
		bool operator!=(const T &u){return((x!=u)||(y!=u)||(z!=u));}
		bool operator<(const Vector3 &u){return((x<u.x)&&(y<u.y)&&(z<u.z));}
		bool operator>(const Vector3 &u){return((x>u.x)&&(y>u.y)&&(z>u.z));}
		bool operator<(const T &u){return(x<u)&&(y<u)&&(z<u);}
		bool operator>(const T &u){return(x>u)&&(y>u)&&(z>u);}
		bool operator<=(const T &u){return(x<=u)&&(y<=u)&&(z<=u);}
		bool operator>=(const T &u){return(x>=u)&&(y>=u)&&(z>=u);}

		Vector3 operator +(){return *this;}
		Vector3 operator -(){return Vector3(-x,-y,-z);}
		T &operator[](int _elem){return X[_elem];}
		T &operator()(int _elem){return X[_elem];}
		T abs(){T result=0;for(int i=0;i<3;i++)result+=X[i]*X[i];return sqrt((double)result);}

		template <class T>friend Vector3 operator *(const Vector3<T> &u, const T a){Vector3<T> result(u);result*=a;return result;}
		template <class T>friend Vector3 operator *(const T a, const Vector3<T> &u){Vector3<T> result(u);result*=a;return result;}
		template <class T>friend Vector3 operator +(const Vector3<T> &u, const Vector3<T> &v){Vector3 result(u);result+=v;return result;}
		template <class T>friend Vector3 operator -(const Vector3<T> &u, const Vector3<T> &v){Vector3 result(u);result-=v;return result;}
		template <class T>friend Vector3 operator /(const Vector3<T> &u, const T a){Vector3 result(u);result/=a;return result;}
		template <class T>friend Vector3 operator %(const Vector3<T> &u, const Vector3<T> &v){return Vector3(u.y*v.z-u.z*v.y,u.z*v.x-u.x*v.z,u.x*v.y-u.y*v.x);}
	};
	template <class T>T operator *(const Vector3<T> &u, const Vector3<T> &v){T result; result=u.x*v.x+u.y*v.y+u.z*v.z; return result;}
	template <class T> std::ostream &operator<<(std::ostream &stream, const Vector3<T> &v){return stream <<v.x<<","<<v.y<<","<<v.z;}
}


/****************************************************************************
** 4D double Vector Class
****************************************************************************/
namespace ModelStudio{
	template <class T>
	class Vector4{
	public:
		union { struct { T x, y, z, w; }; T X[4];};

		Vector4():x(0),y(0),z(0),w(0){}
		Vector4(T _x, T _y, T _z, T _w):x(_x),y(_y),z(_z),w(_w){}
		Vector4(T _X[4]):x(_X[0]),y(_X[1]),z(_X[2]),w(_X[3]){}

		Vector4 &operator=(const Vector4 &v){this->x=v.x;this->y=v.y;this->z=v.z;this->w=v.w;return(*this);}
		Vector4 &operator=(const T v){this->x=v;this->y=v;this->z=v;this->w=v;return(*this);}
		Vector4 &operator+=(const Vector4 &u){x+=u.x;y+=u.y;z+=u.z;w+=u.w;return(*this);}
		Vector4 &operator-=(const Vector4 &u){x-=u.x;y-=u.y;z-=u.z;w-=u.w;return(*this);}
		Vector4 &operator*=(const Vector4 &u){x*=u.x;y*=u.y;z*=u.z;w*=u.w;return(*this);}
		Vector4 &operator*=(T a){x*=a;y*=a;z*=a;w*=a;return(*this);}
		Vector4 &operator/=(T a){x/=a;y/=a;z/=a;w/=a;return(*this);}
		const bool operator==(const Vector4 &u){return((x==u.x)&&(y==u.y)&&(z==u.z)&&(w==u.w));}
		const bool operator!=(const Vector4 &u){return((x!=u.x)||(y!=u.y)||(z!=u.z)||(w!=u.w));}

		Vector4 operator +(){return *this;}
		Vector4 operator -(){return Vector4(-x,-y,-z,-w);}
		T &operator[](int _elem){return X[_elem];}
		T &operator()(int _elem){return X[_elem];}
		T abs(){double result=0;for(int i=0;i<4;i++)result+=X[i]*X[i];return sqrt((double)result);}

		template <class T>friend Vector4 operator *(Vector4<T> &u, T a){Vector4 result(u);result*=a;return result;}
		template <class T>friend Vector4 operator *(T a, Vector4<T> &u){Vector4 result(u);result*=a;return result;}
		template <class T>friend Vector4 operator +(Vector4<T> &u, Vector4<T> &v){Vector4 result(u);result+=v;return result;}
		template <class T>friend Vector4 operator -(Vector4<T> &u, Vector4<T> &v){Vector4 result(u);result-=v;return result;}
		template <class T>friend Vector4 operator /(Vector4<T> &u, T a){Vector4 result(u);result/=a;return result;}
	};
	template <class T>T operator *(Vector4<T> &u, Vector4<T> &v){T result; result=u.x*v.x+u.y*v.y+u.z*v.z; return result;}
	template <class T>std::ostream& operator<<(std::ostream& stream, Vector4<T> &v){return stream <<v.x<<","<<v.y<<","<<v.z<<","<<v.w;}
}

/****************************************************************************
** ND double Vector Class
****************************************************************************/
namespace ModelStudio{
	template <class T>
	class VectorN{
	public:
		int n;
		T *X;
		VectorN():n(0),X(0){}
		VectorN(int _n):n(_n){X=new T[n];for(int i=0;i<n;i++)X[i]=0;}
		VectorN(const VectorN &u){n=u.n;X=new T[n];for(int i=0;i<n;i++)X[i]=u.X[i];}
		~VectorN(){if(n>1)delete []X;n=0;}

		VectorN &operator=(const VectorN &u){
			if(&u==this)return *this;
			if(n==u.n){for(int i=0;i<n;i++)X[i]=u.X[i];return *this;}
			else {if(n>1)delete []X;n=u.n;X=new T[n];
			for(int i=0;i<n;i++)X[i]=u.X[i];}
			return *this;
		}
		VectorN &operator=(const Vector3<T> &u){
			if(n>=3){for(int i=0;i<3;i++)X[i]=u.X[i];return *this;}
			else return *this;
		}
		VectorN &operator=(const T u){
			for(int i=0;i<n;i++)X[i]=u;return *this;
		}
		VectorN &operator+=(const VectorN &u){if(n==u.n){for(int i=0;i<n;i++)X[i]+=u.X[i];}return(*this);}
		VectorN &operator-=(const VectorN &u){if(n==u.n){for(int i=0;i<n;i++)X[i]-=u.X[i];}return(*this);}
		VectorN &operator*=(T a){for(int i=0;i<n;i++)X[i]*=a;return(*this);}
		VectorN &operator/=(T a){for(int i=0;i<n;i++)X[i]/=a;return(*this);}
		bool operator==(const VectorN &u){if(n!=u.n)return false;for(int i=0;i<n;i++){if(X[i]!=u.X[i])return false;}return true;}
		bool operator!=(const VectorN &u){if(n!=u.n)return true;for(int i=0;i<n;i++){if(X[i]!=u.X[i])return true;}return false;}
		VectorN &operator +(){return(*this);}
		VectorN &operator -(){for(int i=0;i<n;i++)X[i]=-X[i];return(*this);}
		T &operator[](int _elem){return X[_elem];}
		T &operator()(int _elem){return X[_elem];}
		T abs(){
			T result=0;
			for(int i=0;i<n;i++)result+=X[i]*X[i];
			return sqrt((double)result);
		}
		void malloc(int _n){
			n=_n; X=new T[n];for(int i=0;i<n;i++)X[i]=0;
		}
		void free(){
			if(n>1)delete []X;
			this->n=0;
		}

		template <class T>friend VectorN operator *(VectorN<T> &u, T a){VectorN result(u);result*=a;return result;}
		template <class T>friend VectorN operator *(T a, VectorN<T> &u){VectorN<T> result(u);result*=a;return result;}
		template <class T>friend VectorN operator +(VectorN<T> &u, VectorN<T> &v){VectorN result(u);result+=v;return result;}
		template <class T>friend VectorN operator -(VectorN<T> &u, VectorN<T> &v){VectorN result(u);result-=v;return result;}
		template <class T>friend VectorN operator /(VectorN<T> &u, T a){VectorN result(u);result/=a;return result;}
		template <class T>friend VectorN operator %(VectorN<T> &u, VectorN<T> &v){
			VectorN result(3);
			if(u.n!=3)return u;
			result.X[0]=u.X[1]*v.X[2]-u.X[2]*v.X[1];
			result.X[1]=u.X[2]*v.X[0]-u.X[0]*v.X[2];
			result.X[2]=u.X[0]*v.X[1]-u.X[1]*v.X[0];
			return result;
		}
	};
	template <class T>T operator *(VectorN<T> &u, VectorN<T> &v){T result=0; for(int i=0;i<u.n;i++)result+=u.X[i]*v.X[i]; return result;}
	template <class T>std::ostream& operator<<(std::ostream& stream, VectorN<T> &v){for(int i=0;i<u.n-1;i++)stream<<v.X[i]<<",";stream<<v.X[u.n-1];}
}

namespace ModelStudio{
	typedef	Vector2<int> Vector2i;
	typedef	Vector2<double> Vector2d;
	typedef	Vector2<float> Vector2f;

	typedef	Vector3<int> Vector3i;
	typedef	Vector3<double> Vector3d;
	typedef	Vector3<float> Vector3f;

	typedef	Vector4<int> Vector4i;
	typedef	Vector4<double> Vector4d;
	typedef	Vector4<float> Vector4f;

	typedef	VectorN<int> VectorNi;
	typedef	VectorN<double> VectorNd;
	typedef	VectorN<float> VectorNf;
}

/****************************************************************************
** N*M dobule Matrix Class
****************************************************************************/
namespace ModelStudio{
	template<class T>
	class Matrix{
	public:
		int n;
		int m;
		T *X;

		Matrix():n(0),m(0),X(0){}
		Matrix(int _n,int _m):n(_n),m(_m){X=new T[n*m];for(int i=0;i<n*m;i++)X[i]=0;}
		Matrix(const Matrix &u){n=u.n;m=u.m;X=new T[n*m];for(int i=0;i<n*m;i++)X[i]=u.X[i];}
		~Matrix(){if(n*m>1)delete []X;n=m=0;}
		Matrix &operator=(const Matrix &u){
			if(&u==this)return *this;
			if(n==u.n&&m==u.m){for(int i=0;i<n*m;i++)X[i]=u.X[i];return *this;}
			else {if(n*m>1)delete []X;n=u.n;m=u.m;X=new T[n*m];
			for(int i=0;i<n*m;i++)X[i]=u.X[i];}return *this;
		}
		Matrix &operator+=(const Matrix &u){if(n==u.n&&m==u.m){for(int i=0;i<n*m;i++)X[i]+=u.X[i];return(*this);}}
		Matrix &operator-=(const Matrix &u){if(n==u.n&&m==u.m){for(int i=0;i<n*m;i++)X[i]-=u.X[i];return(*this);}}
		Matrix &operator*=(const T a){for(int i=0;i<n*m;i++)X[i]*=a;return(*this);}
		Matrix &operator/=(const T a){for(int i=0;i<n*m;i++)X[i]/=a;return(*this);}
		const bool operator==(const Matrix &u){if(m!=u.m||n!=u.n)return false;for(int i=0;i<n*m;i++){if(X[i]!=u.X[i])return false;}return true;}
		const bool operator!=(const Matrix &u){if(m!=u.m||n!=u.n)return true;for(int i=0;i<n*m;i++){if(X[i]!=u.X[i])return true;}return false;}
		Matrix &operator +(){return(*this);}
		Matrix &operator -(){for(int i=0;i<n*m;i++)X[i]=-X[i];return(*this);}
		Matrix<T> inv(void){
			Matrix<T> src=*this;Matrix<T> dst(n,m);
			Inverse_LU(dst.X,src.X,n);
			return dst;
		}
		T det(void){
			int *ip;Matrix src(*this);
			T result;ip = new int[n];
			result=lu(src.n,src.X,ip);
			delete []ip;
			return result;
		}
		Matrix trn(void){
			Matrix result(m,n);
			for(int ii=0;ii<m;ii++)
				for(int jj=0;jj<n;jj++)
					result.X[m*jj+ii]=X[n*ii+jj];
			return result;
		}

		T &operator()(int _row, int _col){return X[m*_row+_col];}

		void free(){if(this->n*this->m)delete []X;this->n=this->m=0;}
		void malloc(int _n ,int _m){n=_n;m=_m;X=new T[n*m];for(int i=0;i<n*m;i++)X[i]=0;}
		void identity(){
			for(int i=0;i<this->n*this->m;i++)
				this->X[i]=0;
			for(int i=0;i<this->m;i++)
				if(i<this->n)this->X[m*i+i]=1;
		}
		template<class T>friend Matrix operator *(const Matrix<T> &u, const T a){Matrix result(u);result*=a;return result;}
		template<class T>friend Matrix operator *(const T a, const Matrix<T> &u){Matrix result(u);result*=a;return result;}
		template<class T>friend Matrix operator *(const Matrix<T> &u, const Matrix<T> &v){
			Matrix result(v.n,u.m);
			for(int i=0;i<u.m;i++)for(int j=0;j<v.n;j++)for(int k=0;k<u.n;k++)
					result.X[v.n*i+j]+=u.X[u.n*i+k]*v.X[v.n*k+j];
			return result;
		}
		template<class T>friend Matrix operator +(const Matrix<T> &u, const Matrix<T> &v){Matrix result(u);result+=v;return result;}
		template<class T>friend Matrix operator -(const Matrix<T> &u, const Matrix<T> &v){Matrix result(u);result-=v;return result;}
		template<class T>friend Matrix operator /(const Matrix<T> &u, const T a){Matrix result=u;result/=a;return result;}

	};
	template<class T>VectorN<T> operator *(const Matrix<T> &u, const VectorN<T> &v){
		VectorN<T> result(u.m);
		if(u.n!=v.n)return v;
		for(int i=0;i<u.m;i++){
			result.X[i]=0;
			for(int j=0;j<v.n;j++)
				result.X[i]+=u.X[v.n*i+j]*v.X[j];
		}
		return result;
	}
	template<class T> Vector3<T> operator *(const Matrix<T> &u, const Vector3<T> &v){
		Vector3<T> result;
		if(u.n!=3)return v;
		for(int i=0;i<3;i++){
			result.X[i]=0;
			for(int j=0;j<3;j++)
				result.X[i]+=u.X[3*i+j]*v.X[j];
		}
		return result;
	}

	typedef	Matrix<int> Matrixi;
	typedef	Matrix<double> Matrixd;
	typedef	Matrix<float> Matrixf;
}

/****************************************************************************
** 4*4 dobule Transfer Matrix Class
****************************************************************************/

/*
class TransferMatrix{
private:
	int i,j,k;
public:
	double T[16];
	double trT[16];
	TransferMatrix(){for(i=0;i<16;i++)T[i]=0;for(i=0;i<4;i++)T[4*i+i]=1;}
	TransferMatrix(double *_T){for(i=0;i<16;i++)T[i]=_T[i];}
	TransferMatrix(float *_T){for(i=0;i<16;i++)T[i]=_T[i];}
	~TransferMatrix(){};
	TransferMatrix &operator=(const TransferMatrix &src){for(i=0;i<16;i++)T[i]=src.T[i];return(*this);}
	TransferMatrix &operator=(const int &src){for(i=0;i<16;i++)T[i]=src;for(i=0;i<4;i++)T[4*i+i]=1;return(*this);}
	TransferMatrix &operator+=(const TransferMatrix &src){for(i=0;i<16;i++)T[i]+=src.T[i];return(*this);}
	TransferMatrix &operator-=(const TransferMatrix &src){for(i=0;i<16;i++)T[i]-=src.T[i];return(*this);}
	TransferMatrix &operator*=(const double &c){for(i=0;i<16;i++)T[i]*=c;return(*this);}
	TransferMatrix &operator/=(const double &c){for(i=0;i<16;i++)T[i]/=c;return(*this);}

	TransferMatrix &operator*=(const TransferMatrix &src){
		TransferMatrix result;
		for(i=0;i<4;i++){
			for(j=0;j<4;j++){
				result.T[4*i+j]=0.0;
				for(k=0;k<4;k++)
					result.T[4*i+j]+=T[4*i+k]*src.T[4*k+j];
			}
		}
		for(i=0;i<16;i++)T[i]=result.T[i];
		return(*this);
	}

	const bool operator!=(const TransferMatrix &src){
		for(i=0;i<4;i++)
			for(j=0;j<4;j++)
				if(this->T[4*i+j]!=src.T[4*i+j])return true;
		return false;
	}
	friend Vector3d operator *(const TransferMatrix &src, const Vector3d &u){
		Vector3d result=Vector3d();
		Vector4d temp_input;
		Vector4d temp_result;
		for(int i=0;i<3;i++)temp_input.X[i]=u.X[i];
		temp_input.w=1;
		for(int i=0;i<4;i++)
			for(int j=0;j<4;j++)
				temp_result.X[i]+=src.T[4*i+j]*temp_input.X[j];
		for(int i=0;i<3;i++)result.X[i]=temp_result.X[i]/temp_result.X[3];
		return result;
	}
	friend Vector3d operator *(const Vector3d &u, const TransferMatrix &src){
		Vector3d result=Vector3d();
		Vector4d temp_input=Vector4d();
		Vector4d temp_result;
		for(int i=0;i<3;i++)temp_input.X[i]=u.X[i];
		temp_input.w=1;
		for(int i=0;i<4;i++)
			for(int j=0;j<4;j++)
				temp_result.X[i]+=temp_input.X[j]*src.T[4*j+i];
		for(int i=0;i<3;i++)result.X[i]=temp_result.X[i]/temp_result.X[3];
		return result;
	}
	friend TransferMatrix operator *(const double c, const TransferMatrix &src){
		TransferMatrix result=src;result*=c;return result;
	}
	friend TransferMatrix operator *(const TransferMatrix &src, const TransferMatrix &dest){
		TransferMatrix result=src;result*=dest;return result;
	}
	friend ostream& operator<<(ostream& stream, const TransferMatrix& v){
		return stream <<v.T[0]<<","<<v.T[1]<<","<<v.T[2]<<","<<v.T[3]<<std::endl
					<<v.T[4]<<","<<v.T[5]<<","<<v.T[6]<<","<<v.T[7]<<std::endl
					<<v.T[8]<<","<<v.T[9]<<","<<v.T[10]<<","<<v.T[11]<<std::endl
					<<v.T[12]<<","<<v.T[13]<<","<<v.T[14]<<","<<v.T[15]<<std::endl;
	}
	TransferMatrix inv();
	Vector3d rotate(Vector3d);
	Vector3d inv_rotate(Vector3d);
	TransferMatrix scale(double);
	TransferMatrix scale(Vector3d);
	TransferMatrix translate();
	TransferMatrix alline_X(Vector3d);
	TransferMatrix alline_Y(Vector3d);
	TransferMatrix setScale(Vector3d);
	TransferMatrix setRotate(Vector3d);
	TransferMatrix setTranslate(Vector3d);
	Vector3d getTranslate();
	TransferMatrix getRotateMatrix();
	TransferMatrix getTranslateMatrix();
	double* getTr4OpenGL(){for(int i=0;i<4;i++)for(int j=0;j<4;j++)trT[4*i+j]=T[4*j+i];return trT;}
};
class TransferMatrixf{
private:
	int i,j,k;
public:
	float T[16];
	TransferMatrixf(){for(i=0;i<16;i++)T[i]=0;for(i=0;i<4;i++)T[4*i+i]=1;}
	TransferMatrixf(float *_T){for(i=0;i<16;i++)T[i]=_T[i];}
	TransferMatrixf(double *_T){for(i=0;i<16;i++)T[i]=_T[i];}
	~TransferMatrixf(){};
	TransferMatrixf &operator=(const TransferMatrixf &src){for(i=0;i<16;i++)T[i]=src.T[i];return(*this);}
	TransferMatrixf &operator=(const int &src){for(i=0;i<16;i++)T[i]=src;for(i=0;i<4;i++)T[4*i+i]=1;return(*this);}
	TransferMatrixf &operator+=(const TransferMatrixf &src){for(i=0;i<16;i++)T[i]+=src.T[i];return(*this);}
	TransferMatrixf &operator-=(const TransferMatrixf &src){for(i=0;i<16;i++)T[i]-=src.T[i];return(*this);}
	TransferMatrixf &operator*=(const float &c){for(i=0;i<16;i++)T[i]*=c;return(*this);}
	TransferMatrixf &operator/=(const float &c){for(i=0;i<16;i++)T[i]/=c;return(*this);}

	TransferMatrixf &operator*=(const TransferMatrixf &src){
		TransferMatrix result;
		for(i=0;i<4;i++){
			for(j=0;j<4;j++){
				result.T[4*i+j]=0.0;
				for(k=0;k<4;k++)
					result.T[4*i+j]+=T[4*i+k]*src.T[4*k+j];
			}
		}
		for(i=0;i<16;i++)T[i]=result.T[i];
		return(*this);
	}

	const bool operator!=(const TransferMatrixf &src){
		for(i=0;i<4;i++)
			for(j=0;j<4;j++)
				if(this->T[4*i+j]!=src.T[4*i+j])return true;
		return false;
	}
	friend Vector3d operator *(const TransferMatrixf &src, const Vector3d &u){
		Vector3d result=Vector3d();
		Vector4d temp_input;
		Vector4d temp_result;
		for(int i=0;i<3;i++)temp_input.X[i]=u.X[i];
		temp_input.w=1;
		for(int i=0;i<4;i++)
			for(int j=0;j<4;j++)
				temp_result.X[i]+=src.T[4*i+j]*temp_input.X[j];
		for(int i=0;i<3;i++)result.X[i]=temp_result.X[i]/temp_result.X[3];
		return result;
	}
	friend Vector3d operator *(const Vector3d &u, const TransferMatrixf &src){
		Vector3d result=Vector3d();
		Vector4d temp_input=Vector4d();
		Vector4d temp_result;
		for(int i=0;i<3;i++)temp_input.X[i]=u.X[i];
		temp_input.w=1;
		for(int i=0;i<4;i++)
			for(int j=0;j<4;j++)
				temp_result.X[i]+=temp_input.X[j]*src.T[4*j+i];
		for(int i=0;i<3;i++)result.X[i]=temp_result.X[i]/temp_result.X[3];
		return result;
	}
	friend TransferMatrixf operator *(const float c, const TransferMatrixf &src){
		TransferMatrixf result=src;result*=c;return result;
	}
	friend TransferMatrixf operator *(const TransferMatrixf &src, const TransferMatrixf &dest){
		TransferMatrixf result=src;result*=dest;return result;
	}
	friend ostream& operator<<(ostream& stream, const TransferMatrixf& v){
		return stream <<v.T[0]<<","<<v.T[1]<<","<<v.T[2]<<","<<v.T[3]<<std::endl
					<<v.T[4]<<","<<v.T[5]<<","<<v.T[6]<<","<<v.T[7]<<std::endl
					<<v.T[8]<<","<<v.T[9]<<","<<v.T[10]<<","<<v.T[11]<<std::endl
					<<v.T[12]<<","<<v.T[13]<<","<<v.T[14]<<","<<v.T[15]<<std::endl;
	}
	TransferMatrixf inv();
	Vector3d rotate(Vector3d);
	Vector3d inv_rotate(Vector3d);
	TransferMatrixf scale(double);
	TransferMatrixf scale(Vector3d);
	TransferMatrixf translate();
	TransferMatrixf alline_X(Vector3d);
	TransferMatrixf alline_Y(Vector3d);
	TransferMatrixf setScale(Vector3d);
	TransferMatrixf setRotate(Vector3d);
	TransferMatrixf setTranslate(Vector3d);
	Vector3d getTranslate();
	TransferMatrixf getRotateMatrix();
	TransferMatrixf getTranslateMatrix();
};
class Calibration{
private:
	Vector3d posA[1000];
	Vector3d posB[1000];
	TransferMatrix T;
	int numPoint;
	Matrix P_A;
	Matrix P_B;
	Matrix T_temp;
public:
	Calibration();
	~Calibration();
	void setPosA(Vector3d);
	void setPosB(Vector3d);
	void clearPoints();
	TransferMatrix genCalibratedTM();
	TransferMatrix getCalibratedTM();
	bool loadCalibratedTM(const char*);
	bool saveCalibratedTM(const char*);
};

class Quaternion{
private:
	union {struct{double t,x,y,z;};double X[4];};
public:
	Quaternion(){t=x=y=z=0;}
	~Quaternion(){}
	Quaternion &operator=(const Quaternion &src){for(int i=0;i<4;i++)X[i]=src.X[i];return(*this);}
	Quaternion &operator*=(const Quaternion &src){
		Vector3d u(x, y, z);
		Vector3d v(src.x, src.y, src.z);
		Vector3d dst;
		double w;
		w=t*src.t-u*v;
		dst=t*v+src.t*u+u%v;
		this->t=w;
		this->x=dst.x;
		this->y=dst.y;
		this->z=dst.z;
		return(*this);
	}
	friend Quaternion operator *(const Quaternion &u, const Quaternion &v){
		Quaternion dst=u;
		dst*=v;
		return dst;
	}
	//Ç±ÇÍÇÕç¿ïWÇÃê›íË
	void setQuatCoord(Vector3d _src){this->t=0;this->x=_src.x;this->y=_src.y;this->z=_src.z;}
	//Ç±ÇÍÇÕã§ñQuaternionÇÃéÊìæ
	Quaternion bar(){Quaternion dst; dst.t=t; dst.x=-x; dst.y=-y; dst.z=-z; return dst;}
	//Ç±ÇÍÇÕé≤é¸ÇËÇÃâÒì]Çï\Ç∑QuaternionÇÃê›íË
	void setQuatRotation(double _angle, Vector3d _axis){
		_axis=_axis/_axis.abs();
		t=cos(_angle/2); x=_axis.x*sin(_angle/2); y=_axis.y*sin(_angle/2); z=_axis.z*sin(_angle/2);
	}
	double getAngle(){		return t;	}
	Vector3d getVector3d(){		return Vector3d(x,y,z);	}
	//Ç±ÇÍÇÕQuaternionÇ©ÇÁïœä∑çsóÒÇ÷ÇÃïœä∑
	TransferMatrix getTransferMatrix(){
	}
};
*/
/****************************************************************************
** Vector and Matrix Calculation Function
****************************************************************************/
/*
namespace MathCalMethod{
	template<class T>T lu(int, T*, int*);
	template<class T>T Inverse_LU(T*, T*,int);
//	template<class T>T Inverse_Gauss(Matrix<T>*, Matrix<T>*);

	Vector3d getRotatedVector(double _angle, Vector3d _axis, Vector3d _src);
	bool isSameDirection(double _torarance, Vector3d _vec1, Vector3d _vec2);
	double getAngleBetween(Vector3d _vec1, Vector3d _vec2);
};


*/