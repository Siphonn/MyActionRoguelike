// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SAction.h"
#include "SActionEffect.generated.h"

/**
 * 
 */
UCLASS()
class MYACTIONROGUELIKE_API USActionEffect : public USAction
{
	GENERATED_BODY()

public:
	USActionEffect();

	virtual void StartAction_Implementation(AActor* Instigator) override;
	virtual void StopAction_Implementation(AActor* Instigator) override;

	UFUNCTION(BlueprintCallable, Category="Action")
	float GetTimeRemaining() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effect")
	float Duration;
	// Time between 'Ticks' to apply damage
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effect")
	float Period;
	FTimerHandle DurationHandle;
	FTimerHandle PeriodHandle;

	UFUNCTION(BlueprintNativeEvent, Category="Effect")
	void ExecutePeriodicEffect(AActor* Instigator);
};
