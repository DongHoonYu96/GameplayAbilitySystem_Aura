// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true; // 해당 컨트롤러가 네트워크 상에서 복제될 수 있도록 함
}

// 게임 시작 시 호출되는 함수
void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay(); // 부모 클래스의 BeginPlay 함수 호출

	check(AuraContext); // AuraContext의 널(Null) 검사

	// Enhanced Input Subsystem을 사용하여 특정 입력 매핑 콘텍스트를 추가하는 부분
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	check(Subsystem); // 서브시스템이 유효한지 확인
	Subsystem->AddMappingContext(AuraContext, 0); // AuraContext를 서브시스템에 추가

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
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);

	// MoveAction이 발생할 때 AAuraPlayerController 클래스의 Move() 함수를 바인딩합니다.
	EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
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
