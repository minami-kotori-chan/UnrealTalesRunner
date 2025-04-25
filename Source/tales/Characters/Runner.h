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
	void VfxVisible();
	void VfxInvisible();
	UFUNCTION()
	void ChangeRunnerState(ECharacterState ChangeState);

	void ChangeRunnerAngry(ECharacterAngry ChangeState);
	void StunStart(float DeltaTime = 3.f);
	void KnockBackStart(float DeltaTime = 3.f);
	bool IsAnger();
	
	//Ä³¸¯ÅÍ Æ¨±è ÇÔ¼ö
	void StartBounce(const FVector& OtherLocationVector, float LaunchStrength, float ZSize);

	bool CanMove();
protected:
	virtual void Jump() override;
	virtual void Move(const FInputActionValue& Value);
	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override;
	virtual void Tick(float DeltaTime);

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

	float RunningSpeed;
	float DashSpeed;


	
	UPROPERTY(EditAnywhere, Category = "Effects")
	class UNiagaraComponent* DashVfxComponent;

	void SetVfxComponent(UNiagaraComponent* VfxComponent);

private:
	UPROPERTY(EditAnywhere,Category=Animation)
	TObjectPtr<class UAnimMontage> RollingAnimation;
	
	void PlayRollingAnimation();
	FTimerHandle TimerHandle;
	FTimerHandle TimerHandleKnockBack;
	FTimerHandle TimerHandleBurnOut;
	FTimerHandle AngryTimerHandle;
	
	bool BwasInputed=false;
	bool DashBtnPressed = false;
	
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
};
