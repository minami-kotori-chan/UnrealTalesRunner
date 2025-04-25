// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StunUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class TALES_API UStunUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(meta = (BindWidget))
	class UButton* ButtonLeft;

	UPROPERTY(meta = (BindWidget))
	class UButton* ButtonRight;

	UFUNCTION(BlueprintCallable, Category = "Enable")
	void Enable();
	UFUNCTION(BlueprintCallable, Category = "Disable")
	void Disable();

protected:
	//ƽ �� ������
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	void ToggleButtonColor(float InDeltaTime);
	void ChangeButtonColor(bool& retFlag);
	virtual void NativeConstruct() override;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Interval")
	float ToggleInterval = 0.1; // 1�� ����
private:

	float CurrentTime = 0.0f;
	
	FLinearColor WhiteColor = FLinearColor(1.0f, 1.0f, 1.0f, 1.0f);  // ���
	FLinearColor YellowColor = FLinearColor(1.0f, 1.0f, 0.0f, 1.0f); // �����
	bool bIsLeftButtonWhite = true;
};
