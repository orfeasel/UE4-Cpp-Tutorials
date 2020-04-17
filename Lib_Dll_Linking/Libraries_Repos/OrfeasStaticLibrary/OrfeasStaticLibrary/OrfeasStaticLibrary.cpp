// OrfeasStaticLibrary.cpp : Defines the functions for the static library.
//

#include "pch.h"
#include "framework.h"
#include "OrfeasMathLibrary.h"

// TODO: This is an example of a library function
void fnOrfeasStaticLibrary()
{
	//TODO: implement
}

namespace OrfeasMathLibrary
{
	double Arithmetic::Add(double a, double b)
	{
		return a + b;
	}

	double Arithmetic::Subtract(double a, double b)
	{
		return a - b;
	}

	double Arithmetic::Multiply(double a, double b)
	{
		return a * b;
	}

	double Arithmetic::Divide(double a, double b)
	{
		return a / b;
	}
}