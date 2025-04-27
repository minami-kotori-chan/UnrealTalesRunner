// Fill out your copyright notice in the Description page of Project Settings.


#include "SelectGameWidget.h"
#include "Components/Slider.h"
#include "Components/TextBlock.h"

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