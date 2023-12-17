// 프로젝트 설정의 Description 페이지에 저작권 고지를 작성합니다.
// (프로젝트 설정 내에 있는 Description 페이지에서 저작권 고지를 작성해야 합니다.)
#include "Character/AuraEnemy.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"

AAuraEnemy::AAuraEnemy()
{
	// 가시성 콜리전 채널(ECollisionChannel::ECC_Visibility)에 대한 메시 콜리전 응답을 ECollisionResponse::ECR_Block으로 설정합니다.
	// 이래야 윤곽선나옴
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	// UAuraAbilitySystemComponent 타입의 AbilitySystemComponent를 기본 하위 객체로 생성합니다.
	// "AbilitySystemComponent"라는 이름을 가진 이 컴포넌트는 캐릭터의 능력(스킬)을 관리합니다.

	AbilitySystemComponent->SetIsReplicated(true);
	// AbilitySystemComponent의 네트워크 복제를 활성화합니다.
	// 이는 멀티플레이어 게임에서 다른 플레이어들과 능력 정보를 동기화하기 위해 필요합니다.

	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	//복제모드설정
	//Mixed :  일부 효과는 서버에서만 처리하고(중요한능력), 일부는 클라이언트에서(덜중요한능력) 처리

	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
	// UAuraAttributeSet 타입의 AttributeSet을 기본 하위 객체로 생성합니다.
	// "AttributeSet"이라는 이름을 가진 이 컴포넌트는 캐릭터의 다양한 속성(예: 체력, 마나, 공격력 등)을 관리합니다.

}

void AAuraEnemy::HighlightActor()
{
	// 엔진에서 직접적으로 체크하고 설정하는 것과 동일한 방식으로 메시의 커스텀 깊이 렌더링을 활성화하고, 커스텀 깊이 값을 설정합니다.
	GetMesh()->SetRenderCustomDepth(true);
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);

	// Weapon(무기)의 커스텀 깊이 렌더링을 활성화합니다.
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
}

void AAuraEnemy::UnHighlightActor()
{
	// 메시와 Weapon(무기)의 커스텀 깊이 렌더링을 비활성화합니다.
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}
