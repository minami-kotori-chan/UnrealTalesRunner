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
    // ���� ������ ��ȿ���� Ȯ��
    
    if (Slider && SliderValue)
    {
        // �����̴� �̺�Ʈ�� �Լ� ���ε�
        Slider->OnValueChanged.AddDynamic(this, &USelectGameWidget::OnSliderValueChanged);

        // �ʱⰪ ����
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
        // �����̴� ���� �ؽ�Ʈ�� ��ȯ�Ͽ� ǥ��
        // �Ҽ��� 1�ڸ����� ǥ�� (�ʿ信 ���� ���� ����)
        FString ValueString = FString::Printf(TEXT("%.0f"), Value);
        SliderValue->SetText(FText::FromString(ValueString));
    }
    
    
}

void USelectGameWidget::OnStartButtonClicked()
{
    SetSubSystemValue(Slider->GetValue());

    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(LoadingImage->Slot))
    {
        CanvasSlot->SetZOrder(10); // ���� ���� �տ� ǥ�õ�
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
    // �Է� ��带 ���� ���� ����
    APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
    if (PlayerController)
    {
        // ���� ���� ���� (UI ��忡�� �Ϲ� ���� ���� ����)
        FInputModeGameOnly InputMode;
        PlayerController->SetInputMode(InputMode);
        // ���콺 Ŀ�� �����
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
