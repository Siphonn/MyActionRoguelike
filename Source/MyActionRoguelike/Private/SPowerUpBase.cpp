// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUpBase.h"


ASPowerUpBase::ASPowerUpBase()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("PowerUp");
	RootComponent = SphereComp;

	RespawnTime = 10.0f;
}

void ASPowerUpBase::Interact_Implementation(APawn* InstigatorPawn)
{
	/// Logic in derived classes...
}

void ASPowerUpBase::ShowPowerUp()
{
	SetPowerUpState(true);
}

void ASPowerUpBase::HideAndCooldownPowerUp()
{
	SetPowerUpState(false);

	GetWorldTimerManager().SetTimer(TimerHandle_RespawnTimer, this, &ASPowerUpBase::ShowPowerUp, RespawnTime);
}

void ASPowerUpBase::SetPowerUpState(bool bNewIsActive)
{
	SetActorEnableCollision(bNewIsActive);

	/// Set visibilty on root and all children
	RootComponent->SetVisibility(bNewIsActive, true);
}
