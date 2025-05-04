// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndingWidget.generated.h"

/**
 * 
 */
UCLASS()
class TALES_API UEndingWidget : public UUserWidget
{
	GENERATED_BODY()
public:

protected:
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* ExplainText;
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* RecordText;

	virtual void NativeConstruct() override;
	void FormatTime(float TimeInSeconds, FString& OutFormattedTime);
private:

};
