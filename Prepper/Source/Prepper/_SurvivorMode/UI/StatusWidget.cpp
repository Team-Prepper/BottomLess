// Fill out your copyright notice in the Description page of Project Settings.


#include "StatusWidget.h"

#include "Prepper/___Legacy/Character/Component/State.h"
#include "Components/ProgressBar.h"

void UStatusWidget::Update(const State& NewData)
{
	HungerBar->SetPercent(NewData.Hungry.GetRatio());
	ThirstBar->SetPercent(NewData.Thirsty.GetRatio());
	InfectionBar->SetPercent(NewData.Infected.GetRatio());
}

void UStatusWidget::Update(const UStatusComponent& NewData)
{
	HungerBar->SetPercent(NewData.GetHungryRatio());
	ThirstBar->SetPercent(NewData.GetThirstyRatio());
	InfectionBar->SetPercent(NewData.GetInfectedRatio());
}
