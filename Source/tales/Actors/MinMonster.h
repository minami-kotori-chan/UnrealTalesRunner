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
	UFUNCTION()
	void OnCameraOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
	void OnCameraOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void CallCameraShake(float DeltaTime);
	void SetPath(float TotalLength) { TotalDungeonLength = TotalLength; };
	void SetSpeed(float Fspeed);
	UFUNCTION()
	void OnMinMonsterOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	void SetMaxEnableY(float Y) { MaxEnableY = Y; };
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	void PlayCameraShake();
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UBoxComponent* CollisionBoxComponent;
	
	UPROPERTY(EditAnywhere)
	float MoveSpeed=700.f;
	float MoveSpeedExpend = 1.f;

	void MoveActor(float DeltaTime);
	UPROPERTY(EditAnywhere, Category = "Camera Shake")
	TSubclassOf<class UCameraShakeBase> ShakeClass;
	bool EnableShake = false;

	float RecallTime = 0.1f;

	float MaxEnableY=1000.f;
	
private:	
	float TotalDungeonLength=0.f;

	float CurrentTime = 0.f;
};
