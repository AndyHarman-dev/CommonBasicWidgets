// Fill out your copyright notice in the Description page of Project Settings.


#include "IndexedListContainer/UMG/IndexedListContainer.h"

void UIndexedListContainer::SelectItemAtIndex(int32 InIndex)
{
	OnIndexedListItemSelected.Broadcast(InIndex);
}
