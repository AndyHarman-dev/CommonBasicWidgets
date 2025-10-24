// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/PanelWidget.h"
#include "StackWidget.generated.h"

class UOverlay;

namespace EEasingFunc
{
	enum Type : int;
}

UENUM(BlueprintType)
enum class EStackTransition : uint8
{
	None,
	Push,
	Pop
};

USTRUCT(BlueprintType)
struct FStackAnimationSettings
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float Duration = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	TEnumAsByte<EEasingFunc::Type> EasingFunction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	float ExponentForEasing = 2.0f;
};

/**
 * 
 */
UCLASS()
class COMMONBASICWIDGETS_API UStackWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UStackWidget(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	FStackAnimationSettings AnimationSettings;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Animation")
	bool bAnimateTransitions = true;

	UFUNCTION(BlueprintCallable, Category = "Stack")
	void PushWidget(UWidget* Widget);

	UFUNCTION(BlueprintCallable, Category = "Stack")
	UWidget* PopWidget();

	UFUNCTION(BlueprintCallable, Category = "Stack")
	UWidget* GetActiveWidget() const;

	UFUNCTION(BlueprintCallable, Category = "Stack")
	int32 GetStackSize() const;

	UFUNCTION(BlueprintCallable, Category = "Stack")
	void ClearStack();


#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif

private:
	UPROPERTY(meta=(BindWidget))
	UOverlay* RootOverlay;
	
	UPROPERTY(Transient)
	TArray<UWidget*> WidgetStack;

	UPROPERTY(Transient)
	UWidget* CurrentActiveWidget;

	struct FAnimationState
	{
		float ElapsedTime = 0.0f;
		EStackTransition TransitionType = EStackTransition::None;
		UWidget* IncomingWidget = nullptr;
		UWidget* OutgoingWidget = nullptr;
		bool bIsAnimating = false;
	};

	FAnimationState AnimState;

	FTSTicker::FDelegateHandle TickerHandle;

	void SetAnimationTicking(bool bTicking);
	void InterruptAnimation();
	bool Tick(float DeltaTime);
	bool IsAnimationTicking() const noexcept;
	void TickAnimation(float DeltaTime);
	void UpdateWidgetVisibility();
	void StartTransitionAnimation(EStackTransition TransitionType, UWidget* Incoming, UWidget* Outgoing);
	void ApplyAnimationToWidget(UWidget* Widget, float Alpha, bool bFadeIn);
	void AddWidgetToOverlay(UWidget* Widget);
	void RemoveWidgetFromOverlay(UWidget* Widget);
};