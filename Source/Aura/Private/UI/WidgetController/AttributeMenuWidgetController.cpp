// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AuraGameplayTags.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet); //부모인 위젯컨트롤러가 갖고있음

	check(AttributeInfo) //DA가 set되었는지 확인

	//기존
	// FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(FAuraGameplayTags::Get().Attributes_Primary_Strength); //일치태그 정보를 리턴
	// Info.AttributeValue=AS->GetStrength(); //속성정보 set 완료, value는 직접넣기로 설계함
	// AttributeInfoDelegate.Broadcast(Info); //set완료후 방송해야함

	//리팩토링
	for(auto& Pair : AS->TagsToAttributes) //AS의 <태그,속성:델리게이트> 맵 순회
	{
		FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Pair.Key); //태그에 맞는 속성 찾기
		FGameplayAttribute Attr = Pair.Value(); //FGameplayAttribute에 접근
		Info.AttributeValue=Attr.GetNumericValue(AS); //GameplayAttribute의 껍질을까서 실제 값에 접근
		//(Attr이 스태틱 함수에서 반환되기떄문에 -> 이속성이 연괸딘 AS가 누구인지 알려줘야함)

		AttributeInfoDelegate.Broadcast(Info);
	}
	
}
