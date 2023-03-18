// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SActionEffect.h"
#include "SActionEffect_Thorns.generated.h"

/**
 * 
 */
UCLASS()
class MYACTIONROGUELIKE_API USActionEffect_Thorns : public USActionEffect
{
	GENERATED_BODY()

public:
	USActionEffect_Thorns();

	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category="Thorns")
	float ReflectFraction;
	UFUNCTION()
	void OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta);
};
