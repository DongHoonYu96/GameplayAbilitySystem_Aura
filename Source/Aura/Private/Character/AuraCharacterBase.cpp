// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacterBase.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
 	// Set this character to call Tick() every frame.
 	// You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; //틱마다 tick함수 실행하지마

	//새콤포넌트 만듬(이름)
	Weapon=CreateDefaultSubobject<USkeletalMeshComponent>("Weapon");
	//무기를 소켓에 연결
	Weapon->SetupAttachment(GetMesh(),FName("WeaponHandSocket"));
	//무기는 충돌안함
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//이후 엔진에서 할일
	//1. 기본캐릭터 상속받은 BPAura 생성
	//2. SKM_Aura - HandL에 WeaponHandSocket생성, 무기위치조정
	//3. BP Aura - Weapon에 staf 매쉬달기

	//여기서하면 고블린에도 카메라가 붙어버림 ㅋㅋㅋㅋ
	// //springarm 붙이기
	// SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	// SpringArmComponent->SetupAttachment(RootComponent);
	// SpringArmComponent->TargetArmLength = 750.f;
	//
	// //카메라를 springarm에 넣기
	// CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	// CameraComponent->SetupAttachment(SpringArmComponent);
	//
	// float CharacterHalfHeight = 95.f;
	// float CharacterRadius = 20.f;
	//
	// GetCapsuleComponent()->InitCapsuleSize(CharacterRadius, CharacterHalfHeight);
	//
	// FVector PivotPosition(0.f, 0.f, -CharacterHalfHeight);
	// FRotator PivotRotation(0.f, -90.f, 0.f);
	// GetMesh()->SetRelativeLocationAndRotation(PivotPosition, PivotRotation);
	
}

//IAbilitySystemInterface 구현 => @Getter 임
UAbilitySystemComponent* AAuraCharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}
