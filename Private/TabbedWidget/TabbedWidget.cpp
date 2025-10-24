// Fill out your copyright notice in the Description page of Project Settings.


#include "TabbedWidget/TabbedWidget.h"

#include "Components/PanelWidget.h"
#include "Components/WidgetSwitcher.h"
#include "Editor/WidgetCompilerLog.h"
#include "Misc/UObjectToken.h"
#include "TabbedWidget/TabInterface.h"

DEFINE_LOG_CATEGORY_STATIC(LogTabbedWidget, Log, All);

void UTabbedWidget::AddTab(UUserWidget* TabHeaderWidget, UUserWidget* TabContentWidget)
{
	if (!TabHeaderWidget || !TabContentWidget)
	{
		UE_LOG(LogTabbedWidget, Warning, TEXT("Cannot add tab with null widgets"));
		return;
	}

	if (!IsValid(TabHeadersContainer) || !IsValid(ContentSwitcher))
	{
		UE_LOG(LogTabbedWidget, Error, TEXT("Container widgets not initialized"));
		return;
	}

	if (!TabHeaderWidget->GetClass()->ImplementsInterface(UTabInterface::StaticClass()))
	{
		UE_LOG(LogTabbedWidget, Error, TEXT("Tab header widget must implement ITabInterface"));
		return;
	}
	
	TabHeaders.Add(TabHeaderWidget);
	TabContents.Add(TabContentWidget);

	TabHeadersContainer->AddChild(TabHeaderWidget);
	ContentSwitcher->AddChild(TabContentWidget);

	RegisterTabHeader(TabHeaderWidget);

	if (ActiveTabIndex == -1)
	{
		SwitchToTab(0);
	}
}

void UTabbedWidget::RemoveTab(int32 TabIndex)
{
	if (TabIndex < 0 || TabIndex >= TabHeaders.Num())
	{
		UE_LOG(LogTabbedWidget, Warning, TEXT("Invalid tab index: %d"), TabIndex);
		return;
	}

	UUserWidget* HeaderToRemove = TabHeaders[TabIndex];
	UUserWidget* ContentToRemove = TabContents[TabIndex];

	TabHeadersContainer->RemoveChild(HeaderToRemove);
	ContentSwitcher->RemoveChild(ContentToRemove);

	TabHeaders.RemoveAt(TabIndex);
	TabContents.RemoveAt(TabIndex);

	UnregisterTabHeader(HeaderToRemove);

	if (TabHeaders.Num() == 0)
	{
		ActiveTabIndex = -1;
		return;
	}

	if (TabIndex <= ActiveTabIndex)
	{
		const int32 NewActiveIndex = FMath::Clamp(ActiveTabIndex - 1, 0, TabHeaders.Num() - 1);
		SwitchToTab(NewActiveIndex);
	}
}

void UTabbedWidget::SwitchToTab(int32 TabIndex)
{
	if (TabIndex < 0 || TabIndex >= TabHeaders.Num())
	{
		UE_LOG(LogTabbedWidget, Warning, TEXT("Invalid tab index: %d"), TabIndex);
		return;
	}

	if (!IsValid(ContentSwitcher))
	{
		return;
	}

	ContentSwitcher->SetActiveWidgetIndex(TabIndex);
	ActiveTabIndex = TabIndex;

	NotifyTabChanged(TabIndex);
}

int32 UTabbedWidget::GetActiveTabIndex() const
{
	return ActiveTabIndex;
}

int32 UTabbedWidget::GetTabCount() const
{
	return TabHeaders.Num();
}

void UTabbedWidget::ClearTabs()
{
	for (int32 i = 0; i < GetTabCount(); ++i)
	{
		RemoveTab(i);
	}
}

#if WITH_EDITOR
void UTabbedWidget::ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const
{
	Super::ValidateCompiledDefaults(CompileLog);
#if not UE_BUILD_SHIPPING
	if (!IsValid(TabHeadersContainer))
	{
		auto ResultText = FMessageLog("BlueprintLog").Warning()
		   ->AddToken(FTextToken::Create(FText::FromString(TEXT("Widget Warning: "))))
		   ->AddToken(FUObjectToken::Create(this))
		   ->AddToken(FTextToken::Create(FText::FromString(TEXT(" - TabHeadersContainer should be set for this widget to function properly!"))))
			->ToText();

		CompileLog.Warning(ResultText);
	}
#endif
}

const FText UTabbedWidget::GetPaletteCategory()
{
	return NSLOCTEXT("CommonBasicWidgets", "CommonBasicWidgets", "CommonBasicWidgets");
}
#endif

void UTabbedWidget::RegisterTabHeader(UUserWidget* TabHeaderWidget)
{
	if (!IsValid(TabHeaderWidget) || !TabHeaderWidget->Implements<UTabInterface>())
	{
		return;
	}
	
	OnClickedDelegate.BindDynamic(this, &UTabbedWidget::OnTabClicked);
	ITabInterface::Execute_BindOnClick(TabHeaderWidget, OnClickedDelegate);
}

void UTabbedWidget::UnregisterTabHeader(UUserWidget* TabHeaderWidget)
{
	if (!IsValid(TabHeaderWidget) || !TabHeaderWidget->Implements<UTabInterface>())
	{
		return;
	}

	ITabInterface::Execute_UnbindOnClick(TabHeaderWidget, OnClickedDelegate);
	OnClickedDelegate.Unbind();
}

void UTabbedWidget::NotifyTabChanged(int32 NewTabIndex)
{
	OnTabChanged.Broadcast(NewTabIndex);
}

void UTabbedWidget::OnTabClicked(TScriptInterface<ITabInterface> TabHeader)
{
	if (!IsValid(TabHeader.GetObject()))
	{
		UE_LOG(LogTabbedWidget, Warning, TEXT("Invalid tab header interface was given to event!"));
		return;
	}

	const auto AsWidget = CastChecked<UUserWidget>(TabHeader.GetObject());
	const auto Index = GetTabIndex(AsWidget);
	if (Index == INDEX_NONE)
	{
		UE_LOG(LogTabbedWidget, Warning, TEXT("Couldn't find widget in tab headers. Widget [%s]"), *GetNameSafe(AsWidget));
		return;
	}

	SwitchToTab(Index);
}

int32 UTabbedWidget::GetTabIndex(UUserWidget* TabWidget) const noexcept
{
	if (!IsValid(TabWidget))
	{
		return INDEX_NONE;
	}

	return TabHeaders.Find(TabWidget);
}
