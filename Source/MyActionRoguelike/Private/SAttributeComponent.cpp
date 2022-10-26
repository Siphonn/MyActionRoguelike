// Fill out your copyright notice in the Description page of Project Settings.


#include "SAttributeComponent.h"

// Sets default values for this component's properties
USAttributeComponent::USAttributeComponent()
{
	Health = 100;
	MaxHealth = 100;
}

bool USAttributeComponent::IsAlive() const
{
	return Health > 0.0f;
}

bool USAttributeComponent::IsFullHealth() const
{
	return Health == MaxHealth;
}

float USAttributeComponent::GetMaxHealth() const
{
	return MaxHealth;
}

float USAttributeComponent::GetHealth() const
{
	return Health;
}

bool USAttributeComponent::ApplyHealthChange(float Delta)
{
	float OldHealth = Health;

	Health += Delta;
	Health = FMath::Clamp(Health, 0.0f, MaxHealth);

	float ActualDelta = Health - OldHealth;
	OnHealthChanged.Broadcast(nullptr, this, Health, ActualDelta);

	return ActualDelta != 0;
}
