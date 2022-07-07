// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"
#include "SAttributeComponent.h"

// Sets default values
ASHealthPotion::ASHealthPotion()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BottleMesh = CreateDefaultSubobject<UStaticMeshComponent>("BottleMesh");
	RootComponent = BottleMesh;
}

void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	// ISGameplayInterface::Interact_Implementation(InstigatorPawn);

	USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(
		InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
	if (AttributeComp && !IsHidden())
	{
		if (AttributeComp->GetHealth() < 100.0f)

			AttributeComp->ApplyHealthChange(50.0f);

		SetActorHiddenInGame(true);

		GetWorldTimerManager().SetTimer(TimerHandle_ActivatePotion, this, &ASHealthPotion::ActivatePotion_Elapsed,
		                                10.0f);
	}
}

void ASHealthPotion::ActivatePotion_Elapsed()
{
	SetActorHiddenInGame(false);
	// Reenable once timer is up
}
