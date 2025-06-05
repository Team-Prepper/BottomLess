#include "SunShine.h"
#include "Net/UnrealNetwork.h"
#include "Components/DirectionalLightComponent.h"

ASunShine::ASunShine()
{
	PrimaryActorTick.bCanEverTick = true;
	DayLength = 60.0f;

	DirectionalLightComponent = CreateDefaultSubobject<UDirectionalLightComponent>(TEXT("DirectionalLightComponent"));
	RootComponent = DirectionalLightComponent;
	SetReplicates(true);

	// Initialize the replicated direction
	LightDirection = DirectionalLightComponent->GetComponentRotation();
}

void ASunShine::BeginPlay()
{
	Super::BeginPlay();
}

void ASunShine::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
    
	if (HasAuthority()) // Only update on the server
	{
		UpdateLightRotation(DeltaTime);
	}
}

void ASunShine::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASunShine, LightDirection);
}

void ASunShine::UpdateLightRotation(float DeltaTime)
{
	if (DirectionalLightComponent)
	{
		FVector RotationAxis = FVector(1.0f, 0.5f, 0.0f).GetSafeNormal();

		// Set the rotation speed (radians per second)
		float RotationSpeed = 2.0f * PI / DayLength;

		// Calculate the rotation angle based on the rotation speed and DeltaTime
		float RotationAngle = RotationSpeed * DeltaTime;

		// Calculate the rotation quaternion
		FQuat RotationQuat = FQuat(RotationAxis, RotationAngle);

		// Apply the rotation to the light's direction vector
		FVector NewDirection = RotationQuat.RotateVector(DirectionalLightComponent->GetForwardVector());

		// Set the new direction of the directional light
		LightDirection = NewDirection.Rotation();
		DirectionalLightComponent->SetWorldRotation(LightDirection);
	}
}

void ASunShine::OnRep_LightDirection()
{
	if (DirectionalLightComponent)
	{
		DirectionalLightComponent->SetWorldRotation(LightDirection);
	}
}
