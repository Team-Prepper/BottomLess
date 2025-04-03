#include "GaugeBarComponent.h"
#include "GaugeBar.h"
#include "Components/ProgressBar.h"

void UGaugeBarComponent::SetGaugePercent(float Percent)
{
	if (GaugeBarWidget == nullptr)
	{
		GaugeBarWidget = Cast<UGaugeBar>(GetUserWidgetObject());
	}
	
	if (GaugeBarWidget && GaugeBarWidget->GaugeBar)
	{
		GaugeBarWidget->GaugeBar->SetPercent(Percent);
	}
}