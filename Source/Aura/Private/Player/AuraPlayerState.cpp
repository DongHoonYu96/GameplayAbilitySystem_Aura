// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerState.h"

AAuraPlayerState::AAuraPlayerState()
{
	// 네트워크 업데이트 주기를 설정합니다. (단위: 헤르츠, 초당 업데이트 횟수)
	// 이 경우, NetUpdateFrequency를 100으로 설정하여 1초에 100번의 네트워크 업데이트를 수행합니다.
	NetUpdateFrequency = 100.f;
}
