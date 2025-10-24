// Fill out your copyright notice in the Description page of Project Settings.

#include "StackWidget/StackWidget.h"

#include "Blueprint/WidgetTree.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Kismet/KismetMathLibrary.h"

#define LOCTEXT_NAMESPACE "CommonBasicWidgets"

UStackWidget::UStackWidget(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , CurrentActiveWidget(nullptr)
{
    bIsVariable = true;
    SetVisibility(ESlateVisibility::SelfHitTestInvisible);
}

#if WITH_EDITOR
const FText UStackWidget::GetPaletteCategory()
{
    return LOCTEXT("CommonBasicWidgets", "CommonBasicWidgets");
}

#endif

void UStackWidget::PushWidget(UWidget* Widget)
{
    if (!IsValid(Widget))
    {
        return;
    }

    if (IsAnimationTicking())
    {
        InterruptAnimation();
    }

    UWidget* PreviousWidget = CurrentActiveWidget;
    WidgetStack.Add(Widget);
    CurrentActiveWidget = Widget;

    AddWidgetToOverlay(Widget);

    if (bAnimateTransitions)
    {
        StartTransitionAnimation(EStackTransition::Push, Widget, PreviousWidget);
    }
    else
    {
        UpdateWidgetVisibility();
    }
}

UWidget* UStackWidget::PopWidget()
{
    if (WidgetStack.Num() == 0)
    {
        return nullptr;
    }

    if (IsAnimationTicking())
    {
        InterruptAnimation();
    }

    UWidget* PoppedWidget = WidgetStack.Pop();
    UWidget* NewActiveWidget = WidgetStack.Num() > 0 ? WidgetStack.Last() : nullptr;

    CurrentActiveWidget = NewActiveWidget;

    if (bAnimateTransitions)
    {
        StartTransitionAnimation(EStackTransition::Pop, NewActiveWidget, PoppedWidget);
    }
    else
    {
        if (IsValid(PoppedWidget))
        {
            RemoveWidgetFromOverlay(PoppedWidget);
        }
        UpdateWidgetVisibility();
    }

    return PoppedWidget;
}

UWidget* UStackWidget::GetActiveWidget() const
{
    return CurrentActiveWidget;
}

int32 UStackWidget::GetStackSize() const
{
    return WidgetStack.Num();
}

void UStackWidget::ClearStack()
{
    AnimState.bIsAnimating = false;
    
    for (UWidget* Widget : WidgetStack)
    {
        if (Widget)
        {
            RemoveWidgetFromOverlay(Widget);
        }
    }

    WidgetStack.Empty();
    CurrentActiveWidget = nullptr;
}

void UStackWidget::UpdateWidgetVisibility()
{
    for (UWidget* Widget : WidgetStack)
    {
        if (IsValid(Widget))
        {
            Widget->SetVisibility(Widget == CurrentActiveWidget ? ESlateVisibility::SelfHitTestInvisible : ESlateVisibility::Collapsed);
            Widget->SetRenderOpacity(1.0f);
        }
    }
}

void UStackWidget::StartTransitionAnimation(EStackTransition TransitionType, UWidget* Incoming, UWidget* Outgoing)
{
    AnimState.ElapsedTime = 0.0f;
    AnimState.TransitionType = TransitionType;
    AnimState.IncomingWidget = Incoming;
    AnimState.OutgoingWidget = Outgoing;
    AnimState.bIsAnimating = true;

    if (IsValid(Incoming))
    {
        Incoming->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
    }
    if (TransitionType == EStackTransition::Push)
    {
        Incoming->SetRenderOpacity(0.f);
    }

    SetAnimationTicking(true);
}

void UStackWidget::SetAnimationTicking(bool bTicking)
{
    if (bTicking)
    {
        TickerHandle = FTSTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateUObject(this, &UStackWidget::Tick), 0.0f);
    }
    else
    {
        FTSTicker::GetCoreTicker().RemoveTicker(TickerHandle);
    }
}

void UStackWidget::InterruptAnimation()
{
    AnimState.bIsAnimating = false;
            
    if (AnimState.TransitionType == EStackTransition::Pop && AnimState.OutgoingWidget)
    {
        RemoveWidgetFromOverlay(AnimState.OutgoingWidget);
    }
            
    UpdateWidgetVisibility();
            
    SetAnimationTicking(false);
}

bool UStackWidget::Tick(float DeltaTime)
{
    if (!AnimState.bIsAnimating)
    {
        SetAnimationTicking(false);
        return false;
    }

    TickAnimation(DeltaTime);

    if (AnimState.ElapsedTime >= AnimationSettings.Duration)
    {
        InterruptAnimation();
        return false;
    }

    return true;
}

bool UStackWidget::IsAnimationTicking() const noexcept
{
    if (auto Element = TickerHandle.Pin())
    {
        return Element->State != Element->RemovedState;
    }

    return false;
}

void UStackWidget::TickAnimation(float DeltaTime)
{
    AnimState.ElapsedTime += DeltaTime;
    const float Alpha = FMath::Clamp(AnimState.ElapsedTime / AnimationSettings.Duration, 0.0f, 1.0f);
    const float EasedAlpha = UKismetMathLibrary::Ease(0.f, 1.f, Alpha, AnimationSettings.EasingFunction, AnimationSettings.ExponentForEasing);

    if (AnimState.IncomingWidget)
    {
        ApplyAnimationToWidget(AnimState.IncomingWidget, EasedAlpha, true);
    }

    if (AnimState.OutgoingWidget)
    {
        ApplyAnimationToWidget(AnimState.OutgoingWidget, EasedAlpha, false);
    }
}

void UStackWidget::ApplyAnimationToWidget(UWidget* Widget, float Alpha, bool bFadeIn)
{
    if (!IsValid(Widget))
    {
        return;
    }

    const float TargetOpacity = bFadeIn ? Alpha : 1.0f - Alpha;
    Widget->SetRenderOpacity(TargetOpacity);
}

void UStackWidget::AddWidgetToOverlay(UWidget* Widget)
{
    if (IsValid(RootOverlay))
    {
        const auto Slot = RootOverlay->AddChildToOverlay(Widget);
        Slot->SetHorizontalAlignment(HAlign_Fill);
        Slot->SetVerticalAlignment(VAlign_Fill);
    }
}

void UStackWidget::RemoveWidgetFromOverlay(UWidget* Widget)
{
    if (IsValid(RootOverlay))
    {
        RootOverlay->RemoveChild(Widget);
    }
}

#undef LOCTEXT_NAMESPACE
