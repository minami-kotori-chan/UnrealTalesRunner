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
    // 시간을 분:초.밀리초 형식으로 포맷팅
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
    // 서브시스템에 이벤트 핸들러 바인딩
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
        // 데드라인 진행도를 프로그레스 바에 적용
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
                // 1. 프로그레스 바의 실제 기준 위치 계산 (오프셋 고려)
                FVector2D BarPosition = BarSlot->GetPosition();
                float BarWidth = BarSlot->GetSize().X;

                // 2. 마커 크기 정보
                FVector2D MarkerSize = MarkerSlot->GetSize();

                // 3. 프로그레스 바 내에서의 상대적 위치 계산
                float ProgressPosition = BarWidth * CharacterProgress;

                // 4. 마커의 최종 X 위치 계산 (프로그레스 바의 시작점 + 진행 위치 - 마커 절반 너비)
                float NewX = BarPosition.X + ProgressPosition - (MarkerSize.X / 2.0f);

                // 5. 마커의 Y 위치는 프로그레스 바 위에 오도록 조정
                // 프로그레스 바의 중앙 Y 위치에 마커가 오도록 계산
                float BarHeight = BarSlot->GetSize().Y;
                //float NewY = BarPosition.Y - (MarkerSize.Y / 2.0f) + (BarHeight / 2.0f);
                float NewY = BarPosition.Y - MarkerSize.Y - 5.0f; // 5픽셀은 마커와 바 사이의 간격
                // 6. 위치 설정
                MarkerSlot->SetPosition(FVector2D(NewX, NewY));
            }
        }
    }
}