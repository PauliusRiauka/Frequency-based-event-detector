//
//    FILE: CComplex.cpp
//  AUTHOR: Rob Tillaart
// VERSION: see CComplex_LIB_VERSION
// PURPOSE: library for CComplex math for Arduino
//     URL:
//
// Released to the public domain
//

#include "CComplex.h"

CComplex::CComplex(double real, double imag)
{
	re = real;
	im = imag;
}

CComplex::CComplex(const CComplex &c)
{
	re = c.re;
	im = c.im;
}

CComplex::CComplex(double d)
{
	re = d;
	im = 0;
}

void CComplex::set(double real, double imag)
{
	re = real;
	im = imag;
}

double CComplex::real()
{
	return re;
}

double CComplex::imag()
{
	return im;
}

void CComplex::polar(double modulus, double phase)
{
	re = modulus * cos(phase);
	im = modulus * sin(phase);
}

double CComplex::phase()
{
	return atan2(im, re);
}

double CComplex::modulus()
{
	return hypot(re, im);
}

// conjugate is the number mirrored in x-axis
CComplex CComplex::conjugate()
{
	return CComplex(re,-im);
}

CComplex CComplex::reciprocal()
{
	double f = re*re + im*im;
	double r = re/f;
	double i = -im/f;
	return CComplex(r,i);
}

// EQUALITIES
bool CComplex::operator == (CComplex c)
{
	return (re == c.re) && (im == c.im);
}

bool CComplex::operator != (CComplex c)
{
	return (re != c.re) || (im != c.im);
}

// NEGATE
CComplex CComplex::operator - ()
{
	return CComplex(-re, -im);
}

// BASIC MATH
CComplex CComplex::operator + (CComplex c)
{
	return CComplex(re + c.re, im + c.im);
}

CComplex CComplex::operator - (CComplex c)
{
	return CComplex(re - c.re, im - c.im);
}

CComplex CComplex::operator * (CComplex c)
{
	double r = re * c.re - im * c.im;
	double i = re * c.im + im * c.re;
	return CComplex(r,i);
}

CComplex CComplex::operator / (CComplex c)
{
	double f = c.re*c.re + c.im*c.im;
	double r = re * c.re + im * c.im;
	double i = re * c.im - im * c.re;
	return CComplex(r / f, -i / f);
}

void CComplex::operator += (CComplex c)
{
	this->re += c.re;
	this->im += c.im;
	return;
}

void CComplex::operator -= (CComplex c)
{
	this->re -= c.re;
	this->im -= c.im;
	return;
}

void CComplex::operator *= (CComplex c)
{
	double r = re * c.re - im * c.im;
	double i = re * c.im + im * c.re;
	this->re = r;
	this->im = i;
	return;
}

void CComplex::operator /= (CComplex c)
{
	double f = c.re*c.re + c.im*c.im;
	double r = re * c.re + im * c.im;
	double i = re * c.im - im * c.re;
	this->re = r/f;
	this->im = -i/f;
	return;
}

//
// POWER FUNCTIONS
//
CComplex CComplex::c_sqrt()
{
	double m = modulus();
	double r = sqrt(0.5 * (m+this->re));
	double i = sqrt(0.5 * (m-this->re));
	if (this->im < 0) i = -i;
	return CComplex(r,i);
}

CComplex CComplex::c_exp()
{
	double e = exp(re);
	return CComplex(e * cos(im), e * sin(im));
}

CComplex CComplex::c_log()
{
	double m = this->modulus();
	double p = this->phase();
	if (p > PI) p -= 2*PI;
	return CComplex(log(m), p);
}

CComplex CComplex::c_pow(CComplex c)
{
	CComplex t = c * this->c_log();
	return t.c_exp();
}

CComplex CComplex::c_logn(CComplex c)
{
	return this->c_log()/c.c_log();
}

CComplex CComplex::c_log10()
{
	return this->c_logn(10);
}

//
// GONIO I - SIN COS TAN 
//
CComplex CComplex::c_sin()
{
	return CComplex(sin(re) * cosh(im), cos(re) * sinh(im));
}

CComplex CComplex::c_cos()
{
	return CComplex(cos(re) * cosh(im), -sin(re) * sinh(im));
}

CComplex CComplex::c_tan()
{
	return c_sin() / c_cos();
}

CComplex CComplex::gonioHelper1(int mode)
{
	CComplex c = (CComplex(1,0) - (*this * *this)).c_sqrt();
	if (mode==0)
	{
		c = c + *this * CComplex(0, -1);
	}
	else
	{
		c = *this + c * CComplex(0, -1);
	}
	c = c.c_log() * CComplex(0,1);
	return c;
}

CComplex CComplex::c_asin()
{
	return this->gonioHelper1(0);
}

CComplex CComplex::c_acos()
{
	return this->gonioHelper1(1);
}

CComplex CComplex::c_atan()
{
	return (CComplex(0,-1) * (CComplex(this->re, this->im - 1)/CComplex(-this->re, -this->im - 1)).c_log())/2;
}

//
// GONIO II - CSC SEC COT
//
CComplex CComplex::c_csc()
{
	return CComplex(1,0) / this->c_sin();
}

CComplex CComplex::c_sec()
{
	return CComplex(1,0) / this->c_cos();
}

CComplex CComplex::c_cot()
{
	return CComplex(1,0) / this->c_tan();
}

CComplex CComplex::c_acsc()
{
	return (CComplex(1,0) / *this).c_asin();
}

CComplex CComplex::c_asec()
{
	return (CComplex(1,0) / *this).c_acos();
}

CComplex CComplex::c_acot()
{
	return (CComplex(1,0) / *this).c_atan();
}

//
// GONIO HYPERBOLICUS I
//
CComplex CComplex::c_sinh()
{
	return CComplex(sinh(this->re) * cos(this->im), cosh(this->re)* sin(this->im));
}

CComplex CComplex::c_cosh()
{
	return CComplex(cosh(this->re) * cos(this->im), sinh(this->re)* sin(this->im));
}

CComplex CComplex::c_tanh()
{
	return c_sinh() / c_cosh();
}

CComplex CComplex::gonioHelper2(int mode)
{
	CComplex c = (*this * *this);
	if (mode==0)
	{
		c += 1;
	}
	else
	{
		c -= 1;
	}
	c = (*this + c.c_sqrt()).c_log();
	return c;
}

CComplex CComplex::c_asinh()
{
	return gonioHelper2(0);
}

CComplex CComplex::c_acosh()
{
	return gonioHelper2(1);
}

CComplex CComplex::c_atanh()
{
	CComplex one(1,0);
	CComplex c = (*this + one).c_log();
	c = c - (-(*this - one)).c_log();
	return c/2;
}

//
// GONIO HYPERBOLICUS II
//
CComplex CComplex::c_csch()
{
	return CComplex(1,0) / this->c_sinh();
}

CComplex CComplex::c_sech()
{
	return CComplex(1,0) / this->c_cosh();
}

CComplex CComplex::c_coth()
{
	return CComplex(1,0) / this->c_tanh();
}

CComplex CComplex::c_acsch()
{
	return (CComplex(1,0) / *this).c_asinh();
}

CComplex CComplex::c_asech()
{
	return (CComplex(1,0) / *this).c_acosh();
}

CComplex CComplex::c_acoth()
{
	return (CComplex(1,0) / *this).c_atanh();
}

// --- END OF FILE ---