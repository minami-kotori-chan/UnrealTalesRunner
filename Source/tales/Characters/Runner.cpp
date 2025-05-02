// Fill out your copyright notice in the Description page of Project Settings.


#include "Runner.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "Blueprint/UserWidget.h"
#include "../UserInterfaceRunner.h"
#include "NiagaraComponent.h"
#include "NiagaraSystem.h"
#include "../StunUserWidget.h"
#include "Components/WidgetComponent.h"
#include "../Actors/GhostTail.h"
#include "Kismet/GameplayStatics.h"
#include "../SubSystem/CustomGameSystem.h"

ARunner::ARunner()
{
	JumpMaxCount = 2;
	
	CurrentStamina = MaxStamina;

	DashVfxComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("DashVfxComponent"));
	SetVfxComponent(DashVfxComponent);
	VfxInvisible();
	
	StunWidgetInit();
}
void ARunner::ReachGoal()
{
	// 1) �Է� ��ü ��Ȱ��ȭ
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		PC->SetIgnoreMoveInput(true);          // �Է� ����
		//PC->SetIgnoreLookInput(true);          // �ü� �Է� ����
	}
}
void ARunner::StunWidgetVisible(bool VisibleType)
{
	StunWidgetComponent->SetHiddenInGame(VisibleType);
}
void ARunner::StunWidgetInit()
{
	StunWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("StunWidgetComponent"));
	StunWidgetComponent->SetupAttachment(RootComponent);
	StunWidgetComponent->SetRelativeLocation(FVector(0.f,0.f,30.f));
	StunWidgetComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	StunWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
}
void ARunner::VfxVisible()
{
	//DashVfxComponent->bHiddenInGame = false;
	//DashVfxComponent->SetVisibility(false, true);
	DashVfxComponent->SetHiddenInGame(false);
}
void ARunner::VfxInvisible()
{
	//DashVfxComponent->bHiddenInGame=true;
	//DashVfxComponent->SetVisibility(true, true);
	DashVfxComponent->SetHiddenInGame(true);
}
void ARunner::SetVfxComponent(UNiagaraComponent* VfxComponent)
{
	// ���̾ư��� ������Ʈ ����
	VfxComponent->SetupAttachment(RootComponent);
	
	
	VfxComponent->SetNiagaraVariableFloat(FString("Sprites_Size"), 7.f);
	VfxComponent->SetNiagaraVariableFloat(FString("Sprites_Lifetime"), 0.03f);
	VfxComponent->SetNiagaraVariableFloat(FString("Trail_Lifetime"), 0.01f);
	VfxComponent->SetNiagaraVariableFloat(FString("Trail_Size"), 7.f);
	//������ ����ȭ �õ�...
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
		// �� ����(�� ���, ���ΰ� ��ģ ĳ����) ������ ������ ����ϰ�, ���� ���� �߰�
		FVector LaunchDirection = GetActorLocation() - OtherLocationVector;

		LaunchDirection.Z = (FMath::Abs(LaunchDirection.X) + FMath::Abs(LaunchDirection.Y)) * ZSize;

		LaunchDirection.Normalize();

		KnockBackStart();//�˹���³־���
		float LaunchSize = FMath::Clamp(GetCharacterMovement()->Velocity.Size(), LaunchStrength, LaunchStrength*10);
		
		
		LaunchCharacter(LaunchDirection * LaunchSize, true, true);// ĳ���Ϳ� ƨ���� ����
		// 1. �� ���� ���� ���� ���
		FVector Direction = OtherLocationVector - GetActorLocation();
		Direction.Normalize();

		// 2. ���� ���͸� ȸ����(FRotator)���� ��ȯ
		FRotator TargetRotation = Direction.Rotation();

		// 3. ����� ȸ������ OtherCharacter�� ����
		SetActorRotation(TargetRotation);

	}
}

void ARunner::Jump()
{
	if (!CanMove())
	{
		return;
	}
	if (CharacterStartFalling == ECharacterStartFalling::ECS_StartFalling)
	{
		//CharacterStartFalling = ECharacterStartFalling::ECS_Normal;
		PlayRollingAnimation(1.f);
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
	if (CharacterStartFalling==ECharacterStartFalling::ECS_StartFalling)
	{
		if (!GetCharacterMovement()->IsFalling())
		{
			CharacterStartFalling = ECharacterStartFalling::ECS_Normal;
		}
	}
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
			StopDash(); // ���¹̳� ���� �� ��� ����
			ChangeRunnerState(ECharacterState::ECS_BurnOut);
			GetWorld()->GetTimerManager().SetTimer(TimerHandleBurnOut, this, &ThisClass::EndStun, BurnOutTime, false);
		}
	}
	else
	{
		// ��� ���� �ƴ� �� ���¹̳� ȸ�� (��: �ʴ� 10�� ȸ��)
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
	
	
	if (GEngine)
	{
		//FString DistanceLog = FString::Printf(TEXT("Boolean: %d"), DashVfxComponent->bHiddenInGame);
		//FString DistanceLog = FString::Printf(TEXT("Boolean: %d"), bIsRollingAnimationPlaying);
		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green, DistanceLog);
	}
	CreateGhost(DeltaTime);

	Super::Tick(DeltaTime);
}

