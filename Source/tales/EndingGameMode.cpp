// Fill out your copyright notice in the Description page of Project Settings.


#include "EndingGameMode.h"
#include "EndingWidget.h"
#include "Kismet/GameplayStatics.h"

void AEndingGameMode::BeginPlay()
{
    // ���� UI ���� �� ǥ��
    if (EndingWidgetClass)
    {
        APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
        if (PC)
        {
            UUserWidget* EndingWidget = CreateWidget<UUserWidget>(PC, EndingWidgetClass);
            if (EndingWidget)
            {
                EndingWidget->AddToViewport();

                // �Է� ��� ���� (UI���� �Է� ����)
                FInputModeUIOnly InputMode;
                InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
                PC->SetInputMode(InputMode);
                PC->bShowMouseCursor = true;
            }
        }
    }
}
