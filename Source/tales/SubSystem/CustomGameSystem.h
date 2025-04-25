// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CustomGameSystem.generated.h"

/**
 * 
 */
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
};
