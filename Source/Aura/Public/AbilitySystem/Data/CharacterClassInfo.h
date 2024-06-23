// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CharacterClassInfo.generated.h"

class UGameplayAbility;
class UGameplayEffect;

UENUM(BlueprintType) 
enum class ECharacterClass : uint8 //캐릭구분용
{
	Elementalist, //0
	Warrior, //1
	Ranger //2
};

USTRUCT(BlueprintType)
struct FCharacterClassDefaultInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, Category="Class Defaults")
	TSubclassOf<UGameplayEffect> PrimaryAttributes; //그 캐릭터만의 고유속성
};
/**
 * 
 */
UCLASS()
class AURA_API UCharacterClassInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Character Class Defaults") //<캐릭터, 정보> 맵
	TMap<ECharacterClass, FCharacterClassDefaultInfo> CharacterClassInformation;

	//공통속성
	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> SecondaryAttributes;

	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TSubclassOf<UGameplayEffect> VitalAttributes;

	UPROPERTY(EditDefaultsOnly, Category="Common Class Defaults")
	TArray<TSubclassOf<UGameplayAbility>> CommonAbilities;
	
	FCharacterClassDefaultInfo GetClassDefaultInfo(ECharacterClass CharacterClass); //@Getter
	
};
