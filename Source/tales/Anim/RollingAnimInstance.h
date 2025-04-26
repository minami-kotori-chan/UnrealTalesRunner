// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "RollingAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class TALES_API URollingAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:

protected:
	UFUNCTION(BlueprintCallable)
	void AnimNotify_EndRolling();

private:
};
