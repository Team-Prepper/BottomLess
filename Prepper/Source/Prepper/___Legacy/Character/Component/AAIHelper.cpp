#include "AAIHelper.h"
#include "Prepper/___Legacy/Character/PlayerCharacter.h"
#include "Prepper/___Legacy/Interfaces/IDetectable.h"

UAIHelper::UAIHelper()
	: OwningCharacter(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UAIHelper::BeginPlay()
{
	Super::BeginPlay();
    
	// 소유 플레이어 캐릭터 가져오기
	OwningCharacter = Cast<ACharacter>(GetOwner());

	// 타이머 설정
	if (DetectionInterval > 0.0f)
	{
		GetWorld()->GetTimerManager().SetTimer(
			DetectionTimerHandle,
			this,
			&UAIHelper::DetectNearbyItems,
			DetectionInterval,
			true // 반복 실행
		);
	}
}

void UAIHelper::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// 타이머 정리
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(DetectionTimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

/*void UAIHelper::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (OwningPlayer)
	{
		DetectNearbyItems();
	}
}*/

void UAIHelper::DetectNearbyItems()
{
	if (!OwningCharacter) return;
	// 현재 프레임에서 감지된 아이템들을 저장할 세트
	TSet<AActor*> CurrentlyDetectedItems;

	TArray<FOverlapResult> OverlapResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(OwningCharacter);

	FCollisionShape CollisionShape = FCollisionShape::MakeSphere(DetectionRadius);
    
	bool bHasOverlap = GetWorld()->OverlapMultiByChannel(
		OverlapResults,
		OwningCharacter->GetActorLocation(),
		FQuat::Identity,
		ECollisionChannel::ECC_Visibility,
		CollisionShape,
		QueryParams
	);

	if (bHasOverlap)
	{
		for (const FOverlapResult& Result : OverlapResults)
		{
			AActor* DetectedActor = Result.GetActor();
			if (DetectedActor && DetectedActor->Implements<UDetectable>())
			{
				IDetectable* DetectableObject = Cast<IDetectable>(DetectedActor);
				if (DetectableObject) // 감지된 객체가 아이템 & 좀비일 경우
				{
					CurrentlyDetectedItems.Add(DetectedActor);
					// 플레이어와 아이템 사이의 거리 계산
					float Distance = FVector::Distance(
						OwningCharacter->GetActorLocation(),
						DetectedActor->GetActorLocation()
					);

					// 새로 감지된 아이템인 경우
					if (!DetectedItems.Contains(DetectedActor))
					{
						OnItemEnterRange(DetectedActor, DetectableObject, Distance);
					}
					else
					{
						// 기존 아이템 거리 업데이트
						UpdateItemDistance(DetectedActor, Distance);
					}
				}
			}
		}
	}

	// 범위를 벗어난 아이템들 확인
	TArray<AActor*> ItemsToRemove;
	for (auto& Pair : DetectedItems)
	{
		if (!CurrentlyDetectedItems.Contains(Pair.Key))
		{
			ItemsToRemove.Add(Pair.Key);
		}
	}
	// 범위를 벗어난 아이템들 처리
	for (AActor* ItemToRemove : ItemsToRemove)
	{
		OnItemLeaveRange(ItemToRemove);
	}
}
void UAIHelper::OnItemEnterRange(AActor* Item, IDetectable* DetectableItem, float Distance)
{
	DetectedItems.Add(Item, FDetectedItemInfo(*DetectableItem->GetDetectableDescription(), Distance, GetWorld()->GetTimeSeconds()));
    
	// 아이템이 범위 내로 들어왔을 때의 처리
	UE_LOG(LogTemp, Log, TEXT("New Item Detected: %s (Distance: %.2f)"), 
		*DetectableItem->GetDetectableDescription(), Distance);
    
}
void UAIHelper::UpdateItemDistance(AActor* Item, float NewDistance)
{
	if (FDetectedItemInfo* ItemInfo = DetectedItems.Find(Item))
	{
		ItemInfo->Distance = NewDistance;
	}
}

void UAIHelper::OnItemLeaveRange(AActor* Item)
{
	DetectedItems.Remove(Item);
    
	// 아이템이 범위를 벗어났을 때의 처리
	UE_LOG(LogTemp, Log, TEXT("Item Left Detection Range: %s"), 
		*Item->GetName());
    
}