// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "SAction.generated.h"

class UWorld;

UCLASS(Blueprintable)
class MYACTIONROGUELIKE_API USAction : public UObject
{
	GENERATED_BODY()

public:
	/* Action nickname to start/stop without a reference to the object*/
	UPROPERTY(EditDefaultsOnly, Category="Actions")
	FName ActionName;

	UFUNCTION(BlueprintNativeEvent, Category="Action")
	void StartAction(AActor* Instigator);
	UFUNCTION(BlueprintNativeEvent, Category="Action")
	void StopAction(AActor* Instigator);

	virtual UWorld* GetWorld() const override;
};
