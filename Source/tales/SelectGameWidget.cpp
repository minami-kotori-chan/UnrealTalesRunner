// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectGameWidget.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"
#include "SubSystem/CustomGameSystem.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"
#include "Components/Image.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void USelectGameWidget::NativeConstruct()
{
    Super::NativeConstruct();
    // 위젯 참조가 유효한지 확인
    
    if (Slider && SliderValue)
    {
        // 슬라이더 이벤트에 함수 바인딩
        Slider->OnValueChanged.AddDynamic(this, &USelectGameWidget::OnSliderValueChanged);

        // 초기값 설정
        OnSliderValueChanged(Slider->GetValue());
    }
    if (StartButton)
    {
        StartButton->OnClicked.AddDynamic(this, &USelectGameWidget::OnStartButtonClicked);
    }
}

void USelectGameWidget::OnSliderValueChanged(float Value)
{
    if (GEngine)
    {
        //FString DistanceLog = FString::Printf(TEXT("Boolean: %d"), DashVfxComponent->bHiddenInGame);
        FString DistanceLog = FString::Printf(TEXT("GetValue: %f"), Value);
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, DistanceLog);
    }

    if (SliderValue)
    {
        // 슬라이더 값을 텍스트로 변환하여 표시
        // 소수점 1자리까지 표시 (필요에 따라 조정 가능)
        FString ValueString = FString::Printf(TEXT("%.0f"), Value);
        SliderValue->SetText(FText::FromString(ValueString));
    }
    
    
}

void USelectGameWidget::OnStartButtonClicked()
{
    SetSubSystemValue(Slider->GetValue());

    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(LoadingImage->Slot))
    {
        CanvasSlot->SetZOrder(10); // 높은 값이 앞에 표시됨
        UE_LOG(LogTemp, Warning, TEXT("SetZorder."));
    }
    ChangeGameInputMode();
    GetWorld()->GetTimerManager().SetTimerForNextTick(this, &USelectGameWidget::OpenLevelWithDelay);
}

void USelectGameWidget::OpenLevelWithDelay()
{
    UGameplayStatics::OpenLevel(GetWorld(), FName("RunningMap"));
}

void USelectGameWidget::ChangeGameInputMode()
{
    // 입력 모드를 게임 모드로 변경
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        // 게임 모드로 설정 (UI 모드에서 일반 게임 모드로 변경)
        FInputModeGameOnly InputMode;
        PlayerController->SetInputMode(InputMode);
        // 마우스 커서 숨기기
        PlayerController->bShowMouseCursor = false;
    }
    
}

void USelectGameWidget::SetSubSystemValue(float Value)
{
    UGameInstance* GameInstance = GetGameInstance();
    if (GameInstance)
    {
        UCustomGameSystem* CustomSubsystem = GameInstance->GetSubsystem<UCustomGameSystem>();
        if (CustomSubsystem)
        {
            CustomSubsystem->SetDungeonLength(Value);
        }
    }
}
