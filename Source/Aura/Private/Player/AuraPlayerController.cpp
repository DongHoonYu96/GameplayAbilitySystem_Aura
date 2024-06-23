// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/AuraPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AuraGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Data/AuraInputConfigData.h"
#include "GameFramework/Character.h"
#include "Input/AuraInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "UI/Widget/DamageTextComponent.h"

AAuraPlayerController::AAuraPlayerController()
{
	bReplicates = true; // 해당 컨트롤러가 네트워크 상에서 복제될 수 있도록 함

	Spline=CreateDefaultSubobject<USplineComponent>("Spline");
}

void AAuraPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	
	AutoRun(); //리팩토링
}

//대상 캐릭터위에 데미지 띄워주는 함수
//RPC함수 : 서버에서 호출가능
void AAuraPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter)
{
	//대상캐릭터위치에 위젯생성함
	if(IsValid(TargetCharacter) && DamageTextComponentClass)
	{
		UDamageTextComponent* DamageText= NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent(); //위젯을 동적생성 -> 수동으로 컴포넌트 등록
		//Text를 대상캐릭터의 루트에 붙이기
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(),FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform); //계속못따라다니도록 분리, 그냥 사라질숫자임
		DamageText->SetDamageText(DamageAmount); //Text값 설정
	}
}

void AAuraPlayerController::CursorTrace()
{
	// 레이캐스트를 통해 마우스 커서 아래에 있는 객체에 대한 충돌 결과를 가져옵니다.
	// ECC_Visibility 콜리전 채널을 사용하여 가시성이 있는 오브젝트를 대상으로 합니다.
	// 두 번째 매개변수는 복합 콜리전에 대한 단일 충돌만 반환해야 하는지 여부를 나타냅니다.
	// CursorHit 변수에 결과가 저장됩니다.
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);

	// 마우스 커서 아래에 아무것도 막히지 않았다면 함수를 종료합니다.
	if (!CursorHit.bBlockingHit) return;

	// LastActor를 현재 ThisActor로 설정합니다.
	// 새로계산 전에 저장목적
	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());
	if(LastActor!=ThisActor)
	{
		if (LastActor) LastActor->UnHighlightActor(); //이전적을 해제
		if (ThisActor) ThisActor->HighlightActor(); //이번적을 하일라이트
	}
	
}

void AAuraPlayerController::AbilityInputTagPressed(FGameplayTag InputTag) //처음누름
{
	//LMB인 경우
	if(InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		//ThisActor 가있으면 타겟팅중임
		bTargeting = ThisActor ? true : false;
		bAutoRunning=false;
	}
}

void AAuraPlayerController::AbilityInputTagReleased(FGameplayTag InputTag) //뗌
{
	//LMB가 아닌 일반태그인 경우, 그냥 능력을 활성화하고 끝냄
	if(!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if(GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag); //ASC의 함수호출
		}
		return;
	}

	//쉬프트키랑 상관없이, 마우스놓으면 ASC에 알림
	if(GetASC()) GetASC()->AbilityInputTagReleased(InputTag); //ASC의 함수호출 (입력이더이상 안눌린다고 알림)
	
	//아래의경우 거기로 부드럽게 이동 구현
	if(!bTargeting && !bShiftKeyDown)
	{
		const APawn* ControlledPawn = GetPawn();
		//짧게누른경우
		if(FollowTime<=ShortPressThreshold && ControlledPawn)
		{
			//(시작위치, 끝위치)
			UNavigationPath* NavPath =
				UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(),CachedDestination);
			if(NavPath)
			{
				Spline->ClearSplinePoints();
				for(const FVector& PointLoc : NavPath->PathPoints) //목적지로가는 경로들에 대해
					{
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World); //스플라인 지점 추가
					DrawDebugSphere(GetWorld(),PointLoc,8.f,8,FColor::Green, false, 5.f); //디버깅용
					}
				CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num()-1]; //이상한곳 클릭방지, 클릭지점을 PathPoint 위의 점으로 강제설정
				bAutoRunning=true; //오토러닝 켜주기
			}
			
		}
		FollowTime=0.f; //팔로우타임리셋
		bTargeting=false; //타게팅여부 리셋
	}

}

void AAuraPlayerController::AbilityInputTagHeld(FGameplayTag InputTag) //누르는중
{
	//LMB가 아닌 일반태그인 경우, 그냥 능력을 활성화하고 끝냄
	if(!InputTag.MatchesTagExact(FAuraGameplayTags::Get().InputTag_LMB))
	{
		if(GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag); //ASC의 함수호출
		}
		return;
	}
	
	//LMB인경우, 달리는 문제를 해결해야함
	//타겟팅중인 적이 있는 경우 or 쉬프트키누르눈중
	if(bTargeting || bShiftKeyDown)
	{
		if(GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag); //ASC의 함수호출 (능력활성화 : 불발사)
		}
	}
	else //타겟팅중인 적이 없는경우
	{
		FollowTime+=GetWorld()->GetDeltaSeconds(); //몇초눌렀는지 저장

		
		if(CursorHit.bBlockingHit)
		{
			CachedDestination = CursorHit.ImpactPoint; //커서눌린위치를 저장
		}

		//이동, 이동하려면 폰이필요함
		if(APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal(); //b-a 하면 방향이나옴
			ControlledPawn->AddMovementInput(WorldDirection); //그 방향으로 이동
		}
	}
}

