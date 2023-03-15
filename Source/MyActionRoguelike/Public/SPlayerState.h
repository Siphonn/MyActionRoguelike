// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditsChanged, ASPlayerState*, PlayerState, int32, NewCredits, int32, Delta);

UCLASS()
class MYACTIONROGUELIKE_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category="Credits")
	int32 Credits;

public:
	UFUNCTION(BlueprintCallable, Category="Credits")
	int32 GetCredits() const;

	UFUNCTION(BlueprintCallable, Category="Credits")
	void AddCredit(int32 CreditAmount);

	UFUNCTION(BlueprintCallable, Category="Credits")
	bool RemoveCredits(int CreditAmount);

	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnCreditsChanged OnCreditsUpdated;
};
