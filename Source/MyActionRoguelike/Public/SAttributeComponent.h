// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SAttributeComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnHealthChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewHealth, float, Delta);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FOnRageChanged, AActor*, InstigatorActor, USAttributeComponent*, OwningComp, float, NewRage, float, Delta);


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class MYACTIONROGUELIKE_API USAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USAttributeComponent();

	UFUNCTION(BlueprintCallable, Category="Attributes")
	static USAttributeComponent* GetAttributes(AActor* FromActor);
	UFUNCTION(BlueprintCallable, Category="Attributes", meta = (DisplayName = "IsAlive"))
	static bool IsActorAlive(AActor* Actor);

protected:
	/// EditAnywhere - edit in BP editor and per-instance in level.
	/// VisibleAnywhere - 'read-only' in editor and level. (Use for Components)
	/// EditDefaultOnly - hide variable per-instance
	/// VisibleDefaultOnly - 'read-only' access for variable, only in BP editor (uncommon)
	/// EditInstanceOnly - allow only editing of instance (eg. when placed in level)
	/// --
	/// BlueprintReadOnly - read-only in the Blueprint scripting (does not affect 'details' panel)
	/// BlueprintReadWrite - read-write access in Blueprints
	/// --
	/// Category = "" -  displayed only for detail panel and blueprint context menu.

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float Health;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float MaxHealth;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float Rage;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attributes")
	float MaxRage;

	// MaxHealth, Stamina, Strength


public:
	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool IsAlive() const;
	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool IsFullHealth() const;
	UFUNCTION(BlueprintCallable, Category="Attributes")
	float GetMaxHealth() const;
	UFUNCTION(BlueprintCallable, Category="Attributes")
	float GetHealth() const;
	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool Kill(AActor* InstigatorActor);

	UFUNCTION(BlueprintCallable)
	float GetRage() const;

	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnHealthChanged OnHealthChanged;
	UPROPERTY(BlueprintAssignable, Category="Attributes")
	FOnRageChanged OnRageChanged;

	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool ApplyHealthChange(AActor* InstigatorActor, float Delta);
	UFUNCTION(BlueprintCallable, Category="Attributes")
	bool ApplyRageChange(AActor* InstigatorActor, float Delta);
};
