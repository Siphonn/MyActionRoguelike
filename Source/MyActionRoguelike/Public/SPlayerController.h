// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPawnChanged, APawn*, NewPawn);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, APlayerState*, NewPlayerState);


/**
 * 
 */
UCLASS()
class MYACTIONROGUELIKE_API ASPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(BlueprintAssignable)
	FOnPawnChanged OnPawnChanged;

	/* Listen for the incoming Player state (for clients this may be a nullptr when initially joining game,
	 * afterwards player state will not change as PlayerControllers maintain the same player state throughout the level) */
	UPROPERTY(BlueprintAssignable)
	FOnPlayerStateChanged OnPlayerStateReceived;

	virtual void SetPawn(APawn* InPawn) override;

	/* Called when player controller is ready to begin playing, good moment to initialize things like UI which might be too early in BeginPlay
	 * (especially in multiplayer clients where not all data such as PlayerState may not have been received) */
	virtual void BeginPlayingState() override;

	UFUNCTION(BlueprintImplementableEvent)
	void BlueprintBeginPlayingState();

	virtual void OnRep_PlayerState() override;
};
