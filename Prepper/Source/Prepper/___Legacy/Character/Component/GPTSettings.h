#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "GPTSettings.generated.h"

UCLASS(Config=Engine, DefaultConfig, meta=(DisplayName="GPT Assistant Settings"))
class PREPPER_API UGPTSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	// 생성자에서 기본값 설정
	UGPTSettings()
	{
		CategoryName = TEXT("Game");
		SectionName = TEXT("GPT Assistant");
	}

	// Config 속성을 사용하여 ini 파일과 연동
	UPROPERTY(Config, EditAnywhere, Category = "API", meta = (DisplayName = "GPT API Key"))
	FString OpenAI_APIKey;
};