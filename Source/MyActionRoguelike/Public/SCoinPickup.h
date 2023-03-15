// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerUpBase.h"
#include "SCoinPickup.generated.h"

/**
 * 
 */
UCLASS()
class MYACTIONROGUELIKE_API ASCoinPickup : public ASPowerUpBase
{
	GENERATED_BODY()

public:
	ASCoinPickup();

	void Interact_Implementation(APawn* InstigatorPawn) override;

protected:
	UPROPERTY(EditAnywhere, Category="Credits")
	int32 CreditAmount;
};
