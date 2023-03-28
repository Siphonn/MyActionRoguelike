// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "SSaveGame.h"


int32 ASPlayerState::GetCredits() const
{
	return Credits;
}

void ASPlayerState::AddCredit(int32 CreditAmount)
{
	if (!ensure(CreditAmount > 0.0f))
	{
		return;
	}

	Credits += CreditAmount;
	OnCreditsUpdated.Broadcast(this, Credits, CreditAmount);
}

bool ASPlayerState::RemoveCredits(int CreditAmount)
{
	if (!ensure(CreditAmount > 0.0f))
	{
		return false;
	}

	if (Credits < CreditAmount)
	{
		// Not enough credits available 
		return false;
	}

	Credits -= CreditAmount;
	OnCreditsUpdated.Broadcast(this, Credits, -CreditAmount);
	return true;
}

void ASPlayerState::SavePlayerState_Implementation(USSaveGame* SaveObject)
{
	if (SaveObject)
	{
		SaveObject->Credits = Credits;
	}
}

void ASPlayerState::LoadPlayerState_Implementation(USSaveGame* SaveObject)
{
	if (SaveObject)
	{
		//Credits = SaveObject->Credits;
		// Make sure to trigger credits updated event
		AddCredit(SaveObject->Credits);
	}
}

// void ASPlayerState::MulticastOnCreditsChanged_Implementation(int32 NewCredits, int32 Delta)
// {
// 	OnCreditsUpdated.Broadcast(this, NewCredits, Delta);
// }

void ASPlayerState::OnRep_Credits(int32 OldCredits)
{
	OnCreditsUpdated.Broadcast(this, Credits, Credits - OldCredits);
	//GEngine->AddOnScreenDebugMessage(-1, 0.0f, FColor::White, TEXT("OnRep_Credits function is run."));
}

void ASPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPlayerState, Credits);
}
