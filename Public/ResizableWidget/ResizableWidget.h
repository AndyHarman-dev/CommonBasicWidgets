// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResizableWidget.generated.h"

UENUM(BlueprintType)
enum class EResizeHandle : uint8
{
	None,
	TopLeft,
	Top,
	TopRight,
	Right,
	BottomRight,
	Bottom,
	BottomLeft,
	Left
};

UCLASS()
class UNullWidget : public UWidget
{
	GENERATED_BODY()

public:

	static UNullWidget* NullWidget();

protected:

	virtual TSharedRef<SWidget> RebuildWidget() override;
	
};

USTRUCT(BlueprintType)
struct FResizeSettings
{
	GENERATED_BODY()
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resize Settings")
	float HandleSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resize Settings")
	FVector2D MinSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resize Settings")
	FVector2D MaxSize;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Resize Settings")
	bool bEnableResize;
};

USTRUCT(BlueprintType)
struct FCornerSettings
{
	GENERATED_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Corner Indicator Settings")
	bool bShowCornerIndicators;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Corner Indicator Settings")
	FLinearColor CornerIndicatorColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Corner Indicator Settings")
	float CornerIndicatorThickness;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Corner Indicator Settings")
	float CornerIndicatorLength;
};

/**
 * 
 */
UCLASS()
class COMMONBASICWIDGETS_API UResizableWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	virtual int32 NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FCursorReply NativeOnCursorQuery(const FGeometry& InGeometry, const FPointerEvent& InCursorEvent) override;

#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
#endif
public:
	UResizableWidget(const FObjectInitializer& ObjectInitializer);
	
	UFUNCTION(BlueprintCallable, Category = "Resizable Widget")
	void SetContent(UWidget* Content);

	UFUNCTION(BlueprintPure, Category = "Resizable Widget")
	UWidget* GetContent() const noexcept;

private:
	UPROPERTY(BlueprintReadWrite, meta = (BindWidget, AllowPrivateAccess))
	UNamedSlot* ContentSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess, ShowOnlyInnerProperties), Category = "Resize Settings")
	FResizeSettings ResizeSettings;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta=(AllowPrivateAccess, ShowOnlyInnerProperties), Category = "Resize Settings")
	FCornerSettings CornerSettings;
	
	EResizeHandle GetHandleAtPosition(const FGeometry& InGeometry, const FVector2D& LocalMousePosition) const;
	void UpdateWidgetSize(const FVector2D& NewSize);
	FVector2D CalculateNewSize(const FVector2D& MouseDelta, EResizeHandle Handle) const;
	EMouseCursor::Type GetCursorForHandle(EResizeHandle Handle) const;
	void DrawCornerIndicators(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements, int32 LayerId) const;


	bool bIsResizing;
	EResizeHandle CurrentHandle;
	FVector2D DragStartMousePosition;
	FVector2D DragStartSize;
	FVector2D CurrentSize;
};
