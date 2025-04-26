// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GhostTail.generated.h"


UCLASS()
class TALES_API AGhostTail : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AGhostTail();
	virtual void Tick(float DeltaTime) override;
	void Init(USkeletalMeshComponent* Pawn);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	


	void SetSkeletalMesh(class USkeletalMesh* SkMesh) { SK_PoseMesh = SkMesh; };
	void SetMaterialMesh(class UMaterialInstance* MaterialMesh) { M_GhostTail = MaterialMesh; };

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class USkeletalMesh* SK_PoseMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UMaterialInstance* M_GhostTail;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	class UPoseableMeshComponent* PoseableMesh;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UMaterialInstance* GhostMaterial;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Life")
	float LifeTime=1.5f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Life")
	float CurrentTime = 0.f;

private:
	
	TArray<UMaterialInstanceDynamic*> Materials;

	bool IsSpawned = false;
	float FadeCountDown;
	float FadeOutTime = 0.5f;
	

};
