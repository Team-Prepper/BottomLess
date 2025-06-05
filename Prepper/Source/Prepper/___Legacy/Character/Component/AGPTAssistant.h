#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/IHttpRequest.h"
#include "AAIHelper.h"
#include "DetectedItemInfo.h"
#include "GPTSettings.h"
#include "AGPTAssistant.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PREPPER_API UGPTAssistant : public UActorComponent
{
	GENERATED_BODY()

public:    
	// 생성자
	UGPTAssistant();

protected:
	// 초기화
	virtual void BeginPlay() override;


private:
	// `AAIHelper` 컴포넌트에 대한 참조
	class UAIHelper* AIHelper;

	// 타이머 관리 변수
	FTimerHandle GPTRequestTimerHandle;

	// GPT API 요청 간격 (초 단위)
	float RequestInterval = 20.0f;

	// GPT API 요청 및 응답 처리 함수
	void RequestGPTAssistant();
	void OnGPTResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);


	// 아이템 정보 직렬화 함수
	FString SerializeDetectedItems(const TMap<AActor*, FDetectedItemInfo>& DetectedItems);

	// GPT API 설정
	const FString GPT_API_URL = TEXT("https://api.openai.com/v1/chat/completions");
	FString GPT_API_KEY;

	// 조력자 AI 메시지를 UI에 전달하는 함수 (추후 구현 필요)
	void HandleAIResponse(const FString& Message);

public:
	// 말풍선 위젯 관리
	FTimerHandle MessageTimerHandle;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI Helper")
	class APetCharacter* PetOwner;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI Helper")
	class USpeechBubbleWidget* SpeechBubbleInstance;
};