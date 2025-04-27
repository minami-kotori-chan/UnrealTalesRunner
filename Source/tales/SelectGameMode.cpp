// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectGameMode.h"
#include "SelectGameWidget.h"
#include "Kismet/GameplayStatics.h"

void ASelectGameMode::BeginPlay()
{
    Super::BeginPlay();

    // 위젯 클래스가 설정되었는지 확인
    if (MenuWidgetClass)
    {
        // 위젯 생성
        MenuWidget = Cast<USelectGameWidget>(CreateWidget(GetWorld(), MenuWidgetClass));

        if (MenuWidget)
        {
            // 뷰포트에 위젯 추가
            MenuWidget->AddToViewport();

            // 입력 모드 설정
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