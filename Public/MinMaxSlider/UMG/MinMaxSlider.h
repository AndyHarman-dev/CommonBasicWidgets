// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MinMaxSlider.generated.h"

/**
 * Wrapper for SMinMaxSlider
 */
UCLASS()
class COMMONBASICWIDGETS_API UMinMaxSlider : public UWidget
{
	GENERATED_BODY()

public:

	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHandleValueChanged, const float, InLowerHandleValue);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnHandleValueChangedNative, const float);

	UPROPERTY(BlueprintAssignable)
	FOnHandleValueChanged OnLowerHandleValueChanged;
	FOnHandleValueChangedNative OnLowerHandleValueChangedNative;

	UPROPERTY(BlueprintAssignable)
	FOnHandleValueChanged OnUpperHandleValueChanged;
	FOnHandleValueChangedNative OnUpperHandleValueChangedNative;

	UFUNCTION(BlueprintCallable)
	void SetLowerHandleValue(float InLowerHandleValue);
	
	UFUNCTION(BlueprintCallable)
	void SetUpperHandleValue(float InUpperHandleValue);

	UFUNCTION(BlueprintPure)
	float GetLowerHandleValue() const noexcept;
	
	UFUNCTION(BlueprintPure)
	float GetUpperHandleValue() const noexcept;

	UFUNCTION(BlueprintCallable)
	void SetMinAndMaxValues(const float InMinValue, const float InMaxValue);

	UFUNCTION(BlueprintPure)
	float GetMinValue() const noexcept;

	UFUNCTION(BlueprintPure)
	float GetMaxValue() const noexcept;

#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

protected:

	UE_DEPRECATED(5.4, "Direct access to LowerHandleValue is deprecated. Please use the getter or setter.")
	/** The minimum value the slider can be set to. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, BlueprintSetter="SetLowerHandleValue", Category="Appearance|Slider")
	float LowerHandleValue;

	UE_DEPRECATED(5.4, "Direct access to UpperHandleValue is deprecated. Please use the getter or setter.")
	/** The maximum value the slider can be set to. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Getter, Setter, BlueprintSetter="SetUpperHandleValue", Category="Appearance|Slider")
	float UpperHandleValue;

	/** The color to draw the slider bar in. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance")
	FSlateColor SliderBarColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Appearance")
	class USlateWidgetStyleAsset* WidgetStyle;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(EditCondition="WidgetStyle==nullptr"), Category="Appearance")
	FSliderStyle Style;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Appearance")
	FSlateBrush ImageBetweenThumbs;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Appearance|Slider")
	float MinValue;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Appearance|Slider")
	float MaxValue;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Appearance|Slider")
	float LowerSliderOffset;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Appearance|Slider")
	float UpperSliderOffset;
	
	/** The color to draw the slider bar in. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance|Slider")
	FLinearColor SliderLowerHandleColor;

	/** The color to draw the slider handle in. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Appearance|Slider")
	FLinearColor SliderUpperHandleColor;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Appearance|Slider")
	bool bHasIndentHandle;
	
	TSharedPtr<class SMinMaxSlider> MyMinMaxSliderWidget = nullptr;
	
	virtual TSharedRef<SWidget> RebuildWidget() override;
	virtual void ReleaseSlateResources(bool bReleaseChildren) override;

	/** Triggers when the lower handle value changes */
	void HandleLowerHandleValueChanged(float InLowerHandleValue);

	/** Triggers when the upper handle value changes */
	void HandleUpperHandleValueChanged(float InUpperHandleValue);
	
};