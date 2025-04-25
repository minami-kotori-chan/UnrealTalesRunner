// Fill out your copyright notice in the Description page of Project Settings.


#include "Runner.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
#include "../UserInterfaceRunner.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"

ARunner::ARunner()
{
	JumpMaxCount = 2;
	
	CurrentStamina = MaxStamina;

	DashVfxComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DashVfxComponent"));
	SetVfxComponent(DashVfxComponent);
	VfxInvisible();
	
}
void ARunner::VfxVisible()
{
	DashVfxComponent->bHiddenInGame = false;
}
void ARunner::VfxInvisible()
{
	DashVfxComponent->bHiddenInGame=true;
}
void ARunner::SetVfxComponent(UNiagaraComponent* VfxComponent)
{
	// 나이아가라 컴포넌트 설정
	VfxComponent->SetupAttachment(RootComponent);
	
	
	VfxComponent->SetNiagaraVariableFloat(FString("Sprites_Size"), 7.f);
	VfxComponent->SetNiagaraVariableFloat(FString("Sprites_Lifetime"), 0.03f);
	VfxComponent->SetNiagaraVariableFloat(FString("Trail_Lifetime"), 0.01f);
	VfxComponent->SetNiagaraVariableFloat(FString("Trail_Size"), 7.f);
	//라이팅 최적화 시도...
	//VfxComponent->bSingleSampleShadowFromStationaryLights = true;
	//VfxComponent->bAffectDynamicIndirectLighting = false;

	//VfxComponent->SetupAttachment(RootComponent);
	
}

void ARunner::ChangeRunnerState(ECharacterState ChangeState)
{
	
	CharacterState = ChangeState;
}

void ARunner::StartBounce(const FVector& OtherLocationVector,float LaunchStrength, float ZSize)
{
	if (!IsAnger() && CanMove())
	{
		// 두 액터(이 경우, 본인과 겹친 캐릭터) 사이의 방향을 계산하고, 위쪽 성분 추가
		FVector LaunchDirection = GetActorLocation() - OtherLocationVector;

		LaunchDirection.Z = (FMath::Abs(LaunchDirection.X) + FMath::Abs(LaunchDirection.Y)) * ZSize;

		LaunchDirection.Normalize();

		KnockBackStart();//넉백상태넣어줌
		float LaunchSize = FMath::Clamp(GetCharacterMovement()->Velocity.Size(), LaunchStrength, LaunchStrength*10);
		
		
		LaunchCharacter(LaunchDirection * LaunchSize, true, true);// 캐릭터에 튕김을 적용
		// 1. 두 액터 간의 방향 계산
		FVector Direction = OtherLocationVector - GetActorLocation();
		Direction.Normalize();

		// 2. 방향 벡터를 회전값(FRotator)으로 변환
		FRotator TargetRotation = Direction.Rotation();

		// 3. 계산한 회전값을 OtherCharacter에 적용
		SetActorRotation(TargetRotation);

	}
}

void ARunner::Jump()
{
	if (!CanMove())
	{
		return;
	}
	if (JumpCurrentCount == 1)
	{
		PlayRollingAnimation();
		CharacterState = ECharacterState::ECS_DoubleJumping;
	}
	Super::Jump();
}

void ARunner::Move(const FInputActionValue& Value)
{
	if (!CanMove())
	{
		return;
	}
	if (CharacterState == ECharacterState::ECS_Idle)
	{
		CharacterState = ECharacterState::ECS_Running;
	}
	Super::Move(Value);
}

void ARunner::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);
	if (CharacterState == ECharacterState::ECS_Stun)
	{

	}
	else if (CharacterState == ECharacterState::ECS_BurnOut)
	{

	}
	else if (CharacterState == ECharacterState::ECS_KnockBack)
	{
		if (!GetCharacterMovement()->IsFalling())
		{
			KnockBackLanding();
		}
	}
	else if (GetCharacterMovement()->IsFalling())
	{
		CharacterState = ECharacterState::ECS_Jumping;
	}
	else 
	{
		CharacterState = ECharacterState::ECS_Idle;
	}
}

