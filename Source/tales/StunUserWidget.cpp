// Fill out your copyright notice in the Description page of Project Settings.


#include "StunUserWidget.h"
#include "Components/Button.h"

void UStunUserWidget::Enable()
{
	//SetVisibility(ESlateVisibility::Visible);
}

void UStunUserWidget::Disable()
{
	//SetVisibility(ESlateVisibility::Hidden);
}

void UStunUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{


	Super::NativeTick(MyGeometry, InDeltaTime);

    ToggleButtonColor(InDeltaTime);
}

void UStunUserWidget::ToggleButtonColor(float InDeltaTime)
{
    // 시간 축적
    CurrentTime += InDeltaTime;

    // 지정된 간격에 도달하면 스타일 토글
    if (CurrentTime >= ToggleInterval)
    {
        bool retFlag;
        ChangeButtonColor(retFlag);

        CurrentTime = 0.0f; // 타이머 리셋

    }
}

void UStunUserWidget::ChangeButtonColor(bool& retFlag)
{
    retFlag = true;
    if (!ButtonLeft || !ButtonRight)
        return;

    // 버튼 색상 스타일 가져오기
    FSlateBrush LeftBrush = ButtonLeft->WidgetStyle.Normal;
    FSlateBrush RightBrush = ButtonRight->WidgetStyle.Normal;

    if (bIsLeftButtonWhite)
    {
        // 왼쪽 버튼: 흰색 -> 노란색, 오른쪽 버튼: 노란색 -> 흰색
        LeftBrush.TintColor = YellowColor;
        RightBrush.TintColor = WhiteColor;
    }
    else
    {
        // 왼쪽 버튼: 노란색 -> 흰색, 오른쪽 버튼: 흰색 -> 노란색
        LeftBrush.TintColor = WhiteColor;
        RightBrush.TintColor = YellowColor;
    }

    // 스타일 적용
    ButtonLeft->WidgetStyle.Normal = LeftBrush;
    ButtonLeft->WidgetStyle.Hovered = LeftBrush;
    ButtonLeft->WidgetStyle.Pressed = LeftBrush;

    ButtonRight->WidgetStyle.Normal = RightBrush;
    ButtonRight->WidgetStyle.Hovered = RightBrush;
    ButtonRight->WidgetStyle.Pressed = RightBrush;

    // 상태 토글
    bIsLeftButtonWhite = !bIsLeftButtonWhite;
    retFlag = false;
}

void UStunUserWidget::NativeConstruct()
{


	Super::NativeConstruct();
}
