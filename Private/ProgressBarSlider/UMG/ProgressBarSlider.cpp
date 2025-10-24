// Fill out your copyright notice in the Description page of Project Settings.


#include "ProgressBarSlider/UMG/ProgressBarSlider.h"

#include "ProgressBarSlider/Slate/SProgressBarSlider.h"

TSharedRef<SWidget> UProgressBarSlider::RebuildWidget()
{
	
	//@note: if USlider is changed in any way, please update this. Check Super::RebuildWidget()
	
	PRAGMA_DISABLE_DEPRECATION_WARNINGS
		MySlider = SNew(SProgressBarSlider)
			.Style(StyleAsset ? StyleAsset->GetStyleChecked<FSliderStyle>() : &WidgetStyle)
			.LeftSideImage_Lambda([this] { return &ProgressSideImage; })
			.ThumbOffset_Lambda([this] { return ThumbOffset; })
			.IsFocusable(IsFocusable)
			.OnMouseCaptureBegin(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnMouseCaptureBegin))
			.OnMouseCaptureEnd(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnMouseCaptureEnd))
			.OnControllerCaptureBegin(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnControllerCaptureBegin))
			.OnControllerCaptureEnd(BIND_UOBJECT_DELEGATE(FSimpleDelegate, HandleOnControllerCaptureEnd))
			.OnValueChanged(BIND_UOBJECT_DELEGATE(FOnFloatValueChanged, HandleOnValueChanged));
	PRAGMA_ENABLE_DEPRECATION_WARNINGS

		return MySlider.ToSharedRef();
}

const FText UProgressBarSlider::GetPaletteCategory()
{
	return NSLOCTEXT("CommonBasicWidgets", "CommonBasicWidgets", "CommonBasicWidgets");
}
