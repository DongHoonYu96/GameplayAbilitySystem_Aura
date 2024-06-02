#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

// 생성자
AAuraEffectActor::AAuraEffectActor()
{
	// 이 액터는 매 프레임마다 업데이트되지 않음
	PrimaryActorTick.bCanEverTick = false;

	SetRootComponent(CreateDefaultSubobject<USceneComponent>("SceneRoot"));
}



void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay(); // 부모 클래스의 BeginPlay 함수를 호출합니다.


}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if(nullptr==TargetASC) return; //target이 ASC가 없는경우

	check(GameplayEffectClass); //효과가 있는지 검사
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext(); //게임플레이 효과에 대한 컨텍스트 정보를 저장하는 데 사용되는 구조체생성
	EffectContextHandle.AddSourceObject(this); //현재 개체(효과를 호출하는 액터나 구성 요소 등)를 효과 컨텍스트에 추가합니다. 이는 적용 시 효과의 소스를 식별하는 데 도움이 됩니다.
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass,1.f, EffectContextHandle);
	// 적용될 게임플레이 효과에 대한 사양이 포함된 'FGameplayEffectSpecHandle'을 생성
	TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get()); // 대상의 AbilitySystemComponent에 효과 사양(효과의 세부 사항)을 적용
	//이 프로세스를 통해 게임플레이 효과가 게임의 능력 시스템 내에서 적절하게 맥락화되고, 지정되고, 적용됩니다.

	
}
