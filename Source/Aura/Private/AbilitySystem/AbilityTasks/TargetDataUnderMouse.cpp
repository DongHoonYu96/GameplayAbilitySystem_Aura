// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{
	//부모super호출 : 로그만찍음 -> 필요없음

	//1. 마우스 커서의 Location 얻기
	//PC얻기 <= 부모가 갖고있는 Ability 변수
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false,CursorHit); //커서힛에 정보넣어줘
	ValidData.Broadcast(CursorHit.Location); //그 위치 방송
}
