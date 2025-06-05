#pragma once
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Components/Widget.h"
#include "Prepper/__Base/ObserverPattern/Observer.h"
#include "Prepper/__Base/Util/GaugeValue.h"
#include "HPUI.generated.h"

UCLASS()
class UHPUI : public UWidget, public IObserver<GaugeValue<float>>
{
	GENERATED_BODY()
private:
	UPROPERTY(meta = (BindWidget))
	UProgressBar* HealthBar;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* HealthText;
	
public:
	virtual void Update(const GaugeValue<float>& NewData) override;
};
