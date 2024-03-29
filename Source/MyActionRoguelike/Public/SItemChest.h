// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SGameplayInterface.h"
#include "SItemChest.generated.h"

class UStaticMeshComponent;

UCLASS()
class MYACTIONROGUELIKE_API ASItemChest : public AActor, public ISGameplayInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere)
	float TargetPitch;

	// Sets default values for this actor's properties
	ASItemChest();

	virtual void OnActorLoaded_Implementation() override;
	
	void Interact_Implementation(APawn* InstigatorPawn) override;

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* LidMesh;

	UPROPERTY(ReplicatedUsing="OnRep_LidOpen", BlueprintReadOnly, SaveGame) // RepNotify (name in blueprint)
	bool bLidOpened;
	
	// Function run when 'bLidOpen' value is updated on "Client"
	UFUNCTION()
	void OnRep_LidOpen();
};
