// Fill out your copyright notice in the Description page of Project Settings.
// 프로젝트 설정의 Description 페이지에 저작권 고지를 작성합니다.

#include "Character/AuraCharacter.h"

#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/AuraPlayerController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

AAuraCharacter::AAuraCharacter()
{
	// 캐릭터의 움직임에 관련된 속성을 설정합니다.

	// 캐릭터가 움직임에 따라 회전하여 이동 방향을 바라보도록 설정합니다.
	GetCharacterMovement()->bOrientRotationToMovement = true;

	// 캐릭터가 회전하는 비율을 설정합니다 (Yaw 축).
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	// 캐릭터의 움직임을 평면에 제한하여 Z축으로의 움직임을 막습니다.
	GetCharacterMovement()->bConstrainToPlane = true;

	// 시작 시 캐릭터를 이동 평면에 정렬합니다.
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// 컨트롤러 기반 회전을 비활성화합니다 (Pitch, Roll, Yaw 축).
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	
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
	InitAbilityActorInfo();
}

// 플레이어 상태가 복제될 때 호출되는 함수
void AAuraCharacter::OnRep_PlayerState()
{
	// 부모 클래스의 OnRep_PlayerState 함수 호출
	Super::OnRep_PlayerState();

	// 클라이언트에서 능력 시스템 액터 정보를 초기화
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
}
