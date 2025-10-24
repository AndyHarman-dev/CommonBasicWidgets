// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Input/SSlider.h"

/**
 * Extends the default slider by adding
 * an effect of a filled progress as slider
 * goes from left to right.
 */
class COMMONBASICWIDGETS_API SProgressBarSlider : public SSlider
{
public:
	SLATE_BEGIN_ARGS(SProgressBarSlider)
		: _IndentHandle(true)
		, _MouseUsesStep(false)
		, _RequiresControllerLock(true)
		, _Locked(false)
		, _Orientation(EOrientation::Orient_Horizontal)
		, _SliderBarColor(FLinearColor::White)
		, _SliderHandleColor(FLinearColor::White)
		, _Style(&FCoreStyle::Get().GetWidgetStyle<FSliderStyle>("Slider"))
		, _StepSize(0.01f)
		, _Value(1.f)
		, _MinValue(0.0f)
		, _MaxValue(1.0f)
		, _IsFocusable(true)
		{
		}

#pragma region SuperArgs
		
		/** Whether the slidable area should be indented to fit the handle. */
		SLATE_ATTRIBUTE( bool, IndentHandle )

		/** Sets new value if mouse position is greater/less than half the step size. */
		SLATE_ARGUMENT( bool, MouseUsesStep )

		/** Sets whether we have to lock input to change the slider value. */
		SLATE_ARGUMENT( bool, RequiresControllerLock )

		/** Whether the handle is interactive or fixed. */
		SLATE_ATTRIBUTE( bool, Locked )

		/** The slider's orientation. */
		SLATE_ARGUMENT( EOrientation, Orientation)

		/** The color to draw the slider bar in. */
		SLATE_ATTRIBUTE( FSlateColor, SliderBarColor )

		/** The color to draw the slider handle in. */
		SLATE_ATTRIBUTE( FSlateColor, SliderHandleColor )

		/** The style used to draw the slider. */
		SLATE_STYLE_ARGUMENT( FSliderStyle, Style )

		/** The input mode while using the controller. */
		SLATE_ATTRIBUTE(float, StepSize)

		/** A value that drives where the slider handle appears. Value is normalized between 0 and 1. */
		SLATE_ATTRIBUTE( float, Value )

		/** The minimum value that can be specified by using the slider. */
		SLATE_ARGUMENT(float, MinValue)
		/** The maximum value that can be specified by using the slider. */
		SLATE_ARGUMENT(float, MaxValue)

		/** Sometimes a slider should only be mouse-clickable and never keyboard focusable. */
		SLATE_ARGUMENT(bool, IsFocusable)

		/** Invoked when the mouse is pressed and a capture begins. */
		SLATE_EVENT(FSimpleDelegate, OnMouseCaptureBegin)

		/** Invoked when the mouse is released and a capture ends. */
		SLATE_EVENT(FSimpleDelegate, OnMouseCaptureEnd)

		/** Invoked when the Controller is pressed and capture begins. */
		SLATE_EVENT(FSimpleDelegate, OnControllerCaptureBegin)

		/** Invoked when the controller capture is released.  */
		SLATE_EVENT(FSimpleDelegate, OnControllerCaptureEnd)

		/** Called when the value is changed by the slider. */
		SLATE_EVENT( FOnFloatValueChanged, OnValueChanged )

#pragma endregion

		SLATE_ATTRIBUTE(const FSlateBrush*, LeftSideImage)
		SLATE_ATTRIBUTE(float, ThumbOffset)

	SLATE_END_ARGS()

	/** Constructs this widget with InArgs */
	void Construct(const FArguments& InArgs);

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;

protected:

	TAttribute<const FSlateBrush*> LeftSideImage;
	TAttribute<float> ThumbOffset;
	
};
