// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "AuraGameplayTags.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
}

void UAuraProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation)
{
	//GEngine->AddOnScreenDebugMessage(1,8.f,FColor::Blue,FString("Effect Applied!"));
	//발사체는 서버에서생성, 클라는 복제본을 보게될거임
	//발사체가 서버에있는지 어케암?
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority(); //아바타액텨 ==불 의 권위확인
	if(! bIsServer) return ; //발사체가 서버있는경우만 발사체 생성

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()); //해당액터가 전투인터를 구현하면, 얻을수있음
	if(CombatInterface)
	{
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
		FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation(); //방향벡터의 회전각 구하기
		Rotation.Pitch=0.f; // 지면과 평행하게 움직이도록
		
		FTransform SpawnTransform; //발사체생성할 위치 == 지팡이끝 / but, 캐릭터에의존하면안됨, 전투인터페이스에서 소켓위치를 반환받으면 좋겠음.
		SpawnTransform.SetLocation(SocketLocation); 
		
		//TODO : 발사체 회전설정
		SpawnTransform.SetRotation(Rotation.Quaternion());  //쿼터니언 : 짐벌락없는 회전표현방법
		
		//<생성하고자하는타입> (클래스, 날아갈 회전값, 수행자, 수행할폰, 충돌-겹침 상관없이 항상생성)
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		//TODO: 발사체에게 Gameplay Effect Spec 부여 => 데미지 주기위해
		//1. SpecHandle 만들기
		UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetAvatarActorFromActorInfo());
		FGameplayEffectContextHandle EffectContextHandle = SourceASC->MakeEffectContext();

		//AS.props에 정보집어넣기
		EffectContextHandle.SetAbility(this);
		EffectContextHandle.AddSourceObject(Projectile);
		TArray<TWeakObjectPtr<AActor>> Actors;
		Actors.Add(Projectile);
		EffectContextHandle.AddActors(Actors);
		FHitResult HitResult;
		HitResult.Location = ProjectileTargetLocation;
		EffectContextHandle.AddHitResult(HitResult);

		const FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), EffectContextHandle);

		//GE_Damage에 사용할 Caller 설정
		FAuraGameplayTags GameplayTags = FAuraGameplayTags::Get();
		const float ScaledDamage = Damage.GetValueAtLevel(20); //현레벨에 맞는 데미지를 얻어옴 (json으로 설정한그값) GetAbilityLevel();
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.Damage, ScaledDamage);
		
		Projectile->DamageEffectSpecHandle=SpecHandle; //set
		
		
		//발사체 생성완료하려면 발사체를 가져와야함(?)
		/** Called to finish the spawning process, generally in the case of deferred spawning */
		Projectile->FinishSpawning(SpawnTransform);
	}
}
