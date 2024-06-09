// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/AuraProjectileSpell.h"

#include "Actor/AuraProjectile.h"
#include "Interaction/CombatInterface.h"
#include "Kismet/KismetSystemLibrary.h"

void UAuraProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                           const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                           const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	//발사체는 서버에서생성, 클라는 복제본을 보게될거임
	//발사체가 서버에있는지 어케암?
	const bool bIsServer = HasAuthority(&ActivationInfo);
	if(! bIsServer) return ; //발사체가 서버있는경우만 발사체 생성

	ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetAvatarActorFromActorInfo()); //해당액터가 전투인터를 구현하면, 얻을수있음
	if(CombatInterface)
	{
		const FVector SocketLocation = CombatInterface->GetCombatSocketLocation();
		FTransform SpawnTransform; //발사체생성할 위치 == 지팡이끝 / but, 캐릭터에의존하면안됨, 전투인터페이스에서 소켓위치를 반환받으면 좋겠음.
		SpawnTransform.SetLocation(SocketLocation);
		//TODO : 발사체 회전설정
		
		//<생성하고자하는타입> (클래스, 날아갈 회전값, 수행자, 수행할폰, 충돌-겹침 상관없이 항상생성)
		AAuraProjectile* Projectile = GetWorld()->SpawnActorDeferred<AAuraProjectile>(
			ProjectileClass,
			SpawnTransform,
			GetOwningActorFromActorInfo(),
			Cast<APawn>(GetOwningActorFromActorInfo()),
			ESpawnActorCollisionHandlingMethod::AlwaysSpawn);

		//TODO: 발사체에게 Gameplay Effect Spec 부여 => 데미지 주기위해
		
		
		//발사체 생성완료하려면 발사체를 가져와야함(?)
		Projectile->FinishSpawning(SpawnTransform);
	}
	
	
}
