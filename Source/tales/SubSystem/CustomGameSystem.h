// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CustomGameSystem.generated.h"

/**
 * 
 */
class ARunner;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FProgressUpdated, float, DeadlineProgress, float, CharacterProgress);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterGoal, ARunner*,Runner);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterOut, ARunner*, Runner);
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

	float ClearTime = 0.0f;

	UPROPERTY(BlueprintAssignable, Category = "Progress")
	FProgressUpdated OnProgressChanged;//진행도 변화 델리게이트
	FOnCharacterGoal OnCharacterGoal;//캐릭터 완주 델리게이트
	FOnCharacterOut OnCharacterOut;//캐릭터 탈락 델리게이트

};
