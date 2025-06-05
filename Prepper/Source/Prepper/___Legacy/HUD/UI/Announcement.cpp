// Fill out your copyright notice in the Description page of Project Settings.


#include "Announcement.h"
#include "Components/TextBlock.h"

void UAnnouncement::Announce(const FString& Content) const
{
	AnnouncementText->SetText(FText::FromString(Content));
}

void UAnnouncement::SetInformation(const FString& Content) const
{
	InfoText->SetText(FText::FromString(Content));
}

void UAnnouncement::SetWarmupTime(int Minutes, int Seconds)
{

	FString CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	WarmupTime->SetText(FText::FromString(CountdownText));
}
