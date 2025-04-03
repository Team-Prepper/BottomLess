#include "AGPTAssistant.h"
#include "AAIHelper.h"
#include "HttpModule.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonSerializer.h"
#include "TimerManager.h"
#include "Misc/ConfigCacheIni.h" // 설정 파일을 읽기 위한 헤더
#include "Misc/Paths.h" // 경로 관련 헤더
#include "GPTSettings.h"
#include "Prepper/__Legacy/Character/PetCharacter.h"
#include "Prepper/__Legacy/HUD/UI/SpeechBubbleWidget.h"

// 생성자
UGPTAssistant::UGPTAssistant()
{
    PrimaryComponentTick.bCanEverTick = true; // 필요 시 Tick 활성화
}

// 초기화
void UGPTAssistant::BeginPlay()
{
    Super::BeginPlay();

    // 소유자 액터에서 `UAIHelper` 컴포넌트 찾기
    AIHelper = GetOwner()->FindComponentByClass<UAIHelper>();
    if (!AIHelper)
    {
        UE_LOG(LogTemp, Error, TEXT("UGPTAssistant: UAIHelper 컴포넌트를 찾을 수 없습니다."));
        return;
    }

    // 설정 파일에서 API 키 읽기
    const UGPTSettings* Settings = GetDefault<UGPTSettings>();
    if (Settings)
    {
        GPT_API_KEY = Settings->OpenAI_APIKey;
        UE_LOG(LogTemp, Log, TEXT("API 키를 성공적으로 읽었습니다."));
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("설정 파일에서 'OpenAI_APIKey'를 읽을 수 없습니다."));
    }

    // 펫 클래스와 말풍선 위젯 컴포넌트 찾기
    PetOwner = Cast<APetCharacter>(GetOwner());
    if (PetOwner && PetOwner->SpeechBubbleComponent)
    {
        // Widget 클래스 설정
        PetOwner->SpeechBubbleComponent->SetWidgetClass(LoadClass<UUserWidget>(nullptr, TEXT("/Game/UI/WBP_SpeechBubble")));
    }

    // 타이머 설정: RequestGPTAssistant를 10초마다 호출
    GetWorld()->GetTimerManager().SetTimer(GPTRequestTimerHandle, this, &UGPTAssistant::RequestGPTAssistant, RequestInterval, true);
}


// 아이템 정보 직렬화
FString UGPTAssistant::SerializeDetectedItems(const TMap<AActor*, FDetectedItemInfo>& DetectedItems)
{
    TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject());

    TArray<TSharedPtr<FJsonValue>> ItemsArray;

    for (const auto& ItemPair : DetectedItems)
    {
        const AActor* ItemActor = ItemPair.Key;
        const FDetectedItemInfo& ItemInfo = ItemPair.Value;

        TSharedPtr<FJsonObject> ItemObject = MakeShareable(new FJsonObject());
        ItemObject->SetStringField(TEXT("ItemName"), ItemInfo.ItemName);
        ItemObject->SetNumberField(TEXT("Distance"), ItemInfo.Distance);

        ItemsArray.Add(MakeShareable(new FJsonValueObject(ItemObject)));
    }

    JsonObject->SetArrayField(TEXT("DetectedItems"), ItemsArray);

    FString OutputString;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&OutputString);
    FJsonSerializer::Serialize(JsonObject.ToSharedRef(), Writer);

    return OutputString;
}

