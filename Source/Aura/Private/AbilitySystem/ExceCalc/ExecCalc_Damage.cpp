// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ExceCalc/ExecCalc_Damage.h"
#include "AbilitySystemComponent.h"
#include "AuraAbilityTypes.h"
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
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitChance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(CriticalHitDamage);

	DECLARE_ATTRIBUTE_CAPTUREDEF(FireResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(LightningResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(ArcaneResistance);
	DECLARE_ATTRIBUTE_CAPTUREDEF(PhysicalResistance);

	TMap<FGameplayTag,FGameplayEffectAttributeCaptureDefinition> TagsToCaptureDefs; //tag와 속성연결
	
	AuraDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, false); //아머라는 속성을 캡쳐하겠다. from target(적) 스냅샷X
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, BlockChance, Target, false); //아머라는 속성을 캡쳐하겠다.
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArmorPenetration, Source, false); //아머라는 속성을 캡쳐하겠다. from 공격자(아우라)임에 주의
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitChance, Source, false); //아머라는 속성을 캡쳐하겠다. from 공격자(아우라)임에 주의
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitResistance, Target, false); //아머라는 속성을 캡쳐하겠다. from 적(고블린)임에 주의
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, CriticalHitDamage, Source, false); //아머라는 속성을 캡쳐하겠다. from 공격자(아우라)임에 주의

		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, FireResistance, Target, false); //아머라는 속성을 캡쳐하겠다. from 공격자(아우라)임에 주의
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, LightningResistance, Target, false); //아머라는 속성을 캡쳐하겠다. from 공격자(아우라)임에 주의
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, ArcaneResistance, Target, false); //아머라는 속성을 캡쳐하겠다. from 공격자(아우라)임에 주의
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, PhysicalResistance, Target, false); //아머라는 속성을 캡쳐하겠다. from 공격자(아우라)임에 주의

		const FAuraGameplayTags& Tags = FAuraGameplayTags::Get();
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_Armor, ArmorDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_BlockChance, BlockChanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_ArmorPenetration, ArmorPenetrationDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitChance, CriticalHitChanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitResistance, CriticalHitResistanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Secondary_CriticalHitDamage, CriticalHitDamageDef);

		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Arcane, ArcaneResistanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Fire, FireResistanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Lightning, LightningResistanceDef);
		TagsToCaptureDefs.Add(Tags.Attributes_Resistance_Physical, PhysicalResistanceDef);
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
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitChanceDef); //캡쳐속성 배열에추가
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitResistanceDef); //캡쳐속성 배열에추가
	RelevantAttributesToCapture.Add(DamageStatics().CriticalHitDamageDef); //캡쳐속성 배열에추가

	RelevantAttributesToCapture.Add(DamageStatics().FireResistanceDef); //캡쳐속성 배열에추가
	RelevantAttributesToCapture.Add(DamageStatics().LightningResistanceDef); //캡쳐속성 배열에추가
	RelevantAttributesToCapture.Add(DamageStatics().ArcaneResistanceDef); //캡쳐속성 배열에추가
	RelevantAttributesToCapture.Add(DamageStatics().PhysicalResistanceDef); //캡쳐속성 배열에추가

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
	// 모든 데미지 타입에 대해 데미지 얻어오기
	float Damage = 0.f;
	for (const TTuple<FGameplayTag, FGameplayTag>& Pair  : FAuraGameplayTags::Get().DamageTypesToResistances)
	{
		//map < 데미지타입, 저항속성>
		const FGameplayTag DamageType = Pair.Key;
		const FGameplayTag ResistanceTag = Pair.Value;

		//저항속성이 TagsToCaptureDefs맵에 없는경우 예외처리, 에러시메시지
		checkf(AuraDamageStatics().TagsToCaptureDefs.Contains(ResistanceTag), TEXT("TagsToCaptureDefs doesn`t contain Tag: [%s] in ExceCalc_Damage"),
			*ResistanceTag.ToString());
		//저항태그 에 대항하는 데미지캡쳐
		const FGameplayEffectAttributeCaptureDefinition CaptureDef = AuraDamageStatics().TagsToCaptureDefs[ResistanceTag];

		float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageType);
		
		float Resistance = 0.f;
		//캡쳐된 속성값 계산
		ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(CaptureDef, EvaluationParameters, Resistance);
		//감소율은 0~100으로 제한
		Resistance = FMath::Clamp(Resistance,0.f,100.f);
		
		DamageTypeValue *= (100.f - Resistance) / 100.f; //저항%만큼 데미지 감소시키기

		Damage += DamageTypeValue;
	}
	// for(FGameplayTag DamageTypeTag : FAuraGameplayTags::Get().DamageTypes)
	// {
	// 	const float DamageTypeValue = Spec.GetSetByCallerMagnitude(DamageTypeTag);
	// 	Damage += DamageTypeValue;
	// }

	//Capture Blockchacne on target , 블록인지 결정
	//블록이면, 데미지 절반으로
	float TargetBlockChance = 0.f;
	//현재 설정되있는 BlockChance값을 얻어와서 Out변수에 집어넣음 (현재 4로 설정되있음)
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().BlockChanceDef, EvaluationParameters,TargetBlockChance );
	TargetBlockChance = FMath::Max<float>(TargetBlockChance,0.f);

	//블록확률 25%
	const bool bBlocked = FMath::RandRange(1,100) < TargetBlockChance;

	FGameplayEffectContextHandle EffectContextHandle = Spec.GetContext();
	UAuraAbilitySystemLibrary::SetIsBlockedHit(EffectContextHandle, bBlocked);
	// FGameplayEffectContext* Context = EffectContextHandle.Get(); //ptr 반환
	// FAuraGameplayEffectContext* AuraContext = static_cast<FAuraGameplayEffectContext*>(Context); //내가만든 setter 쓸려면 캐스트 필요
	// AuraContext->SetIsBlockHit(bBlocked);
	
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

	/*치명타 계산*/
	float SourceCriticalHitChance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitChanceDef, EvaluationParameters,SourceCriticalHitChance ); //치명타얻어오기
	SourceCriticalHitChance = FMath::Max<float>(SourceCriticalHitChance,0.f);

	float TargetCriticalHitResistance = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitResistanceDef, EvaluationParameters,TargetCriticalHitResistance ); //치명타얻어오기
	SourceCriticalHitChance = FMath::Max<float>(TargetCriticalHitResistance,0.f);

	float SourceCriticalHitDamage = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(DamageStatics().CriticalHitDamageDef, EvaluationParameters,SourceCriticalHitDamage ); //치명타얻어오기
	SourceCriticalHitDamage = FMath::Max<float>(SourceCriticalHitChance,0.f);
		//----------------------- 변수 캡쳐------------------------
	
	//캐릭정보얻고, 커브얻고, 레벨에따른 방관 계수값 얻기
	const FRealCurve* CriticalHitResistanceCurve = CharacterClassInfo->DamageCalculationCoefficients->FindCurve(FName("CriticalHitResistance"), FString());
	const float CriticalHitResistanceCoefficient = CriticalHitResistanceCurve->Eval(TargetCombatInterface->GetPlayerLevel()); //적의 레벨에따른 값얻기
	
	const float EffectiveCriticalHitChance = SourceCriticalHitChance-TargetCriticalHitResistance * CriticalHitResistanceCoefficient; //실제치명타확률 == 치적 - 치명타저항
	const bool bCriticalHit = FMath::RandRange(1,100) < EffectiveCriticalHitChance; //랜덤

	UAuraAbilitySystemLibrary::SetIsCriticalHit(EffectContextHandle, bCriticalHit);
	
	if(bCriticalHit) Damage = Damage * 2.f + SourceCriticalHitDamage; //치명타이면 데미지 2배 + 보너스데미지
	/* end 치명타 계산*/
	
	//실제속성변경
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive, Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
}
