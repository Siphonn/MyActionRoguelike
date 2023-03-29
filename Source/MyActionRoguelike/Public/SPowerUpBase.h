// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SGameplayInterface.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "SPowerUpBase.generated.h"

UCLASS()
class MYACTIONROGUELIKE_API ASPowerUpBase : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()

public:
	ASPowerUpBase();

	virtual void Interact_Implementation(APawn* InstigatorPawn) override;

	virtual FText GetInteractText_Implementation(APawn* InstigatorPawn) override;

protected:
	UPROPERTY(EditAnywhere, Category="Power Up")
	float RespawnTime;

	FTimerHandle TimerHandle_RespawnTimer;

	UFUNCTION()
	void ShowPowerUp();
	void HideAndCooldownPowerUp();
	void SetPowerUpState(bool bNewIsActive);

	UPROPERTY(VisibleAnywhere, Category="Components")
	USphereComponent* SphereComp;
	UPROPERTY(VisibleAnywhere, Category="Components")
	UStaticMeshComponent* MeshComp;

	// For Networking
	UPROPERTY(ReplicatedUsing="OnRep_IsActive")
	bool bIsActive;

	UFUNCTION()
	void OnRep_IsActive();
};
