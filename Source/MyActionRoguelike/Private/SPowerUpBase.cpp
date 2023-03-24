// Fill out your copyright notice in the Description page of Project Settings.


#include "SPowerUpBase.h"
#include "Net/UnrealNetwork.h"


ASPowerUpBase::ASPowerUpBase()
{
	SphereComp = CreateDefaultSubobject<USphereComponent>("SphereComp");
	SphereComp->SetCollisionProfileName("PowerUp");
	RootComponent = SphereComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	// Disable collision, instead we use SphereComp to handle interaction queries
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetupAttachment(RootComponent);

	RespawnTime = 10.0f;

	SetReplicates(true);
}

void ASPowerUpBase::Interact_Implementation(APawn* InstigatorPawn)
{
	/// Logic in derived classes...
	
	bPowerUpUsed = !bPowerUpUsed;
	// Run on server
	OnRep_PowerUpUsed();
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

void ASPowerUpBase::OnRep_PowerUpUsed()
{
	// Apply PowerUp effect
	// Hide and Cooldown power-up

	// *NOTE: May only need to run Hide and Cooldown power-up 
	
	HideAndCooldownPowerUp();
}

void ASPowerUpBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ASPowerUpBase, bPowerUpUsed);
}
