// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WidgetController/AuraWidgetController.h"
#include "Player/AuraPlayerState.h"
#include "UI/HUD/AuraHUD.h"

UOverlayWidgetController* UAuraAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	//1. PC 얻기
	if(APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject,0))
	{
		//PC에서는 HUD에 항상접근가능
		if(AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			//PC에서 PS 얻기
			//PS에서 ASC, AS 얻기
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			auto ASC = PS->GetAbilitySystemComponent();
			auto AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS); //4총사가 필요함
			return AuraHUD->GetOverlayWidgetController(WidgetControllerParams); //HUD에는 위젯컨트롤러 getter가있고 매개변수로
		}
	}
	return nullptr;
}

UAttributeMenuWidgetController* UAuraAbilitySystemLibrary::GetAttributeMenuWidgetController(
	const UObject* WorldContextObject)
{
	//1. PC 얻기
	if(APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject,0))
	{
		//PC에서는 HUD에 항상접근가능
		if(AAuraHUD* AuraHUD = Cast<AAuraHUD>(PC->GetHUD()))
		{
			//PC에서 PS 얻기
			//PS에서 ASC, AS 얻기
			AAuraPlayerState* PS = PC->GetPlayerState<AAuraPlayerState>();
			auto ASC = PS->GetAbilitySystemComponent();
			auto AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS); //4총사가 필요함
			return AuraHUD->GetAttributeMenuWidgetController(WidgetControllerParams); //HUD에는 위젯컨트롤러 getter가있고 매개변수로
		}
	}
	return nullptr;
}
