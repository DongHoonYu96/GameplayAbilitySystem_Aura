// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/AuraCharacterBase.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Aura/Aura.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
AAuraCharacterBase::AAuraCharacterBase()
{
 	// Set this character to call Tick() every frame.
 	// You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false; //틱마다 tick함수 실행하지마

	//캐릭겹치면 카메라 줌인 해결 : 캡슐, 매시가 카메라 무시하도록
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera,ECR_Ignore);
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile,ECR_Overlap);
	GetMesh()->SetGenerateOverlapEvents(true);

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

UAnimMontage* AAuraCharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void AAuraCharacterBase::Die()
{
	//무기분리, 분리는자동으로 수행 -> 서버에서분리하면 클라에서도 자동분리
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld,true));
	MulticastHandleDeath();
}

void AAuraCharacterBase::MulticastHandleDeath_Implementation()
{
	//무기, 매시에 피직스 달기 => 땅에서 튕겨나오도록
	Weapon->SetSimulatePhysics(true);
	Weapon->SetEnableGravity(true);
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);

	GetMesh()->SetEnableGravity(true);
	GetMesh()->SetSimulatePhysics(true);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic,ECR_Block);
	
	//캡슐충돌 비활성화
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	//용해효과 실행
	Dissolve();
}

// Called when the game starts or when spawned
void AAuraCharacterBase::BeginPlay()
{
	Super::BeginPlay();
	
}

FVector AAuraCharacterBase::GetCombatSocketLocation()
{
	check(Weapon);
	return Weapon->GetSocketLocation(WeaponTipSocketName);
}

void AAuraCharacterBase::InitAbilityActorInfo()
{
}

void AAuraCharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffect, float Level) const
{
	check(IsValid(GetAbilitySystemComponent())); //ASC확인
	check(GameplayEffect); //GE확인
	//1. Context Handle 만들기
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this); //캐릭이면 캐릭자체, 적이면 적이 소스임
	//2. Effect Spec 만들기
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffect, Level,ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
}

void AAuraCharacterBase::InitializeDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void AAuraCharacterBase::AddCharacterAbilities()
{
	UAuraAbilitySystemComponent* AuraASC = CastChecked<UAuraAbilitySystemComponent>(AbilitySystemComponent); //캐릭베이스는 ASC 가지고있음
	if(!HasAuthority()) return;

	AuraASC->AddCharacterAbilities(StartupAbilities);
	//when call? 플레이어의경우 : 빙의될때
	//적의경우 : ??
}

void AAuraCharacterBase::Dissolve()
{
	if(IsValid(DissolveMaterialInstance))
	{
		//새로운 머테리얼만들고
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0,DynamicMatInst); //설정
		StartDissolveTimeLine(DynamicMatInst);
	}
	if(IsValid(WeaponDissolveMaterialInstance))
	{
		//새로운 머테리얼만들고
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0,DynamicMatInst); //설정
		StartWeaponDissolveTimeLine(DynamicMatInst);
	}
}

