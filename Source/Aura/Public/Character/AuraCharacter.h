// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API AAuraCharacter : public AAuraCharacterBase
{
	GENERATED_BODY()

public:
	AAuraCharacter();
	virtual void PossessedBy(AController* NewController) override;
	virtual void OnRep_PlayerState() override;
private:
	void InitAbilityActorInfo();

	UPROPERTY(EditAnywhere, Category = "Effects")
	class UParticleSystem * LightningThunderParticle;

protected:
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	void MoveToLocation();
	
	// Function to handle lightning strike and damage nearby enemies
	UFUNCTION(BlueprintCallable, Category = "Gameplay")
	void PerformLightningStrike();

	// Input function to trigger lightning strike when 'E' key is pressed
	void TriggerLightningStrike();
	
};