//싱글톤 Getter
UAuraAbilitySystemComponent* AAuraPlayerController::GetASC()
{
	if(AuraAbilitySystemComponent == nullptr)
	{
		AuraAbilitySystemComponent =
			Cast<UAuraAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AuraAbilitySystemComponent;
}

void AAuraPlayerController::AutoRun()
{
	if(!bAutoRunning) return;
	if(APawn* ControlledPawn = GetPawn())
	{
		//캐릭이 스플라인위에 없을수도있음 -> 캐릭에서 스플라인에 가까운 위치찾기
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		//방향찾기
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		//자동이동 범위밖인경우 오토런을 꺼주기
		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if(DistanceToDestination<=AutoRunAcceptanceRadius)
		{
			bAutoRunning=false;
		}
	}
}

// 게임 시작 시 호출되는 함수
void AAuraPlayerController::BeginPlay()
{
	Super::BeginPlay(); // 부모 클래스의 BeginPlay 함수 호출

	check(AuraContext); // AuraContext의 널(Null) 검사

	// Enhanced Input Subsystem을 사용하여 특정 입력 매핑 콘텍스트를 추가하는 부분
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	//check(Subsystem); // 서브시스템이 유효한지 확인
	//Subsystem->AddMappingContext(AuraContext, 0); // AuraContext를 서브시스템에 추가
	if (Subsystem) //멀티플레이어에서 작동하도록 check 미사용!
	{
		Subsystem->AddMappingContext(AuraContext, 0);
	}
	
	// 마우스 커서 관련 설정
	bShowMouseCursor = true; // 마우스 커서를 화면에 표시
	DefaultMouseCursor = EMouseCursor::Default; // 기본 마우스 커서를 설정

	// 입력 모드 설정
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // 뷰포트에 마우스 고정 비활성화
	InputModeData.SetHideCursorDuringCapture(false); // 입력 캡처 중에 마우스 커서 숨김 비활성화
	SetInputMode(InputModeData); // 입력 모드 설정
}

void AAuraPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent(); // 부모 클래스의 SetupInputComponent() 함수 호출

	// InputComponent를 UEnhancedInputComponent 형으로 변환하고 검사합니다. -> 를 상속받은 AuraInput으로 cast
	//Get 인핸스드 인풋 컴포넌트
	UAuraInputComponent* AuraInputComponent = CastChecked<UAuraInputComponent>(InputComponent);

	// MoveAction이 발생할 때 AAuraPlayerController 클래스의 Move() 함수를 바인딩합니다.
	//옵션 : 트리거(누르고있을때), 컴플리트(눌렀다 뗏을때)
	//특정키가(에디터IMC에서 설정한) 눌럿을때, Data 목록 중 Move함수를 실행해라.
	AuraInputComponent->BindAction(InputActions->InputMove, ETriggerEvent::Triggered, this, &AAuraPlayerController::Move);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &AAuraPlayerController::ShiftPressed);
	AuraInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &AAuraPlayerController::ShiftReleased);
	AuraInputComponent->BindAbilityActions(InputConfig,this,&ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased,
		&ThisClass::AbilityInputTagHeld);
}

void AAuraPlayerController::Move(const FInputActionValue& InputActionValue)
{
	// 입력 축 벡터를 가져옵니다. 이 벡터는 플레이어의 움직임 방향을 나타냅니다.
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();

	// 플레이어 컨트롤러의 회전 값을 가져옵니다.
	const FRotator Rotation = GetControlRotation();

	// Yaw (수평) 회전만을 고려하는 회전을 생성합니다.
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);

	// Yaw 회전을 기준으로 전방과 오른쪽 방향 벡터를 계산합니다.
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

	// 조종 중인 Pawn(캐릭터)이 있는지 확인합니다.
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		// Pawn에게 계산된 방향과 입력 축 벡터에 따른 움직임 입력을 추가합니다.
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void AAuraPlayerController::RightClickMove(const FInputActionValue& InputActionValue)
{
	FHitResult Hit;
	this->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility),true,Hit);
	APawn* ControlledPawn = GetPawn<APawn>();
	if(nullptr==ControlledPawn) return ;
	
	if(Hit.bBlockingHit)
	{
		//To - From == From에서 To로 가는 방향벡터!
		FVector LookAtTarget = Hit.ImpactPoint;
		FVector ToTarget = LookAtTarget - ControlledPawn->GetActorLocation();
		ControlledPawn->AddMovementInput(ToTarget);
		
	}

}


