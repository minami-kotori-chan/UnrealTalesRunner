// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SelectGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TALES_API ASelectGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:

protected:
    virtual void BeginPlay() override;

    // ���� Ŭ���� ���۷���
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class USelectGameWidget> MenuWidgetClass;

    // ���� �ν��Ͻ�
    UPROPERTY()
    class USelectGameWidget* MenuWidget;

private:

};
