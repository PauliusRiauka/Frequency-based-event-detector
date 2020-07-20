//
//    FILE: CComplex.h
//  AUTHOR: Rob Tillaart
// VERSION: see CComplex_LIB_VERSION
// PURPOSE: library for CComplex math for Arduino
//     URL:
//
// Released to the public domain
//

#ifndef CComplex_h
#define CComplex_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define CComplex_LIB_VERSION 0.1.01

class CComplex
{
public:
	CComplex(double, double);
	CComplex(const CComplex &);
	CComplex(double);

	void set(double, double);
	double real();
	double imag();

	void polar(double, double);
	double phase();
	double modulus();

	CComplex conjugate();
	CComplex reciprocal();

	bool operator == (CComplex);
	bool operator != (CComplex);

    CComplex operator - (); // negation

	CComplex operator + (CComplex);
    CComplex operator - (CComplex);
	CComplex operator * (CComplex);
    CComplex operator / (CComplex);

	void operator += (CComplex);
	void operator -= (CComplex);
	void operator *= (CComplex);
	void operator /= (CComplex);

	CComplex c_sqrt();
	CComplex c_exp();
	CComplex c_log();
	CComplex c_log10();
	CComplex c_pow(CComplex);
	CComplex c_logn(CComplex);

	CComplex c_sin();
	CComplex c_cos();
	CComplex c_tan();
	CComplex c_asin();
	CComplex c_acos();
	CComplex c_atan();

	CComplex c_csc();
	CComplex c_sec();
	CComplex c_cot();
	CComplex c_acsc();
	CComplex c_asec();
	CComplex c_acot();

	CComplex c_sinh();
	CComplex c_cosh();
	CComplex c_tanh();
	CComplex c_asinh();
	CComplex c_acosh();
	CComplex c_atanh();

	CComplex c_csch();
	CComplex c_sech();
	CComplex c_coth();
	CComplex c_acsch();
	CComplex c_asech();
	CComplex c_acoth();

private:
	double re;
	double im;

	CComplex gonioHelper1(int);
	CComplex gonioHelper2(int);
};
#endif
// --- END OF FILE ---
