// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "IndexedListContainer.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnIndexedListItemSelected, int32, InItemIndex);

/**
 * 
 */
UCLASS(Abstract, Blueprintable)
class COMMONBASICWIDGETS_API UIndexedListContainer : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintAssignable)
	FOnIndexedListItemSelected OnIndexedListItemSelected;
	
	UFUNCTION(BlueprintCallable)
	void SelectItemAtIndex(int32 InIndex);

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category="IndexedListContainer")
	void SetElementsCount(int32 InCount);
};
