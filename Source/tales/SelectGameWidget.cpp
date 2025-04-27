// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectGameWidget.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"

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