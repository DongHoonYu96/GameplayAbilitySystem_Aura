// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagCalc/MMC_MaxMana.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxMana::UMMC_MaxMana()
{
	//VigorDef 채우기
	IntelligenceDef.AttributeToCapture = UAuraAttributeSet::GetIntelligenceAttribute(); //AS클래스에서 getter매크로 사용
	IntelligenceDef.AttributeSource=EGameplayEffectAttributeCaptureSource::Target; //타겟vs소스
	IntelligenceDef.bSnapshot = false; //타이밍
	
	RelevantAttributesToCapture.Add(IntelligenceDef);
}

float UMMC_MaxMana::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	//gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.TargetTags = TargetTags;
	EvaluateParameters.SourceTags = SourceTags;

	//* vigor과 level을 얻은후, 계산해서 리턴하면됨
	float Intelligence=0.f;
	GetCapturedAttributeMagnitude(IntelligenceDef,Spec,EvaluateParameters,Intelligence); //Vigor에 Vigor값 넣어줘
	Intelligence=FMath::Max<float>(Intelligence,0.f);
	
	ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	const int32 PlayerLevel = CombatInterface->GetPlayerLevel();

	return 50.f+2.5f*Intelligence+15.f*PlayerLevel;
}
