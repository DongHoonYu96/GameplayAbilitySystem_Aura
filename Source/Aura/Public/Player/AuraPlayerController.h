// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "GameplayTagContainer.h"
#include "GameFramework/PlayerController.h"
#include "AuraPlayerController.generated.h"

class UAuraInputConfig;
enum EViewMode : int;
class UAuraInputConfigData;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class IEnemyInterface;
class UAuraAbilitySystemComponent;
/**
 * 
 */
UCLASS()
class AURA_API AAuraPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	AAuraPlayerController();
	virtual void PlayerTick(float DeltaTime) override;

	
protected:
	virtual void BeginPlay() override;

	//빙의될때 자동 호출되는 함수!, 여기서 키매핑을 하고 넘겨줘야됨
	virtual void SetupInputComponent() override;
	

private:
	//BP에서 IMC 선택해주기 위함
	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<UInputMappingContext> AuraContext;

	//IMC에서 설정한 함수들 구현!!
	//UPROPERTY(EditAnywhere,Category="Input")
	//TObjectPtr<UInputAction> MoveAction;
	void Move(const FInputActionValue& InputActionValue);
	void RightClickMove(const FInputActionValue& InputActionValue);
	

	//인풋들을 Data들로 관리하기위함(점프,큐,스킬 등..)
	//BP > MoveActions에 박아주면 됨.
	UPROPERTY(EditAnywhere,Category="Input")
	TObjectPtr<UAuraInputConfigData> InputActions;
	

	void CursorTrace();
	TScriptInterface<IEnemyInterface> LastActor; //가장최근에 가리킨 적
	TScriptInterface<IEnemyInterface> ThisActor; //현재커서위치가 가리키는 적

	//3개의 콜백 함수
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHeld(FGameplayTag InputTag);
	
	UPROPERTY(EditDefaultsOnly,Category="Input")
	TObjectPtr<UAuraInputConfig> InputConfig; //(태그-입력)

	UPROPERTY()
	TObjectPtr<UAuraAbilitySystemComponent> AuraAbilitySystemComponent;
	UAuraAbilitySystemComponent* GetASC();
};
