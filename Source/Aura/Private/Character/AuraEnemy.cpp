// 프로젝트 설정의 Description 페이지에 저작권 고지를 작성합니다.
// (프로젝트 설정 내에 있는 Description 페이지에서 저작권 고지를 작성해야 합니다.)
#include "Character/AuraEnemy.h"
#include "Aura/Aura.h"

AAuraEnemy::AAuraEnemy()
{
	// 가시성 콜리전 채널(ECollisionChannel::ECC_Visibility)에 대한 메시 콜리전 응답을 ECollisionResponse::ECR_Block으로 설정합니다.
	// 이래야 윤곽선나옴
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
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
