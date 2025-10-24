// Fill out your copyright notice in the Description page of Project Settings.


#include "MinMaxSlider/UMG/MinMaxSlider.h"
#include "MinMaxSlider/Slate/SMinMaxSlider.h"

void UMinMaxSlider::SetLowerHandleValue(float InLowerHandleValue)
{
	if (MyMinMaxSliderWidget.IsValid())
	{
		MyMinMaxSliderWidget->SetLowerValue(InLowerHandleValue);
	}
}

void UMinMaxSlider::SetUpperHandleValue(float InUpperHandleValue)
{
	if (MyMinMaxSliderWidget.IsValid())
	{
		MyMinMaxSliderWidget->SetUpperValue(InUpperHandleValue);
	}
}

float UMinMaxSlider::GetLowerHandleValue() const noexcept
{
	if (MyMinMaxSliderWidget.IsValid())
	{
		return MyMinMaxSliderWidget->GetLowerValue();
	}

	return 0.f;
}

float UMinMaxSlider::GetUpperHandleValue() const noexcept
{
	if (MyMinMaxSliderWidget.IsValid())
	{
		return MyMinMaxSliderWidget->GetUpperValue();
	}

	return 0.f;
}

void UMinMaxSlider::SetMinAndMaxValues(const float InMinValue, const float InMaxValue)
{
	if (MyMinMaxSliderWidget.IsValid())
	{
		MyMinMaxSliderWidget->SetMinAndMaxValues(InMinValue, InMaxValue);
	}
}

float UMinMaxSlider::GetMinValue() const noexcept
{
	if (MyMinMaxSliderWidget)
	{
		return MyMinMaxSliderWidget->GetMinValue();
	}

	return 0.f;
}

float UMinMaxSlider::GetMaxValue() const noexcept
{
	if (MyMinMaxSliderWidget)
	{
		return MyMinMaxSliderWidget->GetMaxValue();
	}

	return 0.f;
}

const FText UMinMaxSlider::GetPaletteCategory()
{
	return NSLOCTEXT("CommonBasicWidgets", "CommonBasicWidgets", "CommonBasicWidgets");
}

TSharedRef<SWidget> UMinMaxSlider::RebuildWidget()
{
	if (WidgetStyle)
	{
		Style = *WidgetStyle->GetStyleChecked<FSliderStyle>();
	}
	
	return SAssignNew(MyMinMaxSliderWidget, SMinMaxSlider)
		.Style(&Style)
		.ImageBetweenThums_Lambda([this]{ return ImageBetweenThumbs; })
		.IndentHandle(bHasIndentHandle)
		.MinValue(MinValue)
		.MaxValue(MaxValue)
		.LowerHandleValue_Lambda([this]{ return LowerHandleValue; })
		.SliderBarColor_Lambda([this] { return SliderBarColor; })
		.UpperHandleValue_Lambda([this]{ return UpperHandleValue; })
		.LowerSliderOffset_Lambda([this] { return LowerSliderOffset; })
		.UpperSliderOffset_Lambda([this]{  return UpperSliderOffset; })
		.SliderLowerHandleColor_Lambda([this]{ return SliderLowerHandleColor; })
		.SliderUpperHandleColor_Lambda([this] { return SliderUpperHandleColor; })
		.OnLowerHandleValueChanged_UObject(this, &UMinMaxSlider::HandleLowerHandleValueChanged)
		.OnUpperHandleValueChanged_UObject(this, &UMinMaxSlider::HandleUpperHandleValueChanged);
		
}

void UMinMaxSlider::ReleaseSlateResources(bool bReleaseChildren)
{
	Super::ReleaseSlateResources(bReleaseChildren);
	MyMinMaxSliderWidget.Reset();
}

void UMinMaxSlider::HandleLowerHandleValueChanged(float InLowerHandleValue)
{
	OnLowerHandleValueChanged.Broadcast(InLowerHandleValue);
	OnLowerHandleValueChangedNative.Broadcast(InLowerHandleValue);
}

void UMinMaxSlider::HandleUpperHandleValueChanged(float InUpperHandleValue)
{
	OnUpperHandleValueChanged.Broadcast(InUpperHandleValue);
	OnUpperHandleValueChangedNative.Broadcast(InUpperHandleValue);
}
