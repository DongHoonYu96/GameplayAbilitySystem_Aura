// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AuraProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;

UCLASS()
class AURA_API AAuraProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AAuraProjectile();
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement; //날라가기 위해 발사체 콤포넌트 이용

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,bool bFromSweep, const FHitResult& SweepResult);
	
private:
	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.f;
	
	bool bHit=false;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect; //충격효과 =>체력깍기

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound; //부딪히면 소리

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound; //불자체에서 반복해서 나느소리

	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent; //멈추기 위해 저장

	
};
