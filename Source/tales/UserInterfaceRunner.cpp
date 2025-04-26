// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterfaceRunner.h"
#include "Components/ProgressBar.h"

void UUserInterfaceRunner::UpdateStamina(float CurrentStamina, float MaxStamina)
{
    if (StaminaBar && MaxStamina > 0.f)
    {
        float Percent = CurrentStamina / MaxStamina;
        StaminaBar->SetPercent(Percent);
    }
}

void UUserInterfaceRunner::UpdateAnger(float CurrentAnger, float MaxAnger)
{
    if (AngerBar && MaxAnger > 0.f)
    {
        float Percent = CurrentAnger / MaxAnger;
        AngerBar->SetPercent(Percent);
    }
}

void UUserInterfaceRunner::ChangeAngry()
{
    if (StaminaBar && AngerBar)
    {
        // 보라색으로 변경합니다.
        FLinearColor PurpleColor(0.5f, 0.0f, 1.0f, 1.0f);
        StaminaBar->SetFillColorAndOpacity(PurpleColor);
        AngerBar->SetFillColorAndOpacity(PurpleColor);
    }
    
}

void UUserInterfaceRunner::ChangeCharge()
{
    if (StaminaBar && AngerBar)
    {
        // 노란색으로 변경합니다.
        FLinearColor PurpleColor(1.0f, 1.0f, 0.0f, 1.0f);
        StaminaBar->SetFillColorAndOpacity(PurpleColor);
        AngerBar->SetFillColorAndOpacity(PurpleColor);
    }
}

void UUserInterfaceRunner::ChangeOrigin()
{
    if (StaminaBar && AngerBar)
    {
        StaminaBar->SetFillColorAndOpacity(OriginalStaminaColor);
        AngerBar->SetFillColorAndOpacity(OriginalAngerColor);
    }
}

void UUserInterfaceRunner::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
}

void UUserInterfaceRunner::NativeConstruct()
{
    Super::NativeConstruct();

    OriginalStaminaColor= StaminaBar->GetFillColorAndOpacity();
    OriginalAngerColor= AngerBar->GetFillColorAndOpacity();
}
