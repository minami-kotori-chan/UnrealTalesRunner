// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../talesCharacter.h"
#include "CharacterState.h"
#include "Runner.generated.h"

/**
 * 
 */
UCLASS()
class TALES_API ARunner : public AtalesCharacter
{
	GENERATED_BODY()
public:
	ARunner();
	void StunWidgetVisible(bool VisibleType);
	void StunWidgetInit();
	void VfxVisible();
	void VfxInvisible();
	void ReachGoal();
	void CharacterOut();
	void RunnerInputDisable();
	UFUNCTION()
	void ChangeRunnerState(ECharacterState ChangeState);

	void ChangeRunnerAngry(ECharacterAngry ChangeState);
	void StunStart(float DeltaTime = 3.f);
	void KnockBackStart(float DeltaTime = 3.f);
	bool IsAnger();
	
	//ĳ���� ƨ�� �Լ�
	void StartBounce(const FVector& OtherLocationVector, float LaunchStrength, float ZSize);

	bool CanMove();

	void ChangeRollingAnimationCanPlay() { bIsRollingAnimationPlaying = false; };
protected:
	virtual void Jump() override;
	virtual void Move(const FInputActionValue& Value);
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	virtual void Tick(float DeltaTime);

	void CreateGhost(float DeltaTime);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dash")
	float MaxStamina = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Dash")
	float CurrentStamina = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Dash")
	float DashStaminaDrainRate = 20.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dash")
	float BurnOutTime = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anger")
	float CurrentAnger = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anger")
	float MaxAnger = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Anger")
	float AngerUpRate = 10.f;

	void AngerUp();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ECharacterAngry CharacterAngry = ECharacterAngry::ECS_Normal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ECharacterState CharacterState = ECharacterState::ECS_Idle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ECharacterStartFalling CharacterStartFalling = ECharacterStartFalling::ECS_StartFalling;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LandingDash")
	bool bLandingDashRequested = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LandingDash")
	bool PressDashBtn = false;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LandingDash")
	float LandingDashForce = 1000.0f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "LandingDash")
	float SearchSize = 200.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ECharacterDash CharacterDash = ECharacterDash::ECS_Normal;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> HealthWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	TSubclassOf<class UUserWidget> StunWidgetClass;

	void KnockBackLanding();

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	void ReduceStun(float DeltaTime);
	void EndStun();
	void CallReduceStun(float DeltaTime=0.1f);

	

	void HandleLandingDashInput(const FInputActionValue& Value);
	void LandingDashInput();

	void StartDash();
	void StopDash();

	void StartAngry();
	void StopAngry();
	virtual void BeginPlay() override;

	void AddCallbackEndAnimation();
	UFUNCTION()
	void OnRollingEnd(UAnimMontage* Montage, bool bInterrupted);
	
	void StunWidgetCreate();

	void HealthWidgetInit();

	float RunningSpeed;
	float DashSpeed;


	
	UPROPERTY(EditAnywhere, Category = "Effects")
	class UNiagaraComponent* DashVfxComponent;

	void SetVfxComponent(UNiagaraComponent* VfxComponent);

	UPROPERTY(VisibleAnywhere, Category = UI)
	class UWidgetComponent* StunWidgetComponent;

private:
	UPROPERTY(EditAnywhere,Category=Animation)
	TObjectPtr<class UAnimMontage> RollingAnimation;

	UPROPERTY(EditAnywhere, Category = Animation)
	TObjectPtr<class UAnimMontage> GoalAnimation;

	void PlayRollingAnimation(float AnimationPlayRate=1.0f);
	void PlayGoalAnimation();

	FTimerHandle TimerHandle;
	FTimerHandle TimerHandleKnockBack;
	FTimerHandle TimerHandleBurnOut;
	FTimerHandle AngryTimerHandle;
	
	bool BwasInputed=false;
	bool DashBtnPressed = false;
	bool bIsRollingAnimationPlaying = false;
	void DashFailed();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReduceStunAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LandingDashAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* DashAction;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AngryAction;

	class UUserInterfaceRunner* HealthWidget;
	class UStunUserWidget* StunWidget;

	float Timecount = 0.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GhostTime", meta = (AllowPrivateAccess = "true"))
	float TimeLimit=0.1f;

	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<AActor> ActorToSpawn;

	TArray<class AGhostTail*> GhostArray;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GameOver", meta = (AllowPrivateAccess = "true"))
	bool GameOver = false;
};
