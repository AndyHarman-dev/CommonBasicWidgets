#include "ResizableWidget/ResizableWidget.h"
#include "MacroHelpers.h"
#include "SafeAccess.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/NamedSlot.h"
#include "Editor/WidgetCompilerLog.h"

#define LOG_CATEGORY_NAME LogResizableWidget
#include "EasyLog.h"

UNullWidget* UNullWidget::NullWidget()
{
    static TStrongObjectPtr NullWidget(NewObject<UNullWidget>());
    return NullWidget.Get();
}

TSharedRef<SWidget> UNullWidget::RebuildWidget()
{
    return SNew(STextBlock)
        .Text(FText::FromString(TEXT("Null Widget")));
}

int32 UResizableWidget::NativePaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
    const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
    const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
    LayerId = Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle,
                              bParentEnabled);

    if (CornerSettings.bShowCornerIndicators)
    {
        DrawCornerIndicators(AllottedGeometry, OutDrawElements, LayerId);
        ++LayerId;
    }

    return LayerId;
}

FReply UResizableWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (!ResizeSettings.bEnableResize || !InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
    {
        return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
    }

    const FVector2D LocalMousePosition = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
    CurrentHandle = GetHandleAtPosition(InGeometry, LocalMousePosition);
    

    if (CurrentHandle != EResizeHandle::None)
    {
        bIsResizing = true;
        DragStartMousePosition = InMouseEvent.GetScreenSpacePosition();
        
        if (CurrentSize.IsNearlyZero())
        {
            CurrentSize = InGeometry.GetLocalSize();
        }
        
        DragStartSize = CurrentSize;

        return FReply::Handled().CaptureMouse(TakeWidget());
    }

    return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UResizableWidget::NativeOnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (bIsResizing)
    {
        bIsResizing = false;
        CurrentHandle = EResizeHandle::None;

        return FReply::Handled().ReleaseMouseCapture();
    }

    return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UResizableWidget::NativeOnMouseMove(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
    if (bIsResizing && CurrentHandle != EResizeHandle::None)
    {
        const FVector2D CurrentMousePosition = InMouseEvent.GetScreenSpacePosition();
        const FVector2D MouseDelta = CurrentMousePosition - DragStartMousePosition;
        const FVector2D NewSize = CalculateNewSize(MouseDelta, CurrentHandle);

        UpdateWidgetSize(NewSize);

        return FReply::Handled();
    }

    return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

void UResizableWidget::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
    Super::NativeOnMouseLeave(InMouseEvent);

    if (!bIsResizing)
    {
        CurrentHandle = EResizeHandle::None;
    }
}

FCursorReply UResizableWidget::NativeOnCursorQuery(const FGeometry& InGeometry, const FPointerEvent& InCursorEvent)
{
    if (!ResizeSettings.bEnableResize)
    {
        return Super::NativeOnCursorQuery(InGeometry, InCursorEvent);
    }

    if (bIsResizing)
    {
        return FCursorReply::Cursor(GetCursorForHandle(CurrentHandle));
    }

    const FVector2D LocalMousePosition = InGeometry.AbsoluteToLocal(InCursorEvent.GetScreenSpacePosition());
    const EResizeHandle Handle = GetHandleAtPosition(InGeometry, LocalMousePosition);

    if (Handle != EResizeHandle::None)
    {
        return FCursorReply::Cursor(GetCursorForHandle(Handle));
    }

    return Super::NativeOnCursorQuery(InGeometry, InCursorEvent);
}

#if WITH_EDITOR
void UResizableWidget::ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const
{
    Super::ValidateCompiledDefaults(CompileLog);
    if (IsValid(Slot) && !Slot->IsA(UCanvasPanelSlot::StaticClass()))
    {
        CompileLog.Warning(FText::FromString(TEXT("ResizableWidget requires CanvasPanelSlot as a slot!")));
    }
}
#endif

UResizableWidget::UResizableWidget(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer),
    ContentSlot(nullptr),
    DragStartMousePosition(FVector2D::ZeroVector),
    DragStartSize(FVector2D::ZeroVector),
    CurrentSize(FVector2D::ZeroVector)
{
    ResizeSettings.HandleSize = 8.0f;
    ResizeSettings.MinSize = FVector2D(100.0f, 100.0f);
    ResizeSettings.MaxSize = FVector2D(2000.0f, 2000.0f);
    ResizeSettings.bEnableResize = true;
    bIsResizing = false;
    CurrentHandle = EResizeHandle::None;

    CornerSettings.bShowCornerIndicators = true;
    CornerSettings.CornerIndicatorColor = FLinearColor(1.0f, 1.0f, 1.0f, 0.8f);
    CornerSettings.CornerIndicatorThickness = 2.f;
    CornerSettings.CornerIndicatorLength = 12.f;
}

void UResizableWidget::SetContent(UWidget* Content)
{
    if (!VALIDATE_OBJECTS(ContentSlot, Content))
    {
        return;
    }

    ContentSlot->SetContent(Content);
}

UWidget* UResizableWidget::GetContent() const noexcept
{
    SAFE_ACCESS_RETURN(ContentSlot, UNullWidget::NullWidget());
    return ContentSlot->GetContent();
}

EResizeHandle UResizableWidget::GetHandleAtPosition(const FGeometry& InGeometry, const FVector2D& LocalMousePosition) const
{
    const FVector2D WidgetSize = InGeometry.GetLocalSize();
    const bool bLeftEdge = LocalMousePosition.X <= ResizeSettings.HandleSize;
    const bool bRightEdge = LocalMousePosition.X >= WidgetSize.X - ResizeSettings.HandleSize;
    const bool bTopEdge = LocalMousePosition.Y <= ResizeSettings.HandleSize;
    const bool bBottomEdge = LocalMousePosition.Y >= WidgetSize.Y - ResizeSettings.HandleSize;

    if (bTopEdge && bLeftEdge)
        return EResizeHandle::TopLeft;
    if (bTopEdge && bRightEdge)
        return EResizeHandle::TopRight;
    if (bBottomEdge && bLeftEdge)
        return EResizeHandle::BottomLeft;
    if (bBottomEdge && bRightEdge)
        return EResizeHandle::BottomRight;
    if (bTopEdge)
        return EResizeHandle::Top;
    if (bBottomEdge)
        return EResizeHandle::Bottom;
    if (bLeftEdge)
        return EResizeHandle::Left;
    if (bRightEdge)
        return EResizeHandle::Right;

    return EResizeHandle::None;
}

void UResizableWidget::UpdateWidgetSize(const FVector2D& NewSize)
{
    CurrentSize.X = FMath::Clamp(NewSize.X, ResizeSettings.MinSize.X, ResizeSettings.MaxSize.X);
    CurrentSize.Y = FMath::Clamp(NewSize.Y, ResizeSettings.MinSize.Y, ResizeSettings.MaxSize.Y);
    if (UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(Slot))
    {
        CanvasSlot->SetSize(CurrentSize);
    }
}

FVector2D UResizableWidget::CalculateNewSize(const FVector2D& MouseDelta, EResizeHandle Handle) const
{
    FVector2D NewSize = DragStartSize;

    switch (Handle)
    {
        case EResizeHandle::Right:
        case EResizeHandle::TopRight:
        case EResizeHandle::BottomRight:
            NewSize.X = DragStartSize.X + MouseDelta.X;
            break;

        case EResizeHandle::Left:
        case EResizeHandle::TopLeft:
        case EResizeHandle::BottomLeft:
            NewSize.X = DragStartSize.X - MouseDelta.X;
            break;

        default:
            break;
    }

    switch (Handle)
    {
        case EResizeHandle::Bottom:
        case EResizeHandle::BottomLeft:
        case EResizeHandle::BottomRight:
            NewSize.Y = DragStartSize.Y + MouseDelta.Y;
            break;

        case EResizeHandle::Top:
        case EResizeHandle::TopLeft:
        case EResizeHandle::TopRight:
            NewSize.Y = DragStartSize.Y - MouseDelta.Y;
            break;

        default:
            break;
    }

    return NewSize;
}

EMouseCursor::Type UResizableWidget::GetCursorForHandle(EResizeHandle Handle) const
{
    switch (Handle)
    {
        case EResizeHandle::TopLeft:
        case EResizeHandle::BottomRight:
            return EMouseCursor::ResizeSouthEast;

        case EResizeHandle::TopRight:
        case EResizeHandle::BottomLeft:
            return EMouseCursor::ResizeSouthWest;

        case EResizeHandle::Top:
        case EResizeHandle::Bottom:
            return EMouseCursor::ResizeUpDown;

        case EResizeHandle::Left:
        case EResizeHandle::Right:
            return EMouseCursor::ResizeLeftRight;

        default:
            return EMouseCursor::Default;
    }
}

void UResizableWidget::DrawCornerIndicators(const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements,
    int32 LayerId) const
{
    const FVector2D WidgetSize = AllottedGeometry.GetLocalSize();

    TArray<FVector2D> LinePoints;
    LinePoints.Reserve(2);

    auto DrawCornerLines = [&](const FVector2D& CornerPosition, bool bIsLeft, bool bIsTop)
    {
        const float HorizontalDirection = bIsLeft ? 1.0f : -1.0f;
        const float VerticalDirection = bIsTop ? 1.0f : -1.0f;

        LinePoints.Reset();
        LinePoints.Add(CornerPosition);
        LinePoints.Add(CornerPosition + FVector2D(HorizontalDirection * CornerSettings.CornerIndicatorLength, 0.0f));
        FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), LinePoints, ESlateDrawEffect::None, CornerSettings.CornerIndicatorColor, true, CornerSettings.CornerIndicatorThickness);

        LinePoints.Reset();
        LinePoints.Add(CornerPosition);
        LinePoints.Add(CornerPosition + FVector2D(0.0f, VerticalDirection * CornerSettings.CornerIndicatorLength));
        FSlateDrawElement::MakeLines(OutDrawElements, LayerId, AllottedGeometry.ToPaintGeometry(), LinePoints, ESlateDrawEffect::None, CornerSettings.CornerIndicatorColor, true, CornerSettings.CornerIndicatorThickness);
    };

    DrawCornerLines(FVector2D(0.0f, 0.0f), true, true);
    DrawCornerLines(FVector2D(WidgetSize.X, 0.0f), false, true);
    DrawCornerLines(FVector2D(0.0f, WidgetSize.Y), true, false);
    DrawCornerLines(FVector2D(WidgetSize.X, WidgetSize.Y), false, false);
}