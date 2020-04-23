// Fill out your copyright notice in the Description page of Project Settings.


#include "HeavyComp.h"

/* Declaring a stat group that will be visible to the profiler, named Orfeas*/
DECLARE_STATS_GROUP(TEXT("Orfeas_Game"), STATGROUP_Orfeas, STATCAT_Advanced);
/* Declaring a cycle stat that belongs to "Orfeas", named Orfeas-FindPrimeNumbers*/
DECLARE_CYCLE_STAT(TEXT("Orfeas - FindPrimeNumbers"), STAT_FindPrimeNumbers, STATGROUP_Orfeas);

// Sets default values for this component's properties
UHeavyComp::UHeavyComp()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UHeavyComp::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UHeavyComp::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

TArray<int32> UHeavyComp::FindPrimeNumbers()
{
	{//Used to declare the "start" of the block we're profiling 

		//Tell the engine that we're profiling the currect block using the STAT_FindPrimeNumbers stat
		SCOPE_CYCLE_COUNTER(STAT_FindPrimeNumbers);

		//A really bad way to find prime numbers
		//Don't use this in an actual application
		TArray<int32> Primes;
		Primes.Add(1);
		Primes.Add(2);
		Primes.Add(3);

		int32 CurrentNumber = 4;

		while (Primes.Num() <= PrimeNumbersToFind)
		{
			bool IsPrime = true;
			for (int32 index = 2; (index < CurrentNumber / 2) && IsPrime; index++)
			{
				if (CurrentNumber % index == 0)
				{
					IsPrime = false;

				}
			}
			if (IsPrime)
			{
				Primes.Add(CurrentNumber);
			}
			CurrentNumber++;
		}
	}//Closing the block of code we're profiling

	//This means that the following two lines won't be profiled and as a result they won't have an impact in the STAT_FindPrimeNumbers used above
	int32 DummyAction=0;
	DummyAction++;

	return Primes;
}

