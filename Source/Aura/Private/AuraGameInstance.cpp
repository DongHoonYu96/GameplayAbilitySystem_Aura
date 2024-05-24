// Fill out your copyright notice in the Description page of Project Settings.


#include "AuraGameInstance.h"

#include "AuraObject.h"

void UAuraGameInstance::Init()
{
	Super::Init();

	auto Obj1= NewObject<UAuraObject>();

	UE_LOG(LogTexture,Log,TEXT("obj1`s name is : %s"),*Obj1->GetObjectName());
	//문자열 접근시 *으로 접근해야함

	auto NameProperty = UAuraObject::StaticClass()->FindPropertyByName(TEXT("Name"));

	FString CompileTimeObjName;
	if(nullptr!=NameProperty)
	{
		//컨테이너안에서 이름가져와서 &변수에 넣어줘
		NameProperty->GetValue_InContainer(Obj1,&CompileTimeObjName);
		UE_LOG(LogTexture,Log,TEXT("CompileTimeObjName`s name is : %s"),*CompileTimeObjName);
		//문자열 접근시 *으로 접근해야함
		
	}
}
