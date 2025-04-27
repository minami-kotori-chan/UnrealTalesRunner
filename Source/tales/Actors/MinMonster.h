// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MinMonster.generated.h"

class ADungeonPath;

UCLASS()
class TALES_API AMinMonster : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMinMonster();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void SetPath(float TotalLength) { TotalDungeonLength = TotalLength; };
	void SetSpeed(float Fspeed);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComponent;
	
	UPROPERTY(EditAnywhere)
	float MoveSpeed=700.f;
	float MoveSpeedExpend = 1.f;
	UFUNCTION()
	void OnMinMonsterOverlap(AActor* OverlappedActor, AActor* OtherActor);

	void MoveActor(float DeltaTime);
private:	
	float TotalDungeonLength=0.f;

};
