// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "DungeonPath.generated.h"

class UPointLightComponent;
class ATrap;
UCLASS()
class TALES_API ADungeonPath : public AActor
{
	GENERATED_BODY()
	
public:	
	ADungeonPath();
	void InitWallMesh();
	virtual void Tick(float DeltaTime) override;
	void SpawnTraps();
	void ChangeWallMeshdisable(int32 MeshNumber);
	void SetStartPath();
	void SetStartZPath();
	void SetZPath();
	void SetYPath();
protected:

	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<UStaticMeshComponent*> WallMesh;
	UPROPERTY(VisibleAnywhere)
	UPointLightComponent* PointLightComponent;

	UPROPERTY(EditAnywhere, Category = "Trap")
	ATrap* TrapActorA;
	UPROPERTY(EditAnywhere, Category = "Trap")
	ATrap* TrapActorB;

	UPROPERTY(EditAnywhere, Category = "Trap")
	TSubclassOf<ATrap> TrapActorAClass;

	UPROPERTY(EditAnywhere, Category = "Trap")
	TSubclassOf<ATrap> TrapActorBClass;
private:

};
