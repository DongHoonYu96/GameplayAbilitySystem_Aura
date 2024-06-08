// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AuraInputConfig.h"
#include "EnhancedInputComponent.h"
#include "AuraInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:
	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
	void BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object, PressedFuncType PressFunc,
		ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc); //헤더포함 => 템플릿 함수 만들려고
};

//UserCLass에 아무 Class나 들어올수있음! 유사 template<class T>
template <class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UAuraInputComponent::BindAbilityActions(const UAuraInputConfig* InputConfig, UserClass* Object,
	PressedFuncType PressFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc) //키눌렀을때, 누르눈중, 뗏을때 바인딩해주는 함수
{
	check(InputConfig); //입력받은 매개변수 체크

	for(const FAuraInputAction& Action : InputConfig->AbilityInputActions)
	{
		if(Action.InputAction && Action.InputTag.IsValid())
		{
			if(PressFunc) //눌렀을때 콜백됨
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressFunc,Action.InputTag);
			}

			if(ReleasedFunc) 
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc,Action.InputTag);
			}
			
			if(HeldFunc) 
			{
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc,Action.InputTag);
			}
		}
	}
}
