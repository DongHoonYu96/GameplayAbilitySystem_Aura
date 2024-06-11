// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

#include "AbilitySystemComponent.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if(bIsLocallyControlled) //클라의경우 data 보내기
	{
		SendMouseCursorData();
	}
	else
	{
		//TODO : 서버에있는경우, listen target data
		//ASC => 대리인 얻기, 함수바인딩
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		const FPredictionKey ActivationPredictionKey=GetActivationPredictionKey();
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle,ActivationPredictionKey)
		.AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallback);

		//but, 데이터가 이미 전송된경우, 델리게이트가 이미 set된경우 -> 콜백호출
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle,ActivationPredictionKey);
		if(!bCalledDelegate) //데이터가 안온경우, 서버에게 기다리라고 해야함!
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
	//아래 범위가 예측(predict)되도록함.
	//서버에게 아래작업을 로컬에서 할수있도록 허용요청
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());
	
	//1. 마우스 커서의 Location 얻기
	//PC얻기 <= 부모가 갖고있는 Ability 변수
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false,CursorHit); //커서힛에 정보넣어줘
	
	//단일히트대상의 데이터를 만들어줌
	/** Target data with a single hit result, data is packed into the hit result */
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;

	FGameplayAbilityTargetDataHandle DataHandle;
	DataHandle.Add(Data);
	
	// data를 서버로보내기, AT에 ASC변수있음
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(),
		GetActivationPredictionKey(),
		DataHandle,
		FGameplayTag(),
		AbilitySystemComponent->ScopedPredictionKey);

	//능력활성화중일때만 방송해주는 함수, 방송해야되는지 체크해줌
	/** This should be called prior to broadcasting delegates back into the ability graph. This makes sure the ability is still active.  */
	if(ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallback(const FGameplayAbilityTargetDataHandle& DataHandle,FGameplayTag ActivationTag)
{
	//타겟데이터가 서버에 수신완료, 저장할필요없음, data 지우는함수임
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(),GetActivationPredictionKey());

	//능력활성화중일때만 방송해주는 함수, 방송해야되는지 체크해줌
	/** This should be called prior to broadcasting delegates back into the ability graph. This makes sure the ability is still active.  */
	if(ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle); //DataHandle 방송
	}
}