void ARunner::CreateGhost(float DeltaTime)
{
	if (!IsAnger() || !(GetCharacterMovement()->Velocity.Size() > 0.f)) return;
	Timecount += DeltaTime;
	if (Timecount >= TimeLimit)
	{
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		(Cast<AGhostTail>(GetWorld()->SpawnActor<AActor>(ActorToSpawn, GetActorLocation() - FVector(0.f, 0.f, 90.f), GetActorRotation() - FRotator(0.f, 90.f, 0.f), SpawnParams)))->Init(GetMesh());//�����ϴ� �����Ͱ� ��� �ǳ�? ���÷��ǽý��ۿ��� ������Ű�°� �ƴϾ���

		Timecount = 0.f;

	}
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
	StunWidgetVisible(false);
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
			//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("AngryCalled"));
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
			//�������� �ִϸ��̼� ���� �ʿ���
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

	// ���� Ÿ�̸Ӱ� �̹� ���� ���̶�� ����
	if (CurrentRemaining <= 0.f)
	{
		return;
	}

	// ���ҷ���ŭ ���� �ð��� ���Դϴ�.
	float NewRemainingTime = FMath::Max(CurrentRemaining - DeltaTime, 0.0f);

	// Ÿ�̸Ӹ� �缳���մϴ�.
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle);

	if (NewRemainingTime > 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ARunner::EndStun, NewRemainingTime, false);
	}
	else
	{
		// ���� ���� �ð��� 0�̸� �ٷ� ���� ����
		EndStun();
	}
}

void ARunner::EndStun()
{
	ChangeRunnerState(ECharacterState::ECS_Idle);
	StunWidgetVisible(true);
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
	// ���� �ӹ� ���� �Ǵ�: ���� ��� ĳ���� �Ʒ��� raycast�� ��� ���� �Ÿ� �̳��̸� ���� �������� ����
	FVector Start = GetActorLocation();
	FVector End = Start - FVector(0, 0, SearchSize);  // �Ʒ��� ���
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
		// �ʿ��ϴٸ� �α� ���: "���� ��� ��û��"
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
	if (CharacterAngry == ECharacterAngry::ECS_Charge)
	{
		ChangeRunnerAngry(ECharacterAngry::ECS_Angry);

		GetCharacterMovement()->MaxWalkSpeed = DashSpeed * 1.5;
		// 10�� �Ŀ� ResetAnger �Լ��� ȣ���ϵ��� Ÿ�̸� ����
		GetWorldTimerManager().SetTimer(AngryTimerHandle, this, &ARunner::StopAngry, 10.0f, false);
		HealthWidget->ChangeCharge();
	}
	
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
	GetCharacterMovement()->MaxAcceleration = 100000.f;//���ӵ��� �ִ뿡 ������ �༭ �ӵ� ���� �ð��� ����

	CharacterStartFalling = ECharacterStartFalling::ECS_StartFalling;//������ ���۵Ǹ鼭 �÷��̾� ���º����� �ִµ��� ���� ���۽ÿ� State���� �ؼ� �ذ�

	AddCallbackEndAnimation();
	
	RunningSpeed = GetCharacterMovement()->MaxWalkSpeed;
	UE_LOG(LogTemp, Log, TEXT("Max Walking Speed: %f"), RunningSpeed);
	DashSpeed = RunningSpeed * 2;
	HealthWidgetInit();
	StunWidgetCreate();
}

void ARunner::AddCallbackEndAnimation()
{
	// �ִϸ��̼� �ν��Ͻ� ��������
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance)
	{
		// ��Ÿ�� �� �̺�Ʈ�� �ݹ� �Լ� ���
		//AnimInstance->OnMontageEnded.AddDynamic(this, &ARunner::OnRollingEnd);
	}
}

void ARunner::HealthWidgetInit()
{
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
void ARunner::StunWidgetCreate()
{
	// ���� Ŭ������ �����Ǿ� �ִٸ�
	if (StunWidgetClass)
	{
		// ������Ʈ�� ���� Ŭ���� ����
		StunWidgetComponent->SetWidgetClass(StunWidgetClass);

		// ���� �ν��Ͻ� ����
		StunWidget = CreateWidget<UStunUserWidget>(GetWorld(), StunWidgetClass);
		if (StunWidget)
		{
			// ���� ������Ʈ�� ���� �ν��Ͻ� ����
			StunWidgetComponent->SetWidget(StunWidget);

			// �߰� ����
			//StunWidget->Disable();
			StunWidgetVisible(true);
		}
	}
}
void ARunner::PlayRollingAnimation(float AnimationPlayRate)
{
	if (RollingAnimation && bIsRollingAnimationPlaying==false)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(RollingAnimation, AnimationPlayRate); 
		bIsRollingAnimationPlaying = true;
	}
}
void ARunner::OnRollingEnd(UAnimMontage* Montage, bool bInterrupted)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("MontageEnded"));
	}
	bIsRollingAnimationPlaying = false;
}

void ARunner::DashFailed()
{
	FTimerDelegate TimerDelegate;
	TimerDelegate.BindUFunction(this, FName("ChangeRunnerState"), ECharacterState::ECS_Idle);
	GetWorld()->GetTimerManager().SetTimer(TimerHandleKnockBack, TimerDelegate, 4.5f, false);
}
