// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	//구체달고, 루트로만듬
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Sphere->SetCollisionResponseToAllChannels(ECR_Ignore); //모든대상에대해 겹쳐도 무시함
	Sphere->SetCollisionResponseToChannel(ECC_WorldDynamic,ECR_Overlap);//월드 다이나믹 유형과는 겹침이벤트
	Sphere->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Overlap); //월드스태틱 유형과는 겹침이벤트
	Sphere->SetCollisionResponseToChannel(ECC_Pawn,ECR_Overlap); //폰유형과는 겹침이벤트

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");
	ProjectileMovement->InitialSpeed = 550.f;
	ProjectileMovement->MaxSpeed = 550.f;
	ProjectileMovement->ProjectileGravityScale =0.f; //중력없도록
}

void AAuraProjectile::BeginPlay()
{
	Super::BeginPlay();
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap); //오버랩 이벤트 달기, 실행될함수
	
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	
}


