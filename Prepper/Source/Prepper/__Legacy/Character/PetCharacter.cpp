#include "PetCharacter.h"
#include "AIController.h"
#include "Component/AAIHelper.h"
#include "Component/AGPTAssistant.h"
#include "GameFramework/CharacterMovementComponent.h"

APetCharacter::APetCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bIsActive = false;

	// Movement 설정 확인
	UCharacterMovementComponent* MovementComp = GetCharacterMovement();
	if (MovementComp)
	{
		MovementComp->bOrientRotationToMovement = true;
		MovementComp->MaxWalkSpeed = MovementSpeed;
		MovementComp->bConstrainToPlane = true;
		MovementComp->bSnapToPlaneAtStart = true;
	}

	// AI Helper 컴포넌트 추가
	AIHelper = CreateDefaultSubobject<UAIHelper>(TEXT("AIHelper"));
	GPTAssistant = CreateDefaultSubobject<UGPTAssistant>(TEXT("GPTAssistant"));

	// Widget Component 생성 및 설정
    SpeechBubbleComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("SpeechBubble"));
    SpeechBubbleComponent->SetupAttachment(GetRootComponent());
    SpeechBubbleComponent->SetRelativeLocation(FVector(0, 0, 100)); // 펫 머리 위 위치 조정
    SpeechBubbleComponent->SetWidgetSpace(EWidgetSpace::Screen); // 항상 화면을 향하도록 설정
    SpeechBubbleComponent->SetDrawSize(FVector2D(200, 100));
    SpeechBubbleComponent->SetVisibility(false);

	// 생성자 내에서 WidgetClass 설정 (블루프린트 경로 확인 필요)
	static ConstructorHelpers::FClassFinder<UUserWidget> WidgetBPClass(TEXT("/Game/Blueprints/HUD/Character/WBP_SpeechBubble"));
	if (WidgetBPClass.Succeeded())
	{
		SpeechBubbleWidgetClass = WidgetBPClass.Class;
		SpeechBubbleComponent->SetWidgetClass(SpeechBubbleWidgetClass);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("WBP_SpeechBubble 블루프린트를 찾을 수 없습니다."));
	}
}

void APetCharacter::BeginPlay()
{
	Super::BeginPlay();
	AIController = Cast<AAIController>(GetController());

	// WidgetComponent가 제대로 초기화되었는지 확인
	if (SpeechBubbleWidgetClass)
	{
		SpeechBubbleComponent->SetWidgetClass(SpeechBubbleWidgetClass);
		SpeechBubbleComponent->SetVisibility(true); // 필요 시 표시
	}
}

void APetCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!bIsActive || !FollowTarget)
		return;

	FVector TargetLocation = FollowTarget->GetActorLocation();
	FVector CurrentLocation = GetActorLocation();
	float DistanceToTarget = FVector::Distance(CurrentLocation, TargetLocation);

	if (DistanceToTarget > FollowDistance)
	{
		// PlayerLocation 방향으로 Pet 이동
		FVector Direction = (TargetLocation - CurrentLocation).GetSafeNormal();
		FVector NewLocation = CurrentLocation + (Direction * 200.0f * DeltaTime); // 속도 조절

		// 새 위치를 설정
		SetActorLocation(NewLocation);
	}
}

