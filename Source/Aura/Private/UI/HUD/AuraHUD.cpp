// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/AuraHUD.h"

#include "UI/Widget/AuraUserWidget.h"

// AAuraHUD 클래스의 BeginPlay 함수: 게임 플레이가 시작될 때 호출됩니다.
void AAuraHUD::BeginPlay()
{
	Super::BeginPlay(); // 부모 클래스의 BeginPlay 함수를 호출합니다.


	// HUD 위젯을 생성하고 화면에 추가합니다.
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	Widget->AddToViewport();
}
