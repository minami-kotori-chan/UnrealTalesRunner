// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Electric.generated.h"

class UNiagaraComponent;

UCLASS()
class TALES_API AElectric : public AActor
{
	GENERATED_BODY()
	
public:	
	
	virtual void Tick(float DeltaTime) override;
	
	AElectric();

	void CreateCollisionBox();


	//컴포넌트 관련 멤버들
	void SetLightningComponent(UNiagaraComponent* LightningActor);
	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraComponent* LightningEffect;
	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraComponent* LightningEffect2;
	//움직임 관련 멤버들
	void SetElectricMovingSpeed(const FVector& InputSpeed);
	void SetElectricFlatten();
	void SetRedElectric() { EColor = true; };
protected:
	
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Collision")
	class UBoxComponent* CollisionBox;
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,bool bFromSweep, const FHitResult& SweepResult);
	
private:
	//초기값 설정 관련 멤버들
	void SetDataFromSubSystem();
	void InitBoundingBox(const FVector& MinVector, const FVector& MaxVector);
	FVector MovingMaxVector;
	FVector MovingMinVector;
	
	FVector MovingSpeed;

	FVector MovementDirection; // 이동 방향 벡터
	bool bMoveHorizontally = true; // true: 좌우 이동, false: 상하 이동
	UPROPERTY(EditAnywhere, Category = "Moving")
	float MovementSpeed = 1000.0f; // 이동 속도

	//이동 관련 함수들
	void InitializeMovementDirection();
	void MoveActor(float DeltaTime);
	void CheckBoundariesAndUpdateDirection(FVector& NewLocation);
	void EnsureNegativeYDirection();
	void ToggleMovementDirection();
	bool CanMove=true;//좌우상하 이동가능 여부

	bool EColor = false;
};
