// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"
#include "EnhancedInputSubsystems.h"

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