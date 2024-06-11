// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraAssetManager.h"
#include "AbilitySystemGlobals.h"
#include "AuraGameplayTags.h"


UAuraAssetManager& UAuraAssetManager::Get()
{
	check(GEngine);
	UAuraAssetManager* AuraAssetManager = Cast<UAuraAssetManager>(GEngine->AssetManager);
	return *AuraAssetManager; 
}

void UAuraAssetManager::StartInitialLoading() //게임 에셋들 초기세팅하는 함수
{
	Super::StartInitialLoading();
	//Tag들 추가 하기 최적의 장소!
	FAuraGameplayTags::InitializeNativeGameplayTags();
	//할일 : AuraAssetManager를 AssetManager로 설정

	//TargetData 사용위해 필요
	UAbilitySystemGlobals::Get().InitGlobalData();
}
