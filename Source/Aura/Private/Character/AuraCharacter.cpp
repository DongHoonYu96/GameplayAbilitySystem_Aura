// Fill out your copyright notice in the Description page of Project Settings.
// 프로젝트 설정의 Description 페이지에 저작권 고지를 작성합니다.

#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AAuraCharacter::AAuraCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 캐릭터가 회전하는 비율을 설정합니다 (Yaw 축).
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);

	// 캐릭터의 움직임을 평면에 제한하여 Z축으로의 움직임을 막습니다.
	GetCharacterMovement()->bConstrainToPlane = true;

	// 시작 시 캐릭터를 이동 평면에 정렬합니다.
	GetCharacterMovement()->bSnapToPlaneAtStart = true;
	
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	//카메라를 springarm에 넣기
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	
	SpringArmComponent->TargetArmLength = 800.f;
	SpringArmComponent->SetRelativeRotation(FRotator(-45.f, 0.f, 0.f));


	// 컨트롤러 기반 회전을 비활성화합니다 (Pitch, Roll, Yaw 축).
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false; //캐릭터는 컨트롤러의 요를 따라간다.
	bUseControllerRotationRoll = false;

	
	
	SpringArmComponent->bUsePawnControlRotation = false;
		
	SpringArmComponent->bInheritPitch = false;
	SpringArmComponent->bInheritYaw = false; //카메라는 플레이어의 요를 안따른다.
	SpringArmComponent->bInheritRoll = false;
		
	SpringArmComponent->bDoCollisionTest = false;

	
	GetCharacterMovement()->RotationRate = FRotator(0.f, 360.f, 0.f);
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = true; //의지대로 부드럽게 회전함.

	GetCharacterMovement()->bOrientRotationToMovement=true; // 캐릭방향으로 자동회전!
	
}

//사용 예
//일반적으로 InitAbilityActorInfo는 캐릭터의 BeginPlay 함수나 상태가 변경될 때
//(예: PossessedBy, OnRep_PlayerState) 호출됩니다. 이렇게 하면 게임이 시작할 때 또는
//중요한 게임 상태가 변경될 때 능력 시스템이 적절하게 초기화되고 준비됩니다.
// 캐릭터가 새로운 컨트롤러에 의해 소유되었을 때 호출되는 함수
void AAuraCharacter::PossessedBy(AController* NewController)
{
	// 부모 클래스의 PossessedBy 함수 호출
	Super::PossessedBy(NewController);

	// 서버에서 능력 시스템 액터 정보를 초기화
	//init ability actor info for server
	InitAbilityActorInfo();
}

// 플레이어 상태가 복제될 때 호출되는 함수
void AAuraCharacter::OnRep_PlayerState()
{
	// 부모 클래스의 OnRep_PlayerState 함수 호출
	Super::OnRep_PlayerState();

	// 클라이언트에서 능력 시스템 액터 정보를 초기화
	//init ability actor info for client
	InitAbilityActorInfo();
}

// 능력 시스템 액터 정보를 초기화하는 함수
void AAuraCharacter::InitAbilityActorInfo()
{
	// 플레이어 상태를 AAuraPlayerState 타입으로 가져옴
	AAuraPlayerState* AuraPlayerState = GetPlayerState<AAuraPlayerState>();
	// AuraPlayerState가 유효한지 확인
	check(AuraPlayerState);
	// AuraPlayerState의 능력 시스템 컴포넌트를 이용하여 액터 정보를 초기화
	//첫 번째 인자는 능력의 소유자(Owner)를, 두 번째 인자는 능력이 적용될 대상(Actor)
	AuraPlayerState->GetAbilitySystemComponent()->InitAbilityActorInfo(AuraPlayerState, this);

	Cast<UAuraAbilitySystemComponent>(AuraPlayerState->GetAbilitySystemComponent())->AbilityActorInfoSet();
	
	// 이 캐릭터의 AbilitySystemComponent를 AuraPlayerState의 것으로 설정
	AbilitySystemComponent = AuraPlayerState->GetAbilitySystemComponent();
	// 이 캐릭터의 AttributeSet을 AuraPlayerState의 것으로 설정
	AttributeSet = AuraPlayerState->GetAttributeSet();

	//멀티플레이어서 null가능성 있음
	AAuraPlayerController* AuraPlayerController = Cast<AAuraPlayerController>(GetController());
	if (AuraPlayerController)
	{
		if (AAuraHUD* AuraHUD = Cast<AAuraHUD>(AuraPlayerController->GetHUD()))
		{
			AuraHUD->InitOverlay(AuraPlayerController, AuraPlayerState, AbilitySystemComponent, AttributeSet);
		}
	}
	InitializeDefaultAttributes(); //정보
}

void AAuraCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	PlayerInputComponent->BindAction("RightClick", IE_Pressed, this, &AAuraCharacter::MoveToLocation);

	
}



void AAuraCharacter::MoveToLocation()
{
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		FHitResult Hit;
		PC->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, Hit);

		// if (Hit.bBlockingHit)
		// {
		// 	// Move the character to the location
		// 	SetActorLocation(Hit.ImpactPoint);
		// }
		if (Hit.bBlockingHit)
		{
			// Use AIBlueprintHelperLibrary to command the AI to move to the location
			UAIBlueprintHelperLibrary::SimpleMoveToLocation(PC, Hit.ImpactPoint);
		}
	}
}


