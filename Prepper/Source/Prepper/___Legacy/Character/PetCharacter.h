#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Components/WidgetComponent.h"
#include "PetCharacter.generated.h"

UCLASS()
class PREPPER_API APetCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APetCharacter();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	UWidgetComponent* SpeechBubbleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> SpeechBubbleWidgetClass;

	virtual void Tick(float DeltaTime) override;

	void SetFollowTarget(AActor* Target) { FollowTarget = Target; }
	void SetActive(bool bNewActive) { bIsActive = bNewActive; }
	bool IsActive() const { return bIsActive; }

public:
	// AI 도우미 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI Helper")
	class UAIHelper* AIHelper;// AI 도우미 컴포넌트
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="AI Helper")
	class UGPTAssistant* GPTAssistant;

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	class AAIController* AIController;
	
	UPROPERTY()
	AActor* FollowTarget;

	UPROPERTY()
	bool bIsActive;

	UPROPERTY(EditAnywhere, Category = "Pet Properties")
	float FollowDistance = 200.0f;

	UPROPERTY(EditAnywhere, Category = "Pet Properties")
	float MovementSpeed = 300.0f;
};