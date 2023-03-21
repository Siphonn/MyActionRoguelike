// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SInteractionComponent.generated.h"


class USWorldUserWidget;


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYACTIONROGUELIKE_API USInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	USInteractionComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void PrimaryInteract();

protected:

	UPROPERTY(EditDefaultsOnly, Category="Trace")
	float TraceDistance;
	UPROPERTY(EditDefaultsOnly, Category="Trace")
	float TraceRadius;
	UPROPERTY(EditDefaultsOnly, Category="Trace")
	TEnumAsByte<ECollisionChannel> CollisionChannel;
	UPROPERTY()
	AActor* FocusedActor;
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TSubclassOf<USWorldUserWidget> DefaultWidgetClass;
	UPROPERTY()
	USWorldUserWidget* DefaultWidgetInstance;

	void FindBestInteractable();

	// Reliable - will always arrive, eventually. Request will be re-sent unless an acknowledgement was received. (TCP)
	// Unreliable - Not guaranteed, packet can be lost and won't reply. (UDp)
	
	UFUNCTION(Server, Reliable)
	void ServerInteract(AActor* InFocus);
};
