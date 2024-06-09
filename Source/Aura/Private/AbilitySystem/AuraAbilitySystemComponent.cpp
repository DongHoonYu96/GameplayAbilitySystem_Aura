// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraGameplayTags.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&UAuraAbilitySystemComponent::EffectApplied);
	
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	//모든 시작능력에대해
	for(const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		//능력의 스펙을 얻고
		FGameplayAbilitySpec AbilitySpec=FGameplayAbilitySpec(AbilityClass,1);
		//아우라GA에서 파생된 클래스의 경우라면
		if(const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			//시작입력태그를 동적능력태그에 추가
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag);//동적능력태그 => 런타입에 추가제거가능 => 키바꾸기가능
			//능력 활성화
			GiveAbility(AbilitySpec);
		}
		
		//GiveAbility(AbilitySpec);
		//GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

//태그에 맞는 능력활성화 && 능력에게 입력이 눌려졌다고 알리는 함수
void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if(!InputTag.IsValid()) return; //태그검사

	//활성화가능한 능력에대해
	for(FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		//입력태그와 동일한 능력을 활성화시킴!
		if(AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			//이미활성화된경우 -> pass
			//활성화안된경우 -> 활성화
			if(!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
			AbilitySpecInputPressed(AbilitySpec); //능력에게 입력이 눌려졌다고 알림
		}
	}
}

//입력이 더이상 안눌린다고 알리는 함수
void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if(!InputTag.IsValid()) return; //태그검사

	//활성화가능한 능력에대해
	for(FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		//입력태그와 동일한 능력을 활성화시킴!
		if(AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag))
		{
			AbilitySpecInputReleased(AbilitySpec); //능력에게 입력이 더이상 안눌린다고 알림

		}
	}
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer); ///** Appends all tags that apply to this gameplay effect spec */
	EffectAssetTags.Broadcast(TagContainer);
}
