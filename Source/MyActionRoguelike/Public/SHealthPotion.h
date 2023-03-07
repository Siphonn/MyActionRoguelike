// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SPowerUpBase.h"
#include "SHealthPotion.generated.h"

class UStaticMeshComponent;

UCLASS()
class MYACTIONROGUELIKE_API ASHealthPotion : public ASPowerUpBase
{
	GENERATED_BODY()

public:
	ASHealthPotion();

	void Interact_Implementation(APawn* InstigatorPawn) override;

protected:
	UPROPERTY(VisibleAnywhere, Category="Components")
	UStaticMeshComponent* BottleMesh;
	UPROPERTY(EditAnywhere, Category="Health")
	int32 CreditCost;
};
