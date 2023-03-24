// Fill out your copyright notice in the Description page of Project Settings.


#include "SCoinPickup.h"
#include "SPlayerState.h"

ASCoinPickup::ASCoinPickup()
{
	CreditAmount = 5;
}

void ASCoinPickup::Interact_Implementation(APawn* InstigatorPawn)
{
	if (!ensure(InstigatorPawn)) { return; }

	if (ASPlayerState* PlayerState = InstigatorPawn->GetPlayerState<ASPlayerState>())
	{
		PlayerState->AddCredit(CreditAmount);
		//HideAndCooldownPowerUp();
		Super::Interact_Implementation(InstigatorPawn);
	}
}
