// Fill out your copyright notice in the Description page of Project Settings.


#include "SPlayerState.h"


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
		return false;
	}

	Credits -= CreditAmount;
	OnCreditsUpdated.Broadcast(this, Credits, -CreditAmount);
	return true;
}
