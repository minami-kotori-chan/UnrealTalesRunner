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

    // 위젯 클래스 레퍼런스
    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class USelectGameWidget> MenuWidgetClass;

    // 위젯 인스턴스
    UPROPERTY()
    class USelectGameWidget* MenuWidget;

private:

};
