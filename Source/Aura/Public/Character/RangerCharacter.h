// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraCharacterBase.h"
#include "GameFramework/Character.h"
#include "RangerCharacter.generated.h"

UENUM(BlueprintType)
enum EViewMode
{
	None,
	LostArk,
	End
};

UCLASS()
class AURA_API ARangerCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()

public:
	
	// Sets default values for this character's properties
	ARangerCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void SetCamera();
};