// GPT API 요청 함수
void UGPTAssistant::RequestGPTAssistant()
{
    if (!AIHelper)
    {
        UE_LOG(LogTemp, Warning, TEXT("UGPTAssistant: UAIHelper 참조가 유효하지 않습니다."));
        return;
    }

    const TMap<AActor*, FDetectedItemInfo>& DetectedItems = AIHelper->GetDetectedItems();

    // 감지된 아이템이 없을 경우 요청하지 않음
    if (DetectedItems.Num() == 0)
    {
        UE_LOG(LogTemp, Log, TEXT("UGPTAssistant: 감지된 아이템이 없습니다."));
        return;
    }

    // 아이템 정보 직렬화
    FString SerializedItems = SerializeDetectedItems(DetectedItems);
    // 주변 정보 출력
    UE_LOG(LogTemp, Log, TEXT("주변 정보: %s"), *SerializedItems)

    // HTTP 요청 생성
    TSharedRef<IHttpRequest, ESPMode::ThreadSafe> HttpRequest = FHttpModule::Get().CreateRequest();
    HttpRequest->SetURL(GPT_API_URL);
    HttpRequest->SetVerb(TEXT("POST"));
    HttpRequest->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
    HttpRequest->SetHeader(TEXT("Authorization"), FString("Bearer ") + GPT_API_KEY);

    // 요청 본문 생성
    TSharedPtr<FJsonObject> RequestObject = MakeShareable(new FJsonObject());

    // 모델 설정
    RequestObject->SetStringField(TEXT("model"), TEXT("gpt-4o-mini"));

    // 메시지 설정
    TArray<TSharedPtr<FJsonValue>> Messages;
    TSharedPtr<FJsonObject> SystemMessage = MakeShareable(new FJsonObject());
    SystemMessage->SetStringField(TEXT("role"), TEXT("system"));
    SystemMessage->SetStringField(TEXT("content"), TEXT("너는 좀비 세상에서 살아가는 주인공의 조력자역할이야. 주변 아이템 정보를 확인하면 주인공에게 도움이 될만한 말을 건네줘. 단, 50자 이내의 한국어 대화체로 말해야해."));
    Messages.Add(MakeShareable(new FJsonValueObject(SystemMessage)));

    TSharedPtr<FJsonObject> UserMessage = MakeShareable(new FJsonObject());
    UserMessage->SetStringField(TEXT("role"), TEXT("user"));
    UserMessage->SetStringField(TEXT("content"), FString::Printf(TEXT("주위 정보 : %s"), *SerializedItems));
    Messages.Add(MakeShareable(new FJsonValueObject(UserMessage)));

    RequestObject->SetArrayField(TEXT("messages"), Messages);

    // JSON 직렬화
    FString RequestBody;
    TSharedRef<TJsonWriter<>> Writer = TJsonWriterFactory<>::Create(&RequestBody);
    FJsonSerializer::Serialize(RequestObject.ToSharedRef(), Writer);

    HttpRequest->SetContentAsString(RequestBody);

    // 응답 처리 바인딩
    HttpRequest->OnProcessRequestComplete().BindUObject(this, &UGPTAssistant::OnGPTResponseReceived);

    // 요청 전송
    HttpRequest->ProcessRequest();
}

// GPT API 응답 처리 함수
void UGPTAssistant::OnGPTResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
    if (!bWasSuccessful)
    {
        UE_LOG(LogTemp, Error, TEXT("UGPTAssistant: GPT API 요청 실패"));
        return;
    }

    if (Response->GetResponseCode() != 200)
    {
        UE_LOG(LogTemp, Error, TEXT("UGPTAssistant: GPT API 응답 오류: %d"), Response->GetResponseCode());
        return;
    }

    // 응답 JSON 파싱
    TSharedPtr<FJsonObject> JsonResponse;
    TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Response->GetContentAsString());

    if (FJsonSerializer::Deserialize(Reader, JsonResponse) && JsonResponse.IsValid())
    {
        const TArray<TSharedPtr<FJsonValue>>* Choices;
        if (JsonResponse->TryGetArrayField(TEXT("choices"), Choices))
        {
            if (Choices->Num() > 0)
            {
                const TSharedPtr<FJsonObject>* ChoiceObject;
                if ((*Choices)[0]->TryGetObject(ChoiceObject))
                {
                    const TSharedPtr<FJsonObject>* MessageObject;
                    if ((*ChoiceObject)->TryGetObjectField(TEXT("message"), MessageObject))
                    {
                        FString AssistantMessage;
                        if ((*MessageObject)->TryGetStringField(TEXT("content"), AssistantMessage))
                        {
                            // 조력자 AI 메시지 처리
                            HandleAIResponse(AssistantMessage);
                        }
                    }
                }
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("UGPTAssistant: GPT API 응답 JSON 파싱 실패"));
    }
}


void UGPTAssistant::HandleAIResponse(const FString& Message)
{
    // 일단 로그로 출력
    UE_LOG(LogTemp, Log, TEXT("조력자 AI: %s"), *Message);
    if (PetOwner && PetOwner->SpeechBubbleComponent)
    {
        // Widget 인스턴스 가져오기
        if (!SpeechBubbleInstance)
        {
            SpeechBubbleInstance = Cast<USpeechBubbleWidget>(PetOwner->SpeechBubbleComponent->GetUserWidgetObject());
        }

        if (SpeechBubbleInstance)
        {
            
            // 메시지 설정
            SpeechBubbleInstance->SetMessage(Message);
            
            // Widget 표시
            PetOwner->SpeechBubbleComponent->SetVisibility(true);

            // 3초 후 메시지 숨기기
            GetWorld()->GetTimerManager().ClearTimer(MessageTimerHandle);
            GetWorld()->GetTimerManager().SetTimer(MessageTimerHandle, [this]()
            {
                if (PetOwner && PetOwner->SpeechBubbleComponent)
                {
                    PetOwner->SpeechBubbleComponent->SetVisibility(false);
                }
            }, 3.0f, false);
        }
    }
}