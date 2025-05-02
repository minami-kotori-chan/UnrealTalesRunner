// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "talesGameMode.generated.h"


UCLASS(minimalapi)
class AtalesGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AtalesGameMode();
protected:
	virtual void BeginPlay() override;
	void SetSubSystemData();
	virtual void StartPlay() override;
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;
	void SpawnPath();
	void SpawnPathWithVertical();
	int32 GetSpawnCount();
	void SpawnVerticalPaths(const FVector& StartLocation, int32 Count);
	void SpawnHorizontalPaths(const FVector& StartLocation, int32 Count, const int32& StartIndexNumber);
	void SpawnAllMonster();
	void SpawnMaxMonster();
	void SpawnMinMonster();
	virtual void Tick(float DeltaTime) override;
	void SpawnTrigger();
	void SpawnElectricVFX();
	void SpawnElectricVFXSetting(const FBox& BoundingBox, FVector& OutputVector, bool Type = true);
	void UpdateElectricVFX(float DeltaTime);
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;
	UFUNCTION()
	void CalculateDungeonLength();
	void UpdateProgressBar();
	void UpdateProgressVal(float DeltaTime);


	//������ �����н� ����
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<class ADungeonPath> DungeonPathBP;

	//���� ���� ����
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<class AActor> MinMonster;
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<class AActor> MaxMonster;


	//���� ����
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<class AElectric> ElectricRed;
	UPROPERTY(EditDefaultsOnly, Category = "Spawning")
	TSubclassOf<class AElectric> ElectricBlue;

	// X ��ǥ ��ġ ������ (0-100)
	UPROPERTY(EditAnywhere, Category = "Spawning", meta = (ClampMin = "0", ClampMax = "100"))
	float ElectricXPositionPercent = 50.0f;

	// ���� ���� ����
	UPROPERTY(EditAnywhere, Category = "Spawning")
	float ElectricSpawnInterval = 1.0f;

	// ���� ���� ť
	TQueue<AElectric*> ElectricActorQueue;

	// Ÿ�̸� �ڵ�
	FTimerHandle ElectricSpawnTimerHandle;

	//�����н� �迭
	TArray<ADungeonPath*> DungeonPathArray;
	TArray<ADungeonPath*> DungeonPathZArray;

	//������ ��� ����
	UPROPERTY()
	AActor* SpawnedMinMonster;
	UPROPERTY()
	AActor* SpawnedMaxMonster;


	class ARunner* Runner;

	float TotalDungeonLength, DungeonStartY;

	float MinimumProgress=0.f;
	float MaximumProgress=1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "UnitSpeed")
	float MinSpeed=0.01f;
	UPROPERTY(EditDefaultsOnly, Category = "UnitSpeed")
	float MaxSpeed=0.02f;

	float CharacterProgress;

	float LastOffset = 0.f;
};



