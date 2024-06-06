// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	// UAuraAbilitySystemComponent 타입의 AbilitySystemComponent를 기본 하위 객체로 생성합니다.
	// "AbilitySystemComponent"라는 이름을 가진 이 컴포넌트는 캐릭터의 능력(스킬)을 관리합니다.

	AbilitySystemComponent->SetIsReplicated(true);
	// AbilitySystemComponent의 네트워크 복제를 활성화합니다.
	// 이는 멀티플레이어 게임에서 다른 플레이어들과 능력 정보를 동기화하기 위해 필요합니다.

	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	//복제모드설정
	//Mixed :  일부 효과는 서버에서만 처리하고(중요한능력), 일부는 클라이언트에서(덜중요한능력) 처리

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
	// UAuraAttributeSet 타입의 AttributeSet을 기본 하위 객체로 생성합니다.
	// "AttributeSet"이라는 이름을 가진 이 컴포넌트는 캐릭터의 다양한 속성(예: 체력, 마나, 공격력 등)을 관리합니다.

	
	// 네트워크 업데이트 주기를 설정합니다. (단위: 헤르츠, 초당 업데이트 횟수)
	// 이 경우, NetUpdateFrequency를 100으로 설정하여 1초에 100번의 네트워크 업데이트를 수행합니다.
	NetUpdateFrequency = 100.f;
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAuraPlayerState,Level); //복제된소품얻기?
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel)
{
	
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
