// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/RangerCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

// Sets default values
ARangerCharacter::ARangerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// 캐릭터가 회전하는 비율을 설정합니다 (Yaw 축).
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f);

	// 캐릭터의 움직임을 평면에 제한하여 Z축으로의 움직임을 막습니다.
	GetCharacterMovement()->bConstrainToPlane = true;

	// 시작 시 캐릭터를 이동 평면에 정렬합니다.
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	// 컨트롤러 기반 회전을 비활성화합니다 (Pitch, Roll, Yaw 축).
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	bUseControllerRotationYaw = false;

	

}

void ARangerCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
}

void ARangerCharacter::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();
}

// Called when the game starts or when spawned
void ARangerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ARangerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ARangerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ARangerCharacter::SetCamera()
{
}



