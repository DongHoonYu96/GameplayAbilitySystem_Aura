// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/AuraCharacterBase.h"
#include "AuraCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
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

	//카메라는 캐릭터만의 속성임 -> 여기서구현
	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	//카메라는 캐릭터만의 속성임 -> 여기서구현
	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<UCameraComponent> CameraComponent;
	
};
