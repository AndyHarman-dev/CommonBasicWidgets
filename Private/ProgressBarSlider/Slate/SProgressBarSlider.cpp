// Fill out your copyright notice in the Description page of Project Settings.


#include "ProgressBarSlider/Slate/SProgressBarSlider.h"
#include "SlateOptMacros.h"
#include "Kismet/KismetMathLibrary.h"

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SProgressBarSlider::Construct(const FArguments& InArgs)
{
	SSlider::Construct(SSlider::FArguments()
	.Locked(InArgs._Locked)
	.Orientation(InArgs._Orientation)
	.IndentHandle(InArgs._IndentHandle)
	.MouseUsesStep(InArgs._MouseUsesStep)
	.RequiresControllerLock(InArgs._RequiresControllerLock)
	.SliderBarColor(InArgs._SliderBarColor)
	.SliderHandleColor(InArgs._SliderHandleColor)
	.Style(InArgs._Style)
	.StepSize(InArgs._StepSize)
	.IsFocusable(InArgs._IsFocusable)
	.OnMouseCaptureBegin(InArgs._OnMouseCaptureBegin)
	.OnMouseCaptureEnd(InArgs._OnMouseCaptureEnd)
	.OnValueChanged(InArgs._OnValueChanged)
	.Value(InArgs._Value)
	.MinValue(InArgs._MinValue)
	.MaxValue(InArgs._MaxValue)
	.OnControllerCaptureBegin(InArgs._OnControllerCaptureBegin)
	.OnControllerCaptureEnd(InArgs._OnControllerCaptureEnd)
	);

	LeftSideImage = InArgs._LeftSideImage;
	ThumbOffset = InArgs._ThumbOffset;
	
}

int32 SProgressBarSlider::OnPaint(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	 bool bParentEnabled) const
{
	LayerId = SSlider::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);

	if (LeftSideImage.Get())
	{
		FSlateBrush LBrush = *LeftSideImage.Get();
		
		const float LAllottedWidth = Orientation == Orient_Horizontal ? AllottedGeometry.GetLocalSize().X : AllottedGeometry.GetLocalSize().Y;
		const float LAllottedHeight = Orientation == Orient_Horizontal ? AllottedGeometry.GetLocalSize().Y : AllottedGeometry.GetLocalSize().X;
		const float LIndentation = GetIndentHandleAttribute().Get() ? GetThumbImage()->ImageSize.X : 0.0f;
		const float LSliderPercent = FMath::Clamp(GetNormalizedValue(), 0.0f, 1.0f);
		const float LSliderLength = LAllottedWidth - (LIndentation + GetThumbImage()->ImageSize.X);
		const float LSliderHandleOffset = LSliderPercent * LSliderLength + ThumbOffset.Get();
		const float LBarThickness = Style->BarThickness;
		
		// Calculate the position and size of the left side image
		const FVector2f LLeftSideTopLeft(LIndentation * 0.5f, (LAllottedHeight - LBarThickness) * 0.5f);
		const FVector2f LLeftSideSize(LSliderHandleOffset, LBarThickness);
		
		// Convert the radii to linear color for lerping
		const FLinearColor LBarCornerRadii(GetBarImage()->OutlineSettings.CornerRadii);
		const FLinearColor LLeftSideImageCornerRadii(LBrush.OutlineSettings.CornerRadii);

		// Calculate the distance from the center.
		const float LAlphaAsDistance = FMath::Clamp((0.5f - FMath::Abs(0.5f - LSliderPercent)) * 2.0f, 0.0f, 1.0f);

		// Lerp the radii and override it
		LBrush.OutlineSettings.CornerRadii = UKismetMathLibrary::LinearColorLerp(LBarCornerRadii, LLeftSideImageCornerRadii, LAlphaAsDistance);
		
		FSlateDrawElement::MakeBox(
			OutDrawElements,
			LayerId + 1,
			AllottedGeometry.ToPaintGeometry(LLeftSideSize, FSlateLayoutTransform(LLeftSideTopLeft)),
			&LBrush,
			ESlateDrawEffect::None,
			LeftSideImage.Get()->GetTint(InWidgetStyle) * InWidgetStyle.GetColorAndOpacityTint()
		);
	}

	return LayerId + 1;
}

END_SLATE_FUNCTION_BUILD_OPTIMIZATION
