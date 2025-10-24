// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TabInterface.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnTabHeaderClickedEvent, TScriptInterface<class ITabInterface>, TabHeader);

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable)
class UTabInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class COMMONBASICWIDGETS_API ITabInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent, Category = "Tab Header")
	void BindOnClick(const FOnTabHeaderClickedEvent& Event);
	UFUNCTION(BlueprintNativeEvent, Category = "Tab Header")
	void UnbindOnClick(const FOnTabHeaderClickedEvent& Event);
};
