// Fill out your copyright notice in the Description page of Project Settings.


#include "Actor/AuraProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "Aura/Aura.h"
#include "Components/AudioComponent.h"

AAuraProjectile::AAuraProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates=true; //발사체 서버에서 클라로 복사

	//구체달고, 루트로만듬
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	SetRootComponent(Sphere);
	Sphere->SetCollisionObjectType(ECC_Projectile);
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
	SetLifeSpan(LifeSpan); //이 발사체의 수명을 정함
	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraProjectile::OnSphereOverlap); //오버랩 이벤트 달기, 실행될함수

	LoopingSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopingSound,GetRootComponent());
}
/*
 * 
 */
void AAuraProjectile::Destroyed()
{
	//클라의경우 && 복제안된경우
	if(!bHit && !HasAuthority())
	{
		//오버랩되면 소리재생
		UGameplayStatics::PlaySoundAtLocation(this,ImpactSound,GetActorLocation(),FRotator::ZeroRotator);

		//오버랩되면 효과실행
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,ImpactEffect,GetActorLocation());	
	}
	Super::Destroyed();
}

void AAuraProjectile::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                      UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//오버랩되면 소리재생
	UGameplayStatics::PlaySoundAtLocation(this,ImpactSound,GetActorLocation(),FRotator::ZeroRotator);

	//오버랩되면 효과실행
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(this,ImpactEffect,GetActorLocation());
	LoopingSoundComponent->Stop(); // 적과오버랩되면 불 자체에서나는 루핑사운드 중지

	//서버에있으면 파괴 => 클라에복제안되게
	if(HasAuthority())
	{
		Destroy();
	}
	else //클라에서 파괴되기전 겹치는경우, bHit=true이면 이미 복제된거임 / bHit=false이면 복제안된거임, play
	{
		bHit=true;
	}
}


