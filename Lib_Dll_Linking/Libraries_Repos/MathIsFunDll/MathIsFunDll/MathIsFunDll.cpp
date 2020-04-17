// MathIsFunDll.cpp : Defines the exported functions for the DLL.
//

#include "pch.h"
#include "framework.h"
#include "MathIsFunDll.h"


MATHISFUNDLL_API float Sum(float a, float b)
{
    return a+b;
}

MATHISFUNDLL_API int GetFibonacciNTerm(int Term)
{
	//https://www.mathsisfun.com/numbers/fibonacci-sequence.html
	//Recursive way of calculating fibonacci term. Not the best algorithm in terms of efficiency but it works in our case :)
	if (Term == 0)
	{
		return 0;
	}
	else if (Term == 1)
	{
		return 1;
	}
	else return GetFibonacciNTerm(Term - 1) + GetFibonacciNTerm(Term - 2);
}