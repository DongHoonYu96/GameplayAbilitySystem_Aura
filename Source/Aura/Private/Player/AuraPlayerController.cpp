// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Character/RangerCharacter.h"
#include "Data/AuraInputConfigData.h"
#include "Interaction/EnemyInterface.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true; // 해당 컨트롤러가 네트워크 상에서 복제될 수 있도록 함
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	
}



void AAuraPlayerController::CursorTrace()
{
	FHitResult CursorHit;
	// 레이캐스트를 통해 마우스 커서 아래에 있는 객체에 대한 충돌 결과를 가져옵니다.
	// ECC_Visibility 콜리전 채널을 사용하여 가시성이 있는 오브젝트를 대상으로 합니다.
	// 두 번째 매개변수는 복합 콜리전에 대한 단일 충돌만 반환해야 하는지 여부를 나타냅니다.
	// CursorHit 변수에 결과가 저장됩니다.
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	// 마우스 커서 아래에 아무것도 막히지 않았다면 함수를 종료합니다.
	if (!CursorHit.bBlockingHit)
		return;

	// LastActor를 현재 ThisActor로 설정합니다.
	// 새로계산 전에 저장목적
	LastActor = ThisActor;
	// CursorHit에서 반환된 충돌된 액터를 IEnemyInterface로 캐스팅하여 ThisActor에 할당합니다.
	// IEnemyInterface를 구현한 액터에 대해서만 작동합니다.
	//아닌 액터이면 nullptr
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());

	/**
	 * Line trace from cursor. There are several scenarios:
	 *  A. LastActor is null && ThisActor is null
	 *		- Do nothing
	 *	B. LastActor is null && ThisActor is valid
	 *		- Highlight ThisActor
	 *	C. LastActor is valid && ThisActor is null
	 *		- UnHighlight LastActor
	 *	D. Both actors are valid, but LastActor != ThisActor
	 *		- UnHighlight LastActor, and Highlight ThisActor
	 *	E. Both actors are valid, and are the same actor
	 *		- Do nothing
	 */

	if (LastActor == nullptr)
	{
		if (ThisActor != nullptr)
		{
			// Case B
			ThisActor->HighlightActor();
		}
		else
		{
			// Case A - both are null, do nothing
		}
	}
	else // LastActor is valid
	{
		if (ThisActor == nullptr)
		{
			// Case C
			LastActor->UnHighlightActor();
		}
		else // both actors are valid
		{
			if (LastActor != ThisActor)
			{
				// Case D
				LastActor->UnHighlightActor();
				ThisActor->HighlightActor();
			}
			else
			{
				// Case E - do nothing
			}
		}
	}
}

// 게임 시작 시 호출되는 함수
void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay(); // 부모 클래스의 BeginPlay 함수 호출

	check(AuraContext); // AuraContext의 널(Null) 검사

	// Enhanced Input Subsystem을 사용하여 특정 입력 매핑 콘텍스트를 추가하는 부분
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	//check(Subsystem); // 서브시스템이 유효한지 확인
	//Subsystem->AddMappingContext(AuraContext, 0); // AuraContext를 서브시스템에 추가
	if (Subsystem) //멀티플레이어에서 작동하도록 check 미사용!
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}
	
	// 마우스 커서 관련 설정
	bShowMouseCursor = true; // 마우스 커서를 화면에 표시
	DefaultMouseCursor = EMouseCursor::Default; // 기본 마우스 커서를 설정

	// 입력 모드 설정
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // 뷰포트에 마우스 고정 비활성화
	InputModeData.SetHideCursorDuringCapture(false); // 입력 캡처 중에 마우스 커서 숨김 비활성화
	SetInputMode(InputModeData); // 입력 모드 설정
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent(); // 부모 클래스의 SetupInputComponent() 함수 호출

	// InputComponent를 UEnhancedInputComponent 형으로 변환하고 검사합니다.
	//Get 인핸스드 인풋 컴포넌트
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	// MoveAction이 발생할 때 AAuraPlayerController 클래스의 Move() 함수를 바인딩합니다.
	//옵션 : 트리거(누르고있을때), 컴플리트(눌렀다 뗏을때)
	//특정키가(에디터IMC에서 설정한) 눌럿을때, Data 목록 중 Move함수를 실행해라.
	EnhancedInputComponent->BindAction(InputActions->InputMove, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	EnhancedInputComponent->BindAction(InputActions->RightClickThenMoveThere, ETriggerEvent::Triggered, this, &AAuraPlayerController::RightClickMove);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	// 입력 축 벡터를 가져옵니다. 이 벡터는 플레이어의 움직임 방향을 나타냅니다.
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	// 플레이어 컨트롤러의 회전 값을 가져옵니다.
	const FRotator Rotation = GetControlRotation();

	// Yaw (수평) 회전만을 고려하는 회전을 생성합니다.
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	// Yaw 회전을 기준으로 전방과 오른쪽 방향 벡터를 계산합니다.
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// 조종 중인 Pawn(캐릭터)이 있는지 확인합니다.
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		// Pawn에게 계산된 방향과 입력 축 벡터에 따른 움직임 입력을 추가합니다.
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::RightClickMove(const FInputActionValue& InputActionValue)
{
	FHitResult Hit;
	this->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility),true,Hit);
	APawn* ControlledPawn = GetPawn<APawn>();
	if(nullptr==ControlledPawn) return ;
	
	if(Hit.bBlockingHit)
	{
		//To - From == From에서 To로 가는 방향벡터!
		FVector LookAtTarget = Hit.ImpactPoint;
		FVector ToTarget = LookAtTarget - ControlledPawn->GetActorLocation();
		ControlledPawn->AddMovementInput(ToTarget);
		
	}

}


