// Fill out your copyright notice in the Description page of Project Settings.


#include "Input/AuraInputConfig.h"

const UInputAction* UAuraInputConfig::FindAbilityInputActionForTag(const FGameplayTag& InputTag,bool bLogNotFound) const
{
	for(const FAuraInputAction& Action :AbilityInputActions)
	{
		if(Action.InputAction && Action.InputTag==InputTag) //nullchk, 일치하는 태그발견시
		{
			return Action.InputAction; //그때 인풋액션 리턴
		}
	}
	if(bLogNotFound)
	{
		UE_LOG(LogTemp,Error,TEXT("Can`t find AbilityInputAction for Input Tag [%s], on InputConfig [%s]"),*InputTag.ToString(),*GetNameSafe(this))
		//그때 태그, 클래스이름 출력
	}
	return nullptr;
}
