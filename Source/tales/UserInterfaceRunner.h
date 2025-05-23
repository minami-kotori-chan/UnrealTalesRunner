// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UserInterfaceRunner.generated.h"

/**
 * 
 */
UCLASS()
class TALES_API UUserInterfaceRunner : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // UMG 위젯 블루프린트에서 만든 StaminaBar와 연결
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* StaminaBar;

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* AngerBar;

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* GameProgressBar;

    UPROPERTY(meta = (BindWidget))
    class UImage* MarkerImage;

    UPROPERTY(meta = (BindWidget))
    class UCanvasPanel* Canvas;

    UPROPERTY(meta = (BindWidget))
    class UTextBlock* CTime;

    // 진행도 업데이트 함수
    UFUNCTION(BlueprintCallable, Category = "UI")
    void UpdateDungeonProgress(float DeadlineProgress, float CharacterProgress);

    // 체력 정보를 업데이트할 함수 (외부에서 호출하거나 Tick에서 자동 업데이트)
    UFUNCTION(BlueprintCallable, Category = "Stamina")
    void UpdateStamina(float CurrentStamina, float MaxStamina);
    UFUNCTION(BlueprintCallable, Category = "Stamina")
    void UpdateAnger(float CurrentAnger, float MaxAnger);
    void ChangeAngry();
    void ChangeCharge();
    void ChangeOrigin();
    FLinearColor OriginalStaminaColor;
    FLinearColor OriginalAngerColor;

protected:
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

    void FormatTime(float TimeInSeconds, FString& OutFormattedTime);
    
    virtual void NativeConstruct() override;
    UFUNCTION(BlueprintCallable)
    void StopTimer(class ARunner* Runner);
    void StopTimeOut(ARunner* Runner);
    void SetSubSystemData(float Time);
private:
    float CurrentTime = 0.f;
    bool bIsTimerRunning = true;
};
