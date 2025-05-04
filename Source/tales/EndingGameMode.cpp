// Fill out your copyright notice in the Description page of Project Settings.


#include "EndingGameMode.h"
#include "EndingWidget.h"
#include "Kismet/GameplayStatics.h"

void AEndingGameMode::BeginPlay()
{
    // 엔딩 UI 생성 및 표시
    if (EndingWidgetClass)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
        if (PC)
        {
            UUserWidget* EndingWidget = CreateWidget<UUserWidget>(PC, EndingWidgetClass);
            if (EndingWidget)
            {
                EndingWidget->AddToViewport();

                // 입력 모드 설정 (UI에만 입력 초점)
                FInputModeUIOnly InputMode;
                InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
                PC->SetInputMode(InputMode);
                PC->bShowMouseCursor = true;
            }
        }
    }
}
