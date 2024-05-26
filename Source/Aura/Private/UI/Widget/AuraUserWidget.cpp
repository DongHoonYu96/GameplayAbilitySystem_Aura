// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/AuraUserWidget.h"

void UAuraUserWidget::SetWidgetController(UObject* InWidgetController)
{
	//의존성 주입 (초기화)
	WidgetController=InWidgetController;
	WidgetControllerSet(); //BP에서 구현한 함수 실행할것임.
}
