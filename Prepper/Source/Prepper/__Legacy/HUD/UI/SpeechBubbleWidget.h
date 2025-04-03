#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "SpeechBubbleWidget.generated.h"

UCLASS()
class PREPPER_API USpeechBubbleWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* MessageText;

public:
	void SetMessage(const FString& Message);
};
