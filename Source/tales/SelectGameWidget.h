// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SelectGameWidget.generated.h"

/**
 * 
 */
UCLASS()
class TALES_API USelectGameWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(meta = (BindWidget))
	class USlider* Slider;

	UPROPERTY(meta = (BindWidget))
	class UTextBlock* SliderValue;

	UPROPERTY(meta = (BindWidget))
	class UButton* StartButton;

	UPROPERTY(meta = (BindWidget))
	class UImage* LoadingImage;
	
	
protected:
	virtual void NativeConstruct() override;

	UFUNCTION()
	void OnSliderValueChanged(float Value);
	UFUNCTION()
	void OnStartButtonClicked();
	void OpenLevelWithDelay();
	void ChangeGameInputMode();
	void SetSubSystemValue(float Value);


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level")
	FName LevelToLoad;
private:
	
};
