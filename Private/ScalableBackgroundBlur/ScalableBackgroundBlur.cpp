// Fill out your copyright notice in the Description page of Project Settings.


#include "ScalableBackgroundBlur/ScalableBackgroundBlur.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Widgets/Layout/SBackgroundBlur.h"

TSharedRef<SWidget> UScalableBackgroundBlur::RebuildWidget()
{
	const auto SuperWidget = Super::RebuildWidget();
	ScaleCornerRadius();
	return SuperWidget;
}

void UScalableBackgroundBlur::PostInitProperties()
{
	Super::PostInitProperties();

	if (CanRunTick() && !IsDesignTime())
	{
		FTickerDelegate Delegate;
		Delegate.BindUObject(this, &ThisClass::Tick);
		TickerHandle = FTSTicker::GetCoreTicker().AddTicker(Delegate);
	}
}

bool UScalableBackgroundBlur::CanRunTick() const noexcept
{
	return bUpdateCornerRadiusOnTick && !TickerHandle.IsValid() && !HasAnyFlags(RF_ClassDefaultObject);
}

bool UScalableBackgroundBlur::Tick(float DeltaTime)
{
	ScaleCornerRadius();
	return true;
}

void UScalableBackgroundBlur::ScaleCornerRadius()
{
	const auto ViewportScale = UWidgetLayoutLibrary::GetViewportScale(this);
	const auto ScaledCornerRadius = GetCornerRadius() * ViewportScale;
	if (MyBackgroundBlur.IsValid())
	{
		MyBackgroundBlur->SetCornerRadius(ScaledCornerRadius);
	}
}

void UScalableBackgroundBlur::BeginDestroy()
{
	if (TickerHandle.IsValid())
	{
		FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);
	}
	
	Super::BeginDestroy();
}
