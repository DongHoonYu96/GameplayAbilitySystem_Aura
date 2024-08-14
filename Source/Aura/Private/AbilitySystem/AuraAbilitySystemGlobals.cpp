/// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemGlobals.h"

#include "AuraAbilityTypes.h"

FGameplayEffectContext* UAuraAbilitySystemGlobals::AllocGameplayEffectContext() const
{
	//내가만든 EffectContext의 새로운 인스턴스 리턴.
	return new FAuraGameplayEffectContext();
}
