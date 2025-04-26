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
    // UMG ���� �������Ʈ���� ���� StaminaBar�� ����
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* StaminaBar;

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* AngerBar;

    // ü�� ������ ������Ʈ�� �Լ� (�ܺο��� ȣ���ϰų� Tick���� �ڵ� ������Ʈ)
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
    virtual void NativeConstruct() override;
    
};
