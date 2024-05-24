// Fill out your copyright notice in the Description page of Project Settings.


#include "Game/AuraGameModeBase.h"

void AAuraGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTexture,Log,TEXT("Hello World"));
}


