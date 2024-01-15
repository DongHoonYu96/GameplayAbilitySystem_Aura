// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "UObject/NoExportTypes.h"
#include "AuraWidgetController.generated.h"

class UAttributeSet; //전방선언 => 포인터만 사용하겠다.
class UAbilitySystemComponent;
class UAuraAttributeSet;

//UAuraWidgetController 맴버변수 쉽게 초기화하기 위한 구조체
USTRUCT(BlueprintType)
struct FWidgetControllerParams
{
	GENERATED_BODY()

	FWidgetControllerParams(){}
	FWidgetControllerParams(APlayerController* PC,
		APlayerState* PS, UAbilitySystemComponent* ASC,
		UAttributeSet* AS) :
	PlayerController(PC),PlayerState(PS)
	,AbilitySystemComponent(ASC),AttributeSet(AS){}

	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	TObjectPtr<APlayerController> PlayerController=nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	TObjectPtr<APlayerState> PlayerState=nullptr;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite);
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent=nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UAttributeSet> AttributeSet=nullptr;
	
};

/**
 * 
 */

UCLASS()
class AURA_API UAuraWidgetController : public UObject
{
	GENERATED_BODY()

public:
	//멤버들 초기화하는 함수
	UFUNCTION(BlueprintCallable)
	void SetWidgetControllerParams(const FWidgetControllerParams&
		WCParams);

	//속성들 방송하는 함수
	virtual void BroadcastInitialValues();
	
protected:
	UPROPERTY(BlueprintReadOnly, Category="WidgetController");
	TObjectPtr<APlayerController> PlayerController;
	
	UPROPERTY(BlueprintReadOnly, Category="WidgetController");
	TObjectPtr<APlayerState> PlayerState;
	
	UPROPERTY(BlueprintReadOnly, Category="WidgetController");
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(BlueprintReadOnly, Category="WidgetController");
	TObjectPtr<UAttributeSet> AttributeSet;
};
