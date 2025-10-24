// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/Slider.h"
#include "ProgressBarSlider.generated.h"


/**
 * Extends default slider by adding
 * a progress bar like filling for it.
 */
UCLASS()
class COMMONMENUEXTENSIONS_API UProgressBarSlider : public USlider
{
	GENERATED_BODY()
	
protected:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	USlateWidgetStyleAsset* StyleAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	FSlateBrush ProgressSideImage;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Appearance")
	float ThumbOffset;

protected:
	virtual TSharedRef<SWidget> RebuildWidget() override;
#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif
};
