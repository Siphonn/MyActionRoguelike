// Fill out your copyright notice in the Description page of Project Settings.


#include "SInteractionComponent.h"
#include "DrawDebugHelpers.h"
#include "SGameplayInterface.h"


static TAutoConsoleVariable<bool> CVarDebugDrawInteraction(TEXT("su.InteractionDebugDraw"), false, TEXT("Enable Debug lines for Interaction Component"), ECVF_Cheat);


// Sets default values for this component's properties
USInteractionComponent::USInteractionComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

void USInteractionComponent::PrimaryInteract()
{
	bool bDebugDraw = CVarDebugDrawInteraction.GetValueOnGameThread();

	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECC_WorldDynamic);

	AActor* MyOwner = GetOwner();

	FVector EyeLocation;
	FRotator EyeRotation;
	MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);
	FVector End = EyeLocation + (EyeRotation.Vector() * 1000.0f);

	TArray<FHitResult> Hits;

	float Radius = 30.0f;

	FCollisionShape Shape;
	Shape.SetSphere(Radius);

	bool bBlockingHit = GetWorld()->SweepMultiByObjectType(Hits, EyeLocation, End, FQuat::Identity, ObjectQueryParams, Shape);
	FColor LineColour = bBlockingHit ? FColor::Green : FColor::Red;

	for (FHitResult Hit : Hits)
	{
		// DEBUG
		if (bDebugDraw)
		{
			DrawDebugSphere(GetWorld(), Hit.ImpactPoint, Radius, 32, LineColour, false, 2.0f);
		}

		AActor* HitActor = Hit.GetActor();
		if (HitActor)
		{
			if (HitActor->Implements<USGameplayInterface>())
			{
				APawn* MyPawn = Cast<APawn>(MyOwner);

				ISGameplayInterface::Execute_Interact(HitActor, MyPawn);
				break;
			}
		}
	}

	// DEBUG
	if (bDebugDraw)
	{
		DrawDebugLine(GetWorld(), EyeLocation, End, LineColour, false, 2.0f, 0, 2.0f);
	}
}

/// CODE EXAMPLES
/// Single LineTrace
// FHitResult Hit;
// bool bBlockingHit = GetWorld()->LineTraceSingleByObjectType(Hit, EyeLocation, End, ObjectQueryParams);
