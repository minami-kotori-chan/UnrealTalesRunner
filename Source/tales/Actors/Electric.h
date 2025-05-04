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


	//������Ʈ ���� �����
	void SetLightningComponent(UNiagaraComponent* LightningActor);
	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraComponent* LightningEffect;
	UPROPERTY(EditAnywhere, Category = "Effects")
	UNiagaraComponent* LightningEffect2;
	//������ ���� �����
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
	//�ʱⰪ ���� ���� �����
	void SetDataFromSubSystem();
	void InitBoundingBox(const FVector& MinVector, const FVector& MaxVector);
	FVector MovingMaxVector;
	FVector MovingMinVector;
	
	FVector MovingSpeed;

	FVector MovementDirection; // �̵� ���� ����
	bool bMoveHorizontally = true; // true: �¿� �̵�, false: ���� �̵�
	UPROPERTY(EditAnywhere, Category = "Moving")
	float MovementSpeed = 1000.0f; // �̵� �ӵ�

	//�̵� ���� �Լ���
	void InitializeMovementDirection();
	void MoveActor(float DeltaTime);
	void CheckBoundariesAndUpdateDirection(FVector& NewLocation);
	void EnsureNegativeYDirection();
	void ToggleMovementDirection();
	bool CanMove=true;//�¿���� �̵����� ����

	bool EColor = false;
};
