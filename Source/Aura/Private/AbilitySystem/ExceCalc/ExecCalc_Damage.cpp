// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExceCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"

//BP에노출안할거임, c++안에서만 사용되는 원시구조체
struct AuraDamageStatics
{
	//캡쳐속성 선언 매크로 : 속성포인터가지고있음
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false); //아머라는 속성을 캡쳐하겠다. 스냅샷X
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false); //아머라는 속성을 캡쳐하겠다.

	}
	
};

//싱글톤 Getter
static const AuraDamageStatics& DamageStatics()
{
	static AuraDamageStatics Dstatics;
	return Dstatics;
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(DamageStatics().ArmorDef); //캡쳐속성 배열에추가
	RelevantAttributesToCapture.Add(DamageStatics().BlockChanceDef); //캡쳐속성 배열에추가
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC= ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC= ExecutionParams.GetTargetAbilitySystemComponent();

	//ASC가 유효한경우만 avataractor 얻기
	const AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	const AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	// Get Damage Set by Caller Magnitude
	float Damage = Spec.GetSetByCallerMagnitude(FAuraGameplayTags::Get().Damage);

	//Capture Blockchacne on target , 블록인지 결정
	//블록이면, 데미지 절반으로
	float TargetBlockChance = 0.f;
	//현재 설정되있는 BlockChance값을 얻어와서 Out변수에 집어넣음 (현재 4로 설정되있음)
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters,TargetBlockChance );
	TargetBlockChance = FMath::Max<float>(TargetBlockChance,0.f);

	//블록확률 25%
	const bool bBlocked = FMath::RandRange(1,100) < TargetBlockChance;
	if(bBlocked) Damage *= .5f;
	
	
	//실제속성변경
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
