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
    // �ð� ����
    CurrentTime += InDeltaTime;

    // ������ ���ݿ� �����ϸ� ��Ÿ�� ���
    if (CurrentTime >= ToggleInterval)
    {
        bool retFlag;
        ChangeButtonColor(retFlag);

        CurrentTime = 0.0f; // Ÿ�̸� ����

    }
}

void UStunUserWidget::ChangeButtonColor(bool& retFlag)
{
    retFlag = true;
    if (!ButtonLeft || !ButtonRight)
        return;

    // ��ư ���� ��Ÿ�� ��������
    FSlateBrush LeftBrush = ButtonLeft->WidgetStyle.Normal;
    FSlateBrush RightBrush = ButtonRight->WidgetStyle.Normal;

    if (bIsLeftButtonWhite)
    {
        // ���� ��ư: ��� -> �����, ������ ��ư: ����� -> ���
        LeftBrush.TintColor = YellowColor;
        RightBrush.TintColor = WhiteColor;
    }
    else
    {
        // ���� ��ư: ����� -> ���, ������ ��ư: ��� -> �����
        LeftBrush.TintColor = WhiteColor;
        RightBrush.TintColor = YellowColor;
    }

    // ��Ÿ�� ����
    ButtonLeft->WidgetStyle.Normal = LeftBrush;
    ButtonLeft->WidgetStyle.Hovered = LeftBrush;
    ButtonLeft->WidgetStyle.Pressed = LeftBrush;

    ButtonRight->WidgetStyle.Normal = RightBrush;
    ButtonRight->WidgetStyle.Hovered = RightBrush;
    ButtonRight->WidgetStyle.Pressed = RightBrush;

    // ���� ���
    bIsLeftButtonWhite = !bIsLeftButtonWhite;
    retFlag = false;
}

void UStunUserWidget::NativeConstruct()
{


	Super::NativeConstruct();
}
