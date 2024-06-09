// Fill out your copyright notice in the Description page of Project Settings.


#include "Interaction/CombatInterface.h"

// Add default functionality here for any ICombatInterface functions that are not pure virtual.
int32 ICombatInterface::GetPlayerLevel()
{
	return 0; //구현안한경우, 0리턴
}

FVector ICombatInterface::GetCombatSocketLocation()
{
	return FVector();
}
