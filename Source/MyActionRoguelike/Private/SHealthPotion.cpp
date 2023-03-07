// Fill out your copyright notice in the Description page of Project Settings.


#include "SHealthPotion.h"
#include "SAttributeComponent.h"
#include "SPlayerState.h"


ASHealthPotion::ASHealthPotion()
{
	BottleMesh = CreateDefaultSubobject<UStaticMeshComponent>("BottleMesh");
	// Disable collision, instead we use SphereComp to handle interaction queries
	BottleMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BottleMesh->SetupAttachment(RootComponent);
	CreditCost = 10;
}

void ASHealthPotion::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn)) { return; }

	USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(InstigatorPawn->GetComponentByClass(USAttributeComponent::StaticClass()));
	// Check if not already at max health
	if (ensure(AttributeComp) && !AttributeComp->IsFullHealth())
	{
		if (ASPlayerState* PlayerState = InstigatorPawn->GetPlayerState<ASPlayerState>())
		{
			// Check for credits
			/// only activate if healed successfully
			if (PlayerState->RemoveCredits(CreditCost) && AttributeComp->ApplyHealthChange(this, AttributeComp->GetMaxHealth()))
			{
				HideAndCooldownPowerUp();
			}
		}
	}
}
