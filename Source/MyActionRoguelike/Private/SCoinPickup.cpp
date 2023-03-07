// Fill out your copyright notice in the Description page of Project Settings.


#include "SCoinPickup.h"
#include "SPlayerState.h"

ASCoinPickup::ASCoinPickup()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	// Disable collision, instead we use SphereComp to handle interaction queries
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetupAttachment(RootComponent);
	CreditAmount = 5;
}

void ASCoinPickup::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn)) { return; }

	if (ASPlayerState* PlayerState = InstigatorPawn->GetPlayerState<ASPlayerState>())
	{
		PlayerState->AddCredit(CreditAmount);
		HideAndCooldownPowerUp();
	}
}
