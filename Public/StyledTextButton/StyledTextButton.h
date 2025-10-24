// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "StyledTextButton.generated.h"

class UTextBlock;
class UButton;

USTRUCT(BlueprintType)
struct FTextAppearance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateFontInfo Font;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FSlateColor Color;

	FTextAppearance()
		: Color(FLinearColor::White)
	{
	}

	FTextAppearance(const FSlateColor& InColor)
		: Color(InColor)
	{
	}
};

USTRUCT(BlueprintType)
struct FTextButtonStyle
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FButtonStyle ButtonStyle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTextAppearance NormalText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTextAppearance HoveredText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTextAppearance PressedText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FTextAppearance DisabledText;

	FTextButtonStyle()
		: NormalText(FLinearColor::White)
		, HoveredText(FLinearColor::Yellow)
		, PressedText(FLinearColor::Gray)
		, DisabledText(FLinearColor(0.5f, 0.5f, 0.5f, 0.5f))
	{
	}
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnStyledButtonEvent);

/**
 * 
 */
UCLASS()
class COMMONBASICWIDGETS_API UStyledTextButton : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UButton* MainButton;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* ButtonText;

	virtual void NativePreConstruct() override;
	virtual void NativeConstruct() override;

public:
	UPROPERTY(BlueprintAssignable, Category = "Styled Button|Events")
	FOnStyledButtonEvent OnClicked;

	UPROPERTY(BlueprintAssignable, Category = "Styled Button|Events")
	FOnStyledButtonEvent OnHovered;

	UPROPERTY(BlueprintAssignable, Category = "Styled Button|Events")
	FOnStyledButtonEvent OnUnhovered;

	UPROPERTY(BlueprintAssignable, Category = "Styled Button|Events")
	FOnStyledButtonEvent OnPressed;

	UPROPERTY(BlueprintAssignable, Category = "Styled Button|Events")
	FOnStyledButtonEvent OnReleased;

	UFUNCTION(BlueprintCallable, Category = "Styled Button")
	virtual void SetButtonText(const FText& InText);

	UFUNCTION(BlueprintCallable, Category = "Styled Button")
	void SetButtonStyle(const FTextButtonStyle& InStyle);
	
	UFUNCTION(BlueprintPure, Category = "Styled Button")
	FText GetButtonText();

	virtual void SetIsEnabled(bool bInIsEnabled) override;
#if WITH_EDITOR
	virtual const FText GetPaletteCategory() override;
#endif
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Category = "Styled Button")
	FText Text;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Category = "Styled Button")
	FTextButtonStyle Style;

	UFUNCTION()
	virtual void HandleButtonClicked();

	UFUNCTION()
	virtual void HandleButtonHovered();

	UFUNCTION()
	virtual void HandleButtonUnhovered();

	UFUNCTION()
	virtual void HandleButtonPressed();

	UFUNCTION()
	virtual void HandleButtonReleased();

	void ApplyTextAppearance(const FTextAppearance& Appearance);
	void RefreshButtonStyle();
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnStyledButtonTextChanged, const FText&, NewText);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnStyledButtonTextCommitted, const FText&, NewText, ETextCommit::Type, CommitMethod);


UCLASS()
class COMMONBASICWIDGETS_API UStyledEditableTextButton : public UStyledTextButton
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category = "Styled Button|Events")
	FOnStyledButtonTextChanged OnTextChanged;
	UPROPERTY(BlueprintAssignable, Category = "Styled Button|Events")
	FOnStyledButtonTextCommitted OnTextCommitted;

	virtual void SetButtonText(const FText& InText) override;
	
	virtual void SetIsEnabled(bool bInIsEnabled) override;
	
	UFUNCTION(BlueprintCallable, Category = "Styled Button")
	void SetTextEditable(bool bInEditable);
	UFUNCTION(BlueprintCallable, Category = "Styled Button")
	bool IsTextEditable() const { return bIsTextEditable; }

	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
protected:
	virtual void HandleButtonClicked() override;
	virtual void HandleButtonHovered() override;
	virtual void HandleButtonUnhovered() override;
	virtual void HandleButtonPressed() override;
	virtual void HandleButtonReleased() override;

	virtual FReply NativeOnKeyChar(const FGeometry& InGeometry, const FCharacterEvent& InCharEvent) override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;
	virtual FReply NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent) override;
	virtual void NativeOnFocusLost(const FFocusEvent& InFocusEvent) override;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Category = "Styled Button")
	bool bIsTextEditable = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess), Category = "Styled Button|Editing")
	float CaretBlinkInterval = 0.5f;

	FString EditableString;
	int32 CaretPosition = 0;
	bool bShowCaret = false;
	float CaretBlinkTimer = 0.f;
	bool bIsEditing = false;
	
	void StartEditing();
	void StopEditing(ETextCommit::Type CommitMethod);
	void InsertCharacterAtCaret(TCHAR Character);
	void DeleteCharacterBeforeCaret();
	void MoveCaret(int32 Delta);
	void UpdateDisplayText();
	FText GetDisplayTextWithCaret() const;
};
