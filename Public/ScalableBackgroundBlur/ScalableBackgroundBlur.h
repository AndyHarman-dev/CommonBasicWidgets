// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BackgroundBlur.h"
#include "ScalableBackgroundBlur.generated.h"

/**
 * 
 */
UCLASS()
class COMMONBASICWIDGETS_API UScalableBackgroundBlur : public UBackgroundBlur
{
	GENERATED_BODY()
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Appearance")
	bool bUpdateCornerRadiusOnTick = false;
	
	virtual TSharedRef<SWidget> RebuildWidget() override;
private:
	FTSTicker::FDelegateHandle TickerHandle;
	virtual void PostInitProperties() override;
	bool CanRunTick() const noexcept;
	bool Tick(float DeltaTime);
	void ScaleCornerRadius();

	virtual void BeginDestroy() override;
};
