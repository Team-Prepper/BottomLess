#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Prepper/__Legacy/Interfaces/IDetectable.h"
#include "DetectedItemInfo.h"
#include "AAIHelper.generated.h"




class APlayerCharacter;

UCLASS()
class PREPPER_API UAIHelper : public UActorComponent
{
	GENERATED_BODY()

	


	
public:
	UAIHelper();

	UPROPERTY(EditAnywhere, Category = "Detection")
	float DetectionRadius = 1000.0f;

	// 감지 주기 (초 단위)
	UPROPERTY(EditAnywhere, Category = "Detection", meta = (ClampMin = "0.1", UIMin = "0.1"))
	float DetectionInterval = 1.0f;

	// 외부에서 DetectedItems에 접근할 수 있는 함수
	const TMap<AActor*, FDetectedItemInfo>& GetDetectedItems() const { return DetectedItems; }



protected:
	virtual void BeginPlay() override;
	// virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
private:
	void DetectNearbyItems();
    
	UPROPERTY()
	ACharacter* OwningCharacter;

	

	// Key: Actor 포인터, Value: 감지된 아이템 정보
	TMap<AActor*, FDetectedItemInfo> DetectedItems;

	void OnItemEnterRange(AActor* Item, IDetectable* DetectableItem, float Distance);
	void UpdateItemDistance(AActor* Item, float NewDistance);
	// 아이템이 범위를 벗어났을 때 호출
	void OnItemLeaveRange(AActor* Item);

	// 타이머 핸들
	FTimerHandle DetectionTimerHandle;
};
