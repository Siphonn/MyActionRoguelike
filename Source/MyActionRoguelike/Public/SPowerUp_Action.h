// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerUpBase.h"
#include "SPowerUp_Action.generated.h"

class USAction;
/**
 * 
 */
UCLASS()
class MYACTIONROGUELIKE_API ASPowerUp_Action : public ASPowerUpBase
{
	GENERATED_BODY()

public:
	void Interact_Implementation(APawn* InstigatorPawn) override;

protected:
	UPROPERTY(EditAnywhere, Category="Power-Up")
	TSubclassOf<USAction> ActionToGrant;
};
