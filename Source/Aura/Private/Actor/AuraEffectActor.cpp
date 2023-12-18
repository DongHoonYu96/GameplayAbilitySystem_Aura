#include "Actor/AuraEffectActor.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/SphereComponent.h"

// 생성자
AAuraEffectActor::AAuraEffectActor()
{
	// 이 액터는 매 프레임마다 업데이트되지 않음
	PrimaryActorTick.bCanEverTick = false;

	// 메시 컴포넌트 생성 및 초기화
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	// 메시를 루트 컴포넌트로 설정
	SetRootComponent(Mesh);

	// 구형 충돌 컴포넌트 생성 및 초기화
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	// 구형 컴포넌트를 루트 컴포넌트에 첨부
	Sphere->SetupAttachment(GetRootComponent());
}

// (물약이) 다른 액터(플레이어)와 겹쳤을 때 호출되는 함수(v1)
// 나중에 수정할것임
void AAuraEffectActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//TODO: Change this to apply a Gameplay Effect. For now, using const_cast as a hack!
	// 능력 시스템 인터페이스를 구현한 다른 액터를 찾아 체력 증가 처리
	if (IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{ //IAbilitySystemInterface를 구현한 OtherActor에 대해 체크하고, 이를 통해 해당 액터의 능력 시스템 컴포넌트를 가져옵니다.
		const UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(ASCInterface->GetAbilitySystemComponent()->GetAttributeSet(UAuraAttributeSet::StaticClass()));
		
		// 체력 속성을 수정 가능하게 하기 위해 const_cast 사용 (주의: 일반적으로 권장되지 않는 방법)
		UAuraAttributeSet* MutableAuraAttributeSet = const_cast<UAuraAttributeSet*>(AuraAttributeSet);
		// 체력을 증가시키고 이 액터를 파괴
		MutableAuraAttributeSet->SetHealth(AuraAttributeSet->GetHealth() + 25.f);
		Destroy();
	}
}

// 다른 액터와의 겹침이 끝났을 때 호출되는 함수 (현재 구현되지 않음)
void AAuraEffectActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay(); // 부모 클래스의 BeginPlay 함수를 호출합니다.

	Sphere->OnComponentBeginOverlap.AddDynamic(this, &AAuraEffectActor::OnOverlap); // 충돌 시작 이벤트에 OnOverlap 함수를 연결합니다.
	Sphere->OnComponentEndOverlap.AddDynamic(this, &AAuraEffectActor::EndOverlap); // 충돌 종료 이벤트에 EndOverlap 함수를 연결합니다.
}