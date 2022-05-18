// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SProjectile.h"
#include "SMagicProjectile.generated.h"

UCLASS()
class MYACTIONROGUELIKE_API ASMagicProjectile : public ASProjectile
{
	GENERATED_BODY()

public:
	ASMagicProjectile();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnActorOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                    int OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
