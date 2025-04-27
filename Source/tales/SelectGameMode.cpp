// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectGameMode.h"
#include "SelectGameWidget.h"
#include "Kismet/GameplayStatics.h"

void ASelectGameMode::BeginPlay()
{
    Super::BeginPlay();

    // ���� Ŭ������ �����Ǿ����� Ȯ��
    if (MenuWidgetClass)
    {
        // ���� ����
        MenuWidget = Cast<USelectGameWidget>(CreateWidget(GetWorld(), MenuWidgetClass));

        if (MenuWidget)
        {
            // ����Ʈ�� ���� �߰�
            MenuWidget->AddToViewport();

            // �Է� ��� ����
            APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
            if (PlayerController)
            {
                FInputModeUIOnly InputMode;
                InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
                PlayerController->SetInputMode(InputMode);
                PlayerController->bShowMouseCursor = true;
            }
        }
    }
}