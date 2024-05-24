// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "AuraObject.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraObject : public UObject
{
	GENERATED_BODY()

public:
	UAuraObject();

	UFUNCTION()
	void HelloUnreal();

	const FString& GetObjectName() const { return Name;}

protected:
	UPROPERTY()
	FString Name; //(유사) 포인터임!
};
