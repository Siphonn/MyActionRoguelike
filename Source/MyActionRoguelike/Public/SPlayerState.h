// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "SPlayerState.generated.h"

class USSaveGame;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnCreditsChanged, ASPlayerState*, PlayerState, int32, NewCredits, int32, Delta);

UCLASS()
class MYACTIONROGUELIKE_API ASPlayerState : public APlayerState
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, ReplicatedUsing="OnRep_Credits" , Category="Credits") // Reliable
	int32 Credits;

	// Function run when 'credits' value is updated on "Client"
	// OnRep_ can use a parameter containing the 'old value' of the variables it is bound to. Very useful in this case to figure out the 'delta'.
	UFUNCTION()
	void OnRep_Credits(int32 OldCredits);

	// Downside of using a multicast is that is sends more data over the net, since it's an RPC with two parameters. OnRep_ is "free" since Credits is already being replicated anyway. 
	// UFUNCTION(NetMulticast, Reliable)
	// void MulticastOnCreditsChanged(int32 NewCredits, int32 Delta);

public:
	UPROPERTY(BlueprintAssignable, Category="Events")
	FOnCreditsChanged OnCreditsUpdated;
	
	UFUNCTION(BlueprintCallable, Category="Credits")
	int32 GetCredits() const;

	UFUNCTION(BlueprintCallable, Category="Credits")
	void AddCredit(int32 CreditAmount);

	UFUNCTION(BlueprintCallable, Category="Credits")
	bool RemoveCredits(int CreditAmount);
	
	UFUNCTION(BlueprintNativeEvent)
	void SavePlayerState(USSaveGame* SaveObject);

	UFUNCTION(BlueprintNativeEvent)
	void LoadPlayerState(USSaveGame* SaveObject);
};
