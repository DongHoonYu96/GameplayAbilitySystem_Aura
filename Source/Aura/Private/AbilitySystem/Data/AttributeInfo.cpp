// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AttributeInfo.h"

FAuraAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	for(const FAuraAttributeInfo& Info : AttributeInformation)
	{
		if(Info.AttributeTag==AttributeTag) //배열안에 일치하는 태그가 있으면 그 정보 리턴
		{
			return Info;
		}
	}

	if(bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can`t find Info for AttributeTag [%s] on AttributeInfo [%s]."),*AttributeTag.ToString(),*GetNameSafe(this));
		//속성이름, 이클래스의 이름
	}

	return FAuraAttributeInfo(); // 빈 속성정보 리턴
}