void ARunner::Tick(float DeltaTime=3.f)
{
	if (CharacterState!= ECharacterState::ECS_Idle && CanMove() && GetVelocity().Size() == 0.f)
	{
		CharacterState = ECharacterState::ECS_Idle;
	}


	if (DashBtnPressed && CanMove() && CharacterState!= ECharacterState::ECS_Idle)
	{
		CurrentStamina -= DashStaminaDrainRate * DeltaTime;
		HealthWidget->UpdateStamina(CurrentStamina,MaxStamina);

		if (CurrentStamina <= 0.f)
		{
			CurrentStamina = 0.f;
			StopDash(); // 스태미나 소진 시 대시 중지
			ChangeRunnerState(ECharacterState::ECS_BurnOut);
			GetWorld()->GetTimerManager().SetTimer(TimerHandleBurnOut, this, &ThisClass::EndStun, BurnOutTime, false);
		}
	}
	else
	{
		// 대시 중이 아닐 때 스태미나 회복 (예: 초당 10씩 회복)
        if (CanMove() && CurrentStamina < MaxStamina)
        {
            CurrentStamina += 10.f * DeltaTime;
			HealthWidget->UpdateStamina(CurrentStamina, MaxStamina);
            if (CurrentStamina > MaxStamina)
            {
                CurrentStamina = MaxStamina;
            }
        }
	}
	if (CurrentStamina==MaxStamina && CurrentAnger == MaxAnger)
	{
		if (CharacterAngry != ECharacterAngry::ECS_Charge && CharacterAngry != ECharacterAngry::ECS_Angry)
		{
			HealthWidget->ChangeAngry();
			ChangeRunnerAngry(ECharacterAngry::ECS_Charge);
		}
	}
	Super::Tick(DeltaTime);
}

void ARunner::ChangeRunnerAngry(ECharacterAngry ChangeState)
{
	CharacterAngry = ChangeState;
}

void ARunner::StunStart(float DeltaTime)
{
	if (CharacterState == ECharacterState::ECS_Stun || CharacterAngry==ECharacterAngry::ECS_Angry || CharacterState == ECharacterState::ECS_KnockBack)
	{
		return;
	}
	if (CharacterDash == ECharacterDash::ECS_Dash)
	{
		StopDash();
	}
	ChangeRunnerState(ECharacterState::ECS_Stun);
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::EndStun, DeltaTime, false);
	AngerUp();
}

void ARunner::KnockBackStart(float DeltaTime)
{
	if (CharacterDash == ECharacterDash::ECS_Dash)
	{
		StopDash();
	}
	ChangeRunnerState(ECharacterState::ECS_KnockBack);
	AngerUp();
}

bool ARunner::IsAnger()
{
	if (CharacterAngry == ECharacterAngry::ECS_Angry)
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("AngryCalled"));
		}
	}
	return CharacterAngry == ECharacterAngry::ECS_Angry;
}

void ARunner::AngerUp()
{
	CurrentAnger += AngerUpRate;
	if (CurrentAnger >= MaxAnger)
	{
		CurrentAnger = MaxAnger;
	}

	HealthWidget->UpdateAnger(CurrentAnger,MaxAnger);
}

void ARunner::KnockBackLanding()
{
	if (PressDashBtn)
	{
		if (bLandingDashRequested)
		{
			FVector DashDirection = GetActorForwardVector();
			LaunchCharacter(DashDirection * LandingDashForce, true, true);
			//GetCharacterMovement()->MaxWalkSpeed = DashSpeed;
			//GetCharacterMovement()->Velocity = DashDirection*DashSpeed;
			ChangeRunnerState(ECharacterState::ECS_Idle);

			bLandingDashRequested = false;
			PressDashBtn = false;
		}
		else
		{
			DashFailed();
			//쓰러지는 애니메이션 실행 필요함
			PressDashBtn = false;
		}
	}
	else
	{
		DashFailed();
	}
	
}

void ARunner::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		EnhancedInputComponent->BindAction(ReduceStunAction, ETriggerEvent::Started, this, &ThisClass::CallReduceStun, 0.1f);
		EnhancedInputComponent->BindAction(LandingDashAction, ETriggerEvent::Started, this, &ThisClass::HandleLandingDashInput);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Started, this, &ThisClass::StartDash);
		EnhancedInputComponent->BindAction(DashAction, ETriggerEvent::Completed, this, &ThisClass::StopDash);
		EnhancedInputComponent->BindAction(AngryAction, ETriggerEvent::Started, this, &ThisClass::StartAngry);
		
	}

	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ARunner::ReduceStun(float DeltaTime)
{
	if (CharacterState != ECharacterState::ECS_Stun)
	{
		return;
	}
	float CurrentRemaining = GetWorld()->GetTimerManager().GetTimerRemaining(TimerHandle);
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("ReducedCalled"));
	}

	// 만약 타이머가 이미 종료 중이라면 무시
	if (CurrentRemaining <= 0.f)
	{
		return;
	}

	// 감소량만큼 남은 시간을 줄입니다.
	float NewRemainingTime = FMath::Max(CurrentRemaining - DeltaTime, 0.0f);

	// 타이머를 재설정합니다.
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

	if (NewRemainingTime > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ARunner::EndStun, NewRemainingTime, false);
	}
	else
	{
		// 만약 남은 시간이 0이면 바로 스턴 해제
		EndStun();
	}
}

