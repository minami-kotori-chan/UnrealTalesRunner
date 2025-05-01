// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CustomGameSystem.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FProgressUpdated, float, DeadlineProgress, float, CharacterProgress);

UCLASS()
class TALES_API UCustomGameSystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()
	
public:
	void InitDungeonPathData(class ADungeonPath* DungeonPath);
	float StartX, EndX;
	float StartY, EndY;
	float StartZ, EndZ;
	FVector DungeonStartVector;
	FVector DungeonEndVector;

	int32 DungeonLength;
	void SetDungeonLength(float Length) { DungeonLength = (int32)Length; };
	UPROPERTY(BlueprintAssignable, Category = "Progress")
	FProgressUpdated OnProgressChanged;
};
