// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagCalc/MMC_MaxHealth.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	//VigorDef 채우기
	VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute(); //AS클래스에서 getter매크로 사용
	VigorDef.AttributeSource=EGameplayEffectAttributeCaptureSource::Target; //타겟vs소스
	VigorDef.bSnapshot = false; //타이밍
	
	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	//gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.TargetTags = TargetTags;
	EvaluateParameters.SourceTags = SourceTags;

	//* vigor과 level을 얻은후, 계산해서 리턴하면됨
	float Vigor=0.f;
	GetCapturedAttributeMagnitude(VigorDef,Spec,EvaluateParameters,Vigor); //Vigor에 Vigor값 넣어줘
	Vigor=FMath::Max<float>(Vigor,0.f);
	
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	const int32 PlayerLevel = CombatInterface->GetPlayerLevel();

	return 80.f+2.5f*Vigor+10.f*PlayerLevel;
}
