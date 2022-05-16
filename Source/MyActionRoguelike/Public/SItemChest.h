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
	
	void Interact_Implementation(APawn* InstigatorPawn) override;

public:
	// Sets default values for this actor's properties
	ASItemChest();

protected:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* BaseMesh;
	UPROPERTY(VisibleAnywhere , BlueprintReadOnly)
	UStaticMeshComponent* LidMesh;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
};