void ARunner::EndStun()
{
	ChangeRunnerState(ECharacterState::ECS_Idle);
}

void ARunner::CallReduceStun(float DeltaTime)
{
	if (CharacterState == ECharacterState::ECS_Stun)
	{
		ReduceStun(DeltaTime);
	}
}

bool ARunner::CanMove()
{
	return !(CharacterState == ECharacterState::ECS_Stun || CharacterState == ECharacterState::ECS_KnockBack || CharacterState == ECharacterState::ECS_BurnOut);
}

void ARunner::HandleLandingDashInput(const FInputActionValue& Value)
{
	
	if (CharacterState == ECharacterState::ECS_KnockBack && GetCharacterMovement()->IsFalling() && GetCharacterMovement()->Velocity.Z < 0 && !PressDashBtn)
	{
		LandingDashInput();
	}
	else if(CharacterState == ECharacterState::ECS_Running || CharacterDash == ECharacterDash::ECS_Dash)
	{

	}
}

void ARunner::LandingDashInput()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("ButtonCalled"));
	}
	// 착지 임박 여부 판단: 예를 들어 캐릭터 아래로 raycast를 쏘아 일정 거리 이내이면 착지 직전으로 간주
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0, 0, SearchSize);  // 아래로 쏘기
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params);

	if (bHit)
	{
		bLandingDashRequested = true;
		float Distance = FVector::Dist(Start, HitResult.ImpactPoint);
		//FString DistanceLog = FString::Printf(TEXT("Detected object distance: %f"), Distance);
		FString DistanceLog = FString::Printf(TEXT("start: %f end : %f distance %f"), Start.Z, (HitResult.ImpactPoint).Z, Distance);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, DistanceLog);
		}
		// 필요하다면 로그 출력: "착지 대시 요청됨"
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("DashCalled"));
		}
	}
	else
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("TimmingMiss"));
		}
	}
	PressDashBtn = true;
}

void ARunner::StartDash()
{
	if (CharacterAngry == ECharacterAngry::ECS_Charge || CharacterAngry == ECharacterAngry::ECS_Angry)
	{
		return;
	}
	DashBtnPressed =true;
	if (CanMove() && CharacterState != ECharacterState::ECS_Idle)
	{
		VfxVisible();
		GetCharacterMovement()->MaxWalkSpeed = DashSpeed;
		//CharacterState = ECharacterState::ECS_Dash;
		CharacterDash = ECharacterDash::ECS_Dash;
	}
	
}

void ARunner::StopDash()
{
	if (CharacterAngry == ECharacterAngry::ECS_Charge || CharacterAngry == ECharacterAngry::ECS_Angry)
	{
		return;
	}
	DashBtnPressed = false;
	if (CanMove())
	{
		GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
		/*
		if (CharacterState == ECharacterState::ECS_Dash)
		{
			CharacterState = ECharacterState::ECS_Running;
		}
		*/
		CharacterDash = ECharacterDash::ECS_Normal;
		VfxInvisible();
	}
}

void ARunner::StartAngry()
{
	ChangeRunnerAngry(ECharacterAngry::ECS_Angry);

	GetCharacterMovement()->MaxWalkSpeed = DashSpeed*1.5;
	// 10초 후에 ResetAnger 함수를 호출하도록 타이머 설정
	GetWorldTimerManager().SetTimer(AngryTimerHandle, this, &ARunner::StopAngry, 10.0f, false);
}

void ARunner::StopAngry()
{
	ChangeRunnerAngry(ECharacterAngry::ECS_Normal);
	GetCharacterMovement()->MaxWalkSpeed = RunningSpeed;
	HealthWidget->ChangeOrigin();
	CurrentAnger = 0.f;
	HealthWidget->UpdateAnger(CurrentAnger,MaxAnger);
}

void ARunner::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxAcceleration = 100000.f;

	RunningSpeed = GetCharacterMovement()->MaxWalkSpeed;
	UE_LOG(LogTemp, Log, TEXT("Max Walking Speed: %f"), RunningSpeed);
	DashSpeed = RunningSpeed * 2;
	if (HealthWidgetClass)
	{
		
		HealthWidget = Cast<UUserInterfaceRunner>(CreateWidget(GetWorld(), HealthWidgetClass));
		if (HealthWidget)
		{
			HealthWidget->AddToViewport();
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("UICalled"));
			}
		}
	}
}

void ARunner::PlayRollingAnimation()
{
	if (RollingAnimation)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(RollingAnimation);
	}
}

void ARunner::DashFailed()
{
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("ChangeRunnerState"), ECharacterState::ECS_Idle);
	GetWorld()->GetTimerManager().SetTimer(TimerHandleKnockBack, TimerDelegate, 4.5f, false);
}
