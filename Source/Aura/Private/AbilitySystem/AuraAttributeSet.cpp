// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AuraAttributeSet.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UAuraAttributeSet::UAuraAttributeSet()
{
	//Setter 사용
	InitHealth(50.f);
	InitMaxHealth(100.f);
	InitMana(50.f);
	InitMaxMana(100.f);
}

// !네트워크에 복제할 속성들을 등록하는 함수! 등록안해주면 빌드에러
void UAuraAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	// 부모 클래스의 같은 함수를 호출하여 기본 설정을 상속받음
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// 'Health'와 'MaxHealth' 속성을 네트워크 복제하도록 설정
	// 값이 변경될 때마다 알림이 발생하도록 설정
	//UAuraAttributeSet: 이는 매크로가 적용될 클래스입니다. 이 경우 UAuraAttributeSet 클래스 내의 멤버 변수를 복제할 것임을 의미합니다.
	//MaxMana: 이는 네트워크를 통해 복제할 멤버 변수의 이름입니다. MaxMana라는 이름의 멤버 변수가 변경될 때, 그 변경 사항이 네트워크를 통해 다른 클라이언트에게 전달됩니다.
	//COND_None: 이는 복제 조건을 나타냅니다. COND_None은 별다른 조건 없이 항상 복제하겠다는 것을 의미합니다.
	//REPNOTIFY_Always: 이는 복제 알림 방식을 나타냅니다. REPNOTIFY_Always는 변수가 변경될 때마다 항상 알림이 발생하도록 설정합니다.
	//즉, MaxMana 변수의 값이 변경될 때마다, 그 변경 사항에 대한 알림이 발생하고 관련된 함수가 호출됩니다.
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Mana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxMana, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, Identity, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAuraAttributeSet, MaxIdentity, COND_None, REPNOTIFY_Always);
}

void UAuraAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetHealthAttribute()) //속성이 체력인경우
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth()); //new val을 범위제한함(0~최대체력)
	}
	if (Attribute == GetManaAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMana());
	}
}

void UAuraAttributeSet::SetEffectProperties(const FGameplayEffectModCallbackData& Data, FEffectProperties& Props) const
{
	// Soruce == causer of effect , Target = target of the effect(owner of AS)


	Props.EffectContextHandle = Data.EffectSpec.GetContext();

	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent();

	//널체크
	if(IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get();

		//소스의 컨트롤러얻기
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get();

		//소스의 컨트롤러가 널인경우, (아바타)액터를 폰에 캐스팅하여, 액터자체어서 컨트롤러를 가져옴
		if(Props.SourceController==nullptr &&Props.SourceAvatarActor!=nullptr)
		{
			if(const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}
		if(Props.SourceController!=nullptr)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	if(Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
	
}
void UAuraAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	FEffectProperties Props;
	SetEffectProperties(Data,Props);

	if(Data.EvaluatedData.Attribute==GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(),0.f,GetMaxHealth()));
	}
	if(Data.EvaluatedData.Attribute==GetManaAttribute())
	{
		SetMana(FMath::Clamp(GetMana(),0.f,GetMaxMana()));
	}
}

// 'Health' 속성이 변경될 때 호출되는 함수
void UAuraAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	// 'Health' 속성의 변경을 처리
	// 핵이면, 롤백용으로 OldVal을 알아야함!
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Health, OldHealth);
}

// 'MaxHealth' 속성이 변경될 때 호출되는 함수
void UAuraAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	// 'MaxHealth' 속성의 변경을 처리
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxHealth, OldMaxHealth);
}

// 'Mana' 속성이 변경될 때 호출되는 함수
void UAuraAttributeSet::OnRep_Mana(const FGameplayAttributeData& OldMana) const
{
	// 'Mana' 속성의 변경을 처리
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Mana, OldMana);
}

// 'MaxMana' 속성이 변경될 때 호출되는 함수
void UAuraAttributeSet::OnRep_MaxMana(const FGameplayAttributeData& OldMaxMana) const
{
	// 'MaxMana' 속성의 변경을 처리
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxMana, OldMaxMana);
}

void UAuraAttributeSet::OnRep_MaxIdentity(const FGameplayAttributeData& OldMaxIdentity) const
{
	// 'Health' 속성의 변경을 처리
	// 핵이면, 롤백용으로 OldVal을 알아야함!
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, MaxIdentity, OldMaxIdentity);
}

void UAuraAttributeSet::OnRep_Identity(const FGameplayAttributeData& OldIdentity) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Identity, OldIdentity);
}

void UAuraAttributeSet::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Strength, OldStrength);
}

void UAuraAttributeSet::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Intelligence, OldIntelligence);

}

void UAuraAttributeSet::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Resilience, OldResilience);
}

void UAuraAttributeSet::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAuraAttributeSet, Vigor, OldVigor);
}

