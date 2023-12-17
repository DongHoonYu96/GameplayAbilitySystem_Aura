// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AuraAttributeSet.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	// UAuraAttributeSet 클래스의 생성자
	UAuraAttributeSet();

	// 네트워크 복제를 위한 속성 설정 함수
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// 체력 속성, 네트워크 복제됨, 변경 시 OnRep_Health 함수 호출
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Vital Attributes")
	FGameplayAttributeData Health;

	// 최대 체력 속성, 네트워크 복제됨, 변경 시 OnRep_MaxHealth 함수 호출
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Vital Attributes")
	FGameplayAttributeData MaxHealth;

	// 마나 속성, 네트워크 복제됨, 변경 시 OnRep_Mana 함수 호출
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Mana, Category = "Vital Attributes")
	FGameplayAttributeData Mana;

	// 최대 마나 속성, 네트워크 복제됨, 변경 시 OnRep_MaxMana 함수 호출
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMana, Category = "Vital Attributes")
	FGameplayAttributeData MaxMana;

	// 체력이 변경될 때 호출되는 함수
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth) const;

	// 최대 체력이 변경될 때 호출되는 함수
	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const;

	// 마나가 변경될 때 호출되는 함수
	UFUNCTION()
	void OnRep_Mana(const FGameplayAttributeData& OldMana) const;

	// 최대 마나가 변경될 때 호출되는 함수
	UFUNCTION()
	void OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const;

};
