// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/HUD/AuraHUD.h"
#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if(OverlayWidgetController == nullptr)
	{
		//(연결될클래스 , 생성될타입)
		//Outer : 생성될 오브젝트를 소유하는 UObject 객체
		//Class : 생성될 오브젝트의 클래스를 나타내는 UClass 정보
		OverlayWidgetController=NewObject<UOverlayWidgetController>(this,OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);

		return OverlayWidgetController;
	}
	return OverlayWidgetController;
}

//위젯컨트롤러구성, 위젯 구성, 뷰포트에 추가하는 함수
void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	//BP에서 클래스를 안넣어준경우 예외처리, 실패시 문자열프린트
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class uninitialized, please fill out BP_AuraHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class uninitialized, please fill out BP_AuraHUD"));

	//위젯생성 (OevelayWidgetClass는 BP에서 넣어준 구체적 대상이 들어감)
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(),OverlayWidgetClass);
	OverlayWidget=Cast<UAuraUserWidget>(Widget);
	
	//컨트롤러생성
	const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);

	//위젯에 컨트롤러 set하기
	OverlayWidget->SetWidgetController(WidgetController);
	WidgetController->BroadcastInitialValues(); //뷰포트추가전에 방송
	Widget->AddToViewport();
}
