// Fill out your copyright notice in the Description page of Project Settings.


#include "EndingWidget.h"
#include "Kismet/GameplayStatics.h"
#include "SubSystem/CustomGameSystem.h"
#include "Components/TextBlock.h"

void UEndingWidget::NativeConstruct()
{
    float ClearTime=0.f;
    bool IsClear = false;
    UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(GetWorld());
    if (GameInstance)
    {
        UCustomGameSystem* CustomSubsystem = GameInstance->GetSubsystem<UCustomGameSystem>();
        if (CustomSubsystem)
        {
            ClearTime=CustomSubsystem->ClearTime;
        }
    }

    if (ClearTime == 0.f)
    {
        ExplainText->SetText(FText::FromString(TEXT("탈락")));
        RecordText->SetText(FText::FromString(FString::Printf(TEXT(""))));
    }
    else
    {
        ExplainText->SetText(FText::FromString(FString::Printf(TEXT("클리어"))));
        FString FormattedTime;
        FormatTime(ClearTime, FormattedTime);
        RecordText->SetText(FText::FromString(FString::Printf(TEXT("%s"), *FormattedTime)));
    }
}

void UEndingWidget::FormatTime(float TimeInSeconds, FString& OutFormattedTime)//나중에 싱글톤 함수로 만들어서 userinterfacerunner랑 여기서 불러오는 식으로 설계해도 좋을듯
{
    int32 Minutes = FMath::FloorToInt(TimeInSeconds / 60.0f);
    int32 Seconds = FMath::FloorToInt(TimeInSeconds) % 60;
    int32 Milliseconds = FMath::FloorToInt(FMath::Fmod(TimeInSeconds, 1.0f) * 100);

    OutFormattedTime = FString::Printf(TEXT("%02d:%02d.%02d"), Minutes, Seconds, Milliseconds);
}