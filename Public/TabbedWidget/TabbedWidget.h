// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TabInterface.h"
#include "Blueprint/UserWidget.h"
#include "TabbedWidget.generated.h"

class UWidgetSwitcher;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTabChanged, int32, TabIndex);

/**
 * 
 */
UCLASS()
class COMMONBASICWIDGETS_API UTabbedWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable, Category = "Tab Events")
	FOnTabChanged OnTabChanged;

	UFUNCTION(BlueprintCallable, Category = "Tabs")
	void AddTab(UUserWidget* TabHeaderWidget, UUserWidget* TabContentWidget);

	UFUNCTION(BlueprintCallable, Category = "Tabs")
	void RemoveTab(int32 TabIndex);

	UFUNCTION(BlueprintCallable, Category = "Tabs")
	void SwitchToTab(int32 TabIndex);

	UFUNCTION(BlueprintCallable, Category = "Tabs")
	int32 GetActiveTabIndex() const;

	UFUNCTION(BlueprintCallable, Category = "Tabs")
	int32 GetTabCount() const;

	UFUNCTION(BLueprintCallable, Category = "Tabs")
	void ClearTabs();

#if WITH_EDITOR
	virtual void ValidateCompiledDefaults(class IWidgetCompilerLog& CompileLog) const override;
	virtual const FText GetPaletteCategory() override;
#endif
private:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	UPanelWidget* TabHeadersContainer;
	UPROPERTY(EditDefaultsOnly, meta = (BindWidget, AllowPrivateAccess))
	UWidgetSwitcher* ContentSwitcher;
	
	UPROPERTY()
	TArray<UUserWidget*> TabHeaders;
	UPROPERTY()
	TArray<UUserWidget*> TabContents;

	FOnTabHeaderClickedEvent OnClickedDelegate;

	int32 ActiveTabIndex = -1;
	
	void RegisterTabHeader(UUserWidget* TabHeaderWidget);
	void UnregisterTabHeader(UUserWidget* TabHeaderWidget);
	void NotifyTabChanged(int32 NewTabIndex);

	UFUNCTION()
	void OnTabClicked(TScriptInterface<ITabInterface> TabHeader);
	int32 GetTabIndex(UUserWidget* TabWidget) const noexcept;
};
