#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SunShine.generated.h"

UCLASS()
class PREPPER_API ASunShine : public AActor
{
	GENERATED_BODY()
    
public:	
	// Sets default values for this actor's properties
	ASunShine();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Replicate properties
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	// Update the light rotation
	void UpdateLightRotation(float DeltaTime);

	// Function called when LightDirection is replicated
	UFUNCTION()
	void OnRep_LightDirection();

	// Directional light component
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Light", meta = (AllowPrivateAccess = "true"))
	class UDirectionalLightComponent* DirectionalLightComponent;

	// Direction of the light, replicated to clients
	UPROPERTY(ReplicatedUsing = OnRep_LightDirection)
	FRotator LightDirection;

	// Length of a day in seconds
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time", meta = (AllowPrivateAccess = "true"))
	float DayLength;
};
