// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Announcement.generated.h"

/**
 * 
 */
class UTextBlock;

UCLASS()
class PREPPER_API UAnnouncement : public UUserWidget
{
	GENERATED_BODY()

public:
	
	UPROPERTY(meta = (BindWidget))
	UTextBlock* AnnouncementText;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* WarmupTime;
	UPROPERTY(meta = (BindWidget))
	UTextBlock* InfoText;
	
	void Announce(const FString& Content) const;
	void SetInformation(const FString& Content) const;

	void SetWarmupTime(int Minute, int Second);
};
