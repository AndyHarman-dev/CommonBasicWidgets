// Fill out your copyright notice in the Description page of Project Settings.


#include "StyledTextButton/StyledTextButton.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Framework/Application/SlateApplication.h"

void UStyledTextButton::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (IsValid(ButtonText))
	{
		ButtonText->SetText(Text);
	}

	RefreshButtonStyle();
}

void UStyledTextButton::NativeConstruct()
{
	Super::NativeConstruct();

	if (IsValid(MainButton))
	{
		MainButton->OnClicked.AddDynamic(this, &UStyledTextButton::HandleButtonClicked);
		MainButton->OnHovered.AddDynamic(this, &UStyledTextButton::HandleButtonHovered);
		MainButton->OnUnhovered.AddDynamic(this, &UStyledTextButton::HandleButtonUnhovered);
		MainButton->OnPressed.AddDynamic(this, &UStyledTextButton::HandleButtonPressed);
		MainButton->OnReleased.AddDynamic(this, &UStyledTextButton::HandleButtonReleased);
	}

	ApplyTextAppearance(Style.NormalText);
}

void UStyledTextButton::SetButtonText(const FText& InText)
{
	Text = InText;
	
	if (IsValid(ButtonText))
	{
		ButtonText->SetText(Text);
	}
}

void UStyledTextButton::SetButtonStyle(const FTextButtonStyle& InStyle)
{
	Style = InStyle;
	RefreshButtonStyle();
}

FText UStyledTextButton::GetButtonText()
{
	if (IsValid(ButtonText))
	{
		return ButtonText->GetText();
	}

	return Text;
}

void UStyledTextButton::SetIsEnabled(bool bInIsEnabled)
{
	Super::SetIsEnabled(bInIsEnabled);

	if (IsValid(MainButton))
	{
		MainButton->SetIsEnabled(bInIsEnabled);

		if (bInIsEnabled)
		{
			ApplyTextAppearance(Style.NormalText);
		}
		else
		{
			ApplyTextAppearance(Style.DisabledText);
		}
	}
}

const FText UStyledTextButton::GetPaletteCategory()
{
	return NSLOCTEXT("CommonBasicWidgets", "CommonBasicWidgets", "CommonBasicWidgets");
}

void UStyledTextButton::HandleButtonClicked()
{
	OnClicked.Broadcast();
}

void UStyledTextButton::HandleButtonHovered()
{
	OnHovered.Broadcast();
}

void UStyledTextButton::HandleButtonUnhovered()
{
	if (MainButton && MainButton->IsPressed())
	{
		return;
	}
	
	OnUnhovered.Broadcast();
}

void UStyledTextButton::HandleButtonPressed()
{
	OnPressed.Broadcast();
}

void UStyledTextButton::HandleButtonReleased()
{
	if (MainButton && MainButton->IsHovered())
	{
		ApplyTextAppearance(Style.HoveredText);
	}
	else
	{
		ApplyTextAppearance(Style.NormalText);
	}

	OnReleased.Broadcast();
}

void UStyledTextButton::ApplyTextAppearance(const FTextAppearance& Appearance)
{
	if (IsValid(ButtonText))
	{
		if (Appearance.Font.Size > 0)
		{
			ButtonText->SetFont(Appearance.Font);
		}

		ButtonText->SetColorAndOpacity(Appearance.Color);
	}
}

void UStyledTextButton::RefreshButtonStyle()
{
	if (IsValid(MainButton))
	{
		MainButton->SetStyle(Style.ButtonStyle);
	}

	if (IsValid(ButtonText))
	{
		ApplyTextAppearance(Style.NormalText);
	}
}

void UStyledEditableTextButton::SetButtonText(const FText& InText)
{
	Super::SetButtonText(InText);
	EditableString = InText.ToString();
}

void UStyledEditableTextButton::SetIsEnabled(bool bInIsEnabled)
{
	Super::SetIsEnabled(bInIsEnabled);
	if (bIsEditing)
	{
		StopEditing(ETextCommit::OnUserMovedFocus);
	}
}

void UStyledEditableTextButton::SetTextEditable(bool bInEditable)
{
	bIsTextEditable = bInEditable;
	
	if (!bIsTextEditable && bIsEditing)
	{
		StopEditing(ETextCommit::OnUserMovedFocus);
	}
}

void UStyledEditableTextButton::NativeConstruct()
{
	Super::NativeConstruct();
	EditableString = Text.ToString();
	SetIsFocusable(true);
}

void UStyledEditableTextButton::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	if (bIsEditing)
	{
		CaretBlinkTimer += InDeltaTime;
		if (CaretBlinkTimer >= CaretBlinkInterval)
		{
			bShowCaret = !bShowCaret;
			CaretBlinkTimer = 0.f;
			UpdateDisplayText();
		}
	}
}

void UStyledEditableTextButton::HandleButtonClicked()
{
	if (bIsTextEditable && !bIsEditing)
	{
		FSlateApplication::Get().SetKeyboardFocus(TakeWidget());
		StartEditing();
	}
	else if (!bIsTextEditable)
	{
		Super::HandleButtonClicked();
	}
}

void UStyledEditableTextButton::HandleButtonHovered()
{
	if (!bIsEditing)
	{
		ApplyTextAppearance(Style.HoveredText);
	}
	
	Super::HandleButtonHovered();
}

