// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExceCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AuraGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemLibrary.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Interaction/CombatInterface.h"

//BP에노출안할거임, c++안에서만 사용되는 원시구조체
struct AuraDamageStatics
{
	//캡쳐속성 선언 매크로 : 속성포인터가지고있음
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArmorPenetration);
	DECLARE_ATTRIBUTE_CAPTUREDEF(BlockChance);
	
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false); //아머라는 속성을 캡쳐하겠다. from target(적) 스냅샷X
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false); //아머라는 속성을 캡쳐하겠다.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false); //아머라는 속성을 캡쳐하겠다. from 공격자(아우라)임에 주의
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
	RelevantAttributesToCapture.Add(DamageStatics().ArmorPenetrationDef); //캡쳐속성 배열에추가

}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	const UAbilitySystemComponent* SourceASC= ExecutionParams.GetSourceAbilitySystemComponent();
	const UAbilitySystemComponent* TargetASC= ExecutionParams.GetTargetAbilitySystemComponent();

	//ASC가 유효한경우만 avataractor 얻기
	AActor* SourceAvatar = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetAvatar = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	//레벨을 얻기위해 인터페이스로 만듬
	ICombatInterface* SourceCombatInterface = Cast<ICombatInterface>(SourceAvatar);
	ICombatInterface* TargetCombatInterface = Cast<ICombatInterface>(TargetAvatar);

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

	//방어력 얻어오기
	float TargetArmor = 0.f;
	//현재 설정되있는 BlockChance값을 얻어와서 Out변수에 집어넣음 (현재 4로 설정되있음)
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorDef, EvaluationParameters,TargetArmor );
	TargetBlockChance = FMath::Max<float>(TargetArmor,0.f);

	//방관 얻어오기
	float SourceArmorPenetration = 0.f;
	//현재 설정되있는 BlockChance값을 얻어와서 Out변수에 집어넣음 (현재 4로 설정되있음)
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().ArmorPenetrationDef, EvaluationParameters,SourceArmorPenetration );
	SourceArmorPenetration = FMath::Max<float>(SourceArmorPenetration,0.f);

	//캐릭정보얻고, 커브얻고, 레벨에따른 방관 계수값 얻기
	const UCharacterClassInfo* CharacterClassInfo = UAuraAbilitySystemLibrary::GetCharacterClassInfo(SourceAvatar); //소스아바타(아우라)의 정보얻기
	const FRealCurve* ArmorPenetrationCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("ArmorPenetration"), FString());
	const float ArmorPenetrationCoefficient = ArmorPenetrationCurve->Eval(SourceCombatInterface->GetPlayerLevel()); //아우라의 레벨에따른 값얻기

	// FString TheFloatStr = FString::SanitizeFloat(ArmorPenetrationCoefficient);
	// GEngine->AddOnScreenDebugMessage(1,8.f,FColor::Blue,TheFloatStr); //디버깅

	//방관은 방어력을 %로 무시함 , ex : 방관100이면 방어 100% 무시함 / 뒤의계수 == op 조정
	if(SourceArmorPenetration>100.f) SourceArmorPenetration=100.f;
	const float EffectiveArmor = TargetArmor *= (100 - SourceArmorPenetration * ArmorPenetrationCoefficient) / 100.f; //유효방어력 = 방관계산후 방어력

	//캐릭정보얻고, 커브얻고, 레벨에따른 방관 계수값 얻기
	const FRealCurve* EffectiveArmorCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("EffectiveArmor"), FString());
	const float EffectiveArmorCoefficient = EffectiveArmorCurve->Eval(TargetCombatInterface->GetPlayerLevel()); //적의 레벨에따른 값얻기
	
	// FString TheFloatStr2 = FString::SanitizeFloat(EffectiveArmorCoefficient);
	// GEngine->AddOnScreenDebugMessage(1,8.f,FColor::Blue,TheFloatStr); //디버깅
	
	Damage *= (100-EffectiveArmor * EffectiveArmorCoefficient) / 100.f;
	
	//실제속성변경
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
