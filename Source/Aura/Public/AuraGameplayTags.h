// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

/**
 * AuraGameplayTags
 *
 * 싱글톤, 포함하는 native GETags
 */

struct FAuraGameplayTags
{
public:
 static const FAuraGameplayTags& Get(){ return GameplayTags; }
 static void InitializeNativeGameplayTags(); //@Setter

 FGameplayTag Attributes_Secondary_Armor;
 
protected:

private:
 static FAuraGameplayTags GameplayTags;
};