#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "InventoryInteractionUI.generated.h"

class UImage;
class UTextBlock;
class UButton;

UCLASS()
class PREPPER_API UInventoryInteractionUI : public UUserWidget
{
	GENERATED_BODY()
public:
	//meta = (BindWidget)
	
	FString ItemCode;
	
	UPROPERTY()
	UImage* ItemIcon;

	UPROPERTY()
	UTextBlock* ItemName;

	UPROPERTY()
	UTextBlock* ItemDescription;

	UPROPERTY()
	UButton* UseButton;

	UPROPERTY()
	UButton* DiscardButton;

	void InitializeWidget(const FString& InitItemCode);
};
