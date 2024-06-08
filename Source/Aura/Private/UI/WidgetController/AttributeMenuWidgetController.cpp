// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AuraGameplayTags.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet); //부모인 위젯컨트롤러가 갖고있음
	check(AttributeInfo) //DA가 set되었는지 확인
	for(auto& Pair : AS->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda( //해당속성 : str,int에 대해
[this, Pair](const FOnAttributeChangeData& Data)//속성변경 데이터 시그니쳐 가져와서
		{
			BroadcastAttributeInfo(Pair.Key,Pair.Value()); //<태그, 속성>
		}
	);
	}
	
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet); //부모인 위젯컨트롤러가 갖고있음

	check(AttributeInfo) //DA가 set되었는지 확인
	//리팩토링
	for(auto& Pair : AS->TagsToAttributes) //AS의 <태그,속성:델리게이트> 맵 순회
	{
		BroadcastAttributeInfo(Pair.Key,Pair.Value());

	}
	
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
	const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag); //태그에 맞는 속성 찾기
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet); //속성의실제값을 get , 위젯컨트롤러가 AS 가지고있음
	AttributeInfoDelegate.Broadcast(Info); //정보의 변경을 방송
}
