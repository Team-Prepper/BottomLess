#include "SpeechBubbleWidget.h"

void USpeechBubbleWidget::SetMessage(const FString& Message)
{
	if (MessageText)
	{
		MessageText->SetText(FText::FromString(Message));
	}
}