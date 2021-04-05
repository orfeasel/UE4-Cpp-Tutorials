// Fill out your copyright notice in the Description page of Project Settings.


#include "Tests/MathTests.h"
#include "AutomationPost/TestClass.h" //Contains FMathStruct
#include "Misc/AutomationTest.h"


#if WITH_DEV_AUTOMATION_TESTS

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FMathStructTest, "MathTests",EAutomationTestFlags::ApplicationContextMask | EAutomationTestFlags::SmokeFilter)


bool FMathStructTest::RunTest(const FString& Parameters)
{
	//Addition tests
	{
		int32 ResultA = FMathStruct::Add(5,15);
		int32 ExpectedResultA = 19;
		float ResultB = FMathStruct::Add(3.5f,1.5f);
		float ExpectedResultB = 5.f;
		
		TestEqual(TEXT("Testing sum in integers"), ResultA, ExpectedResultA);
		TestEqual(TEXT("Testing sum in floats"),ResultB, ExpectedResultB);
	}

	return true;
}


#endif //WITH_DEV_AUTOMATION_TESTS