void UStyledEditableTextButton::HandleButtonUnhovered()
{
	if (!bIsEditing)
	{
		ApplyTextAppearance(Style.NormalText);
	}
	
	Super::HandleButtonUnhovered();
}

void UStyledEditableTextButton::HandleButtonPressed()
{
	if (!bIsEditing)
	{
		ApplyTextAppearance(Style.PressedText);
	}
	
	Super::HandleButtonPressed();
}

void UStyledEditableTextButton::HandleButtonReleased()
{
	if (bIsEditing)
	{
		return;
	}
	
	Super::HandleButtonReleased();
}

FReply UStyledEditableTextButton::NativeOnKeyChar(const FGeometry& InGeometry, const FCharacterEvent& InCharEvent)
{
	if (bIsEditing && bIsTextEditable)
	{
		const TCHAR Character = InCharEvent.GetCharacter();
		
		if (Character >= 32 && Character != 127)
		{
			InsertCharacterAtCaret(Character);
			return FReply::Handled();
		}
	}

	return Super::NativeOnKeyChar(InGeometry, InCharEvent);
}

FReply UStyledEditableTextButton::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (bIsEditing && bIsTextEditable)
	{
		const FKey Key = InKeyEvent.GetKey();

		if (Key == EKeys::BackSpace)
		{
			DeleteCharacterBeforeCaret();
			return FReply::Handled();
		}
		else if (Key == EKeys::Delete)
		{
			if (CaretPosition < EditableString.Len())
			{
				EditableString.RemoveAt(CaretPosition, 1);
				UpdateDisplayText();
				OnTextChanged.Broadcast(FText::FromString(EditableString));
			}
			return FReply::Handled();
		}
		else if (Key == EKeys::Left)
		{
			MoveCaret(-1);
			return FReply::Handled();
		}
		else if (Key == EKeys::Right)
		{
			MoveCaret(1);
			return FReply::Handled();
		}
		else if (Key == EKeys::Home)
		{
			CaretPosition = 0;
			UpdateDisplayText();
			return FReply::Handled();
		}
		else if (Key == EKeys::End)
		{
			CaretPosition = EditableString.Len();
			UpdateDisplayText();
			return FReply::Handled();
		}
		else if (Key == EKeys::Enter)
		{
			StopEditing(ETextCommit::OnEnter);
			return FReply::Handled();
		}
		else if (Key == EKeys::Escape)
		{
			EditableString = Text.ToString();
			StopEditing(ETextCommit::OnUserMovedFocus);
			return FReply::Handled();
		}
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

FReply UStyledEditableTextButton::NativeOnFocusReceived(const FGeometry& InGeometry, const FFocusEvent& InFocusEvent)
{
	if (bIsTextEditable && !bIsEditing)
	{
		StartEditing();
	}

	return Super::NativeOnFocusReceived(InGeometry, InFocusEvent);
}

void UStyledEditableTextButton::NativeOnFocusLost(const FFocusEvent& InFocusEvent)
{
	Super::NativeOnFocusLost(InFocusEvent);

	if (bIsEditing)
	{
		StopEditing(ETextCommit::OnUserMovedFocus);
	}
}

void UStyledEditableTextButton::StartEditing()
{
	bIsEditing = true;
	CaretPosition = EditableString.Len();
	bShowCaret = true;
	CaretBlinkTimer = 0.f;
	UpdateDisplayText();
	SetUserFocus(GetOwningPlayer());
}

void UStyledEditableTextButton::StopEditing(ETextCommit::Type CommitMethod)
{
	bIsEditing = false;
	bShowCaret = false;
	
	const FText NewText = FText::FromString(EditableString);
	Text = NewText;
	
	if (IsValid(ButtonText))
	{
		ButtonText->SetText(NewText);
	}

	OnTextCommitted.Broadcast(NewText, CommitMethod);
}

void UStyledEditableTextButton::InsertCharacterAtCaret(TCHAR Character)
{
	EditableString.InsertAt(CaretPosition, Character);
	CaretPosition++;
	bShowCaret = true;
	CaretBlinkTimer = 0.f;
	UpdateDisplayText();
	OnTextChanged.Broadcast(FText::FromString(EditableString));
}

void UStyledEditableTextButton::DeleteCharacterBeforeCaret()
{
	if (CaretPosition > 0)
	{
		EditableString.RemoveAt(CaretPosition - 1, 1);
		CaretPosition--;
		bShowCaret = true;
		CaretBlinkTimer = 0.f;
		UpdateDisplayText();
		OnTextChanged.Broadcast(FText::FromString(EditableString));
	}
}

void UStyledEditableTextButton::MoveCaret(int32 Delta)
{
	CaretPosition = FMath::Clamp(CaretPosition + Delta, 0, EditableString.Len());
	bShowCaret = true;
	CaretBlinkTimer = 0.f;
	UpdateDisplayText();
}

void UStyledEditableTextButton::UpdateDisplayText()
{
	if (IsValid(ButtonText))
	{
		ButtonText->SetText(GetDisplayTextWithCaret());
	}
}

FText UStyledEditableTextButton::GetDisplayTextWithCaret() const
{
	if (bIsEditing && bShowCaret)
	{
		FString DisplayString = EditableString;
		DisplayString.InsertAt(CaretPosition, TEXT("|"));
		return FText::FromString(DisplayString);
	}

	return FText::FromString(EditableString);
}
