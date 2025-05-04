// Fill out your copyright notice in the Description page of Project Settings.


#include "UserInterfaceRunner.h"
#include "Components/ProgressBar.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Kismet/GameplayStatics.h"
#include "SubSystem/CustomGameSystem.h"
#include "Components/TextBlock.h"

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
        // ��������� �����մϴ�.
        FLinearColor PurpleColor(0.5f, 0.0f, 1.0f, 1.0f);
        StaminaBar->SetFillColorAndOpacity(PurpleColor);
        AngerBar->SetFillColorAndOpacity(PurpleColor);
    }
    
}

void UUserInterfaceRunner::ChangeCharge()
{
    if (StaminaBar && AngerBar)
    {
        // ��������� �����մϴ�.
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

    CurrentTime += InDeltaTime;
    if (bIsTimerRunning)
    {
        CurrentTime += InDeltaTime;

        if (CTime)
        {
            FString FormattedTime;
            FormatTime(CurrentTime, FormattedTime);
            CTime->SetText(FText::FromString(FormattedTime));
        }
    }

    
}
void UUserInterfaceRunner::FormatTime(float TimeInSeconds, FString& OutFormattedTime)
{
    // �ð��� ��:��.�и��� �������� ������
    int32 Minutes = FMath::FloorToInt(TimeInSeconds / 60.0f);
    int32 Seconds = FMath::FloorToInt(TimeInSeconds) % 60;
    int32 Milliseconds = FMath::FloorToInt((TimeInSeconds - FMath::FloorToFloat(TimeInSeconds)) * 1000);

    OutFormattedTime = FString::Printf(TEXT("%02d:%02d.%03d"), Minutes, Seconds, Milliseconds);
}
void UUserInterfaceRunner::NativeConstruct()
{
    Super::NativeConstruct();

    OriginalStaminaColor= StaminaBar->GetFillColorAndOpacity();
    OriginalAngerColor= AngerBar->GetFillColorAndOpacity();
    // ����ý��ۿ� �̺�Ʈ �ڵ鷯 ���ε�
    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
    if (GameInstance)
    {
        UCustomGameSystem* CustomSubsystem = GameInstance->GetSubsystem<UCustomGameSystem>();
        if (CustomSubsystem)
        {
            CustomSubsystem->OnProgressChanged.AddDynamic(this, &UUserInterfaceRunner::UpdateDungeonProgress);
        }
    }
}

void UUserInterfaceRunner::UpdateDungeonProgress(float DeadlineProgress, float CharacterProgress)
{
    if (GameProgressBar)
    {
        // ������� ���൵�� ���α׷��� �ٿ� ����
        GameProgressBar->SetPercent(DeadlineProgress);
    }

    if (MarkerImage)
    {
        UCanvasPanelSlot* MarkerSlot = Cast<UCanvasPanelSlot>(MarkerImage->Slot);
        if (MarkerSlot && GameProgressBar)
        {
            UCanvasPanelSlot* BarSlot = Cast<UCanvasPanelSlot>(GameProgressBar->Slot);
            if (BarSlot)
            {
                // 1. ���α׷��� ���� ���� ���� ��ġ ��� (������ ���)
                FVector2D BarPosition = BarSlot->GetPosition();
                float BarWidth = BarSlot->GetSize().X;

                // 2. ��Ŀ ũ�� ����
                FVector2D MarkerSize = MarkerSlot->GetSize();

                // 3. ���α׷��� �� �������� ����� ��ġ ���
                float ProgressPosition = BarWidth * CharacterProgress;

                // 4. ��Ŀ�� ���� X ��ġ ��� (���α׷��� ���� ������ + ���� ��ġ - ��Ŀ ���� �ʺ�)
                float NewX = BarPosition.X + ProgressPosition - (MarkerSize.X / 2.0f);

                // 5. ��Ŀ�� Y ��ġ�� ���α׷��� �� ���� ������ ����
                // ���α׷��� ���� �߾� Y ��ġ�� ��Ŀ�� ������ ���
                float BarHeight = BarSlot->GetSize().Y;
                //float NewY = BarPosition.Y - (MarkerSize.Y / 2.0f) + (BarHeight / 2.0f);
                float NewY = BarPosition.Y - MarkerSize.Y - 5.0f; // 5�ȼ��� ��Ŀ�� �� ������ ����
                // 6. ��ġ ����
                MarkerSlot->SetPosition(FVector2D(NewX, NewY));
            }
        }
    }
}