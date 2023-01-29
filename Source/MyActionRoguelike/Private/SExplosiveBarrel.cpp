// Fill out your copyright notice in the Description page of Project Settings.


#include "SExplosiveBarrel.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "SAttributeComponent.h"

// Sets default values
ASExplosiveBarrel::ASExplosiveBarrel()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>("MeshComp");
	MeshComp->SetSimulatePhysics(true);
	RootComponent = MeshComp;

	RadialForceComp = CreateDefaultSubobject<URadialForceComponent>("RadialForceComp");
	RadialForceComp->SetupAttachment(MeshComp);

	/// Leaving this on applies small constant force component 'tick' (optional)
	RadialForceComp->bAutoActivate = false;

	RadialForceComp->Radius = 500.0f;
	RadialForceComp->ImpulseStrength = 2000.0f;
	/// Alternative: 200000.0 if bImpulseVelChange = false
	   /// Optional, ignores 'Mass' of other objects (if false, the impulse strength will be much higher to push push most objects depending on Mass)
	RadialForceComp->bImpulseVelChange = true;

	/// Optional, default constructor of component already adds 4 object types to effect, excluding WorldDynamic
	RadialForceComp->AddCollisionChannelToAffect(ECC_WorldDynamic);

	// MeshComp->SetCollisionProfileName("PhysicsActor");
	// MeshComp->bIgnoreRadialForce = true;
	// RadialForceComp->Falloff = RIF_Constant;
}

void ASExplosiveBarrel::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	MeshComp->OnComponentHit.AddDynamic(this, &ASExplosiveBarrel::OnHit);
}

void ASExplosiveBarrel::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& Hit)
{
	RadialForceComp->FireImpulse();

	UE_LOG(LogTemp, Log, TEXT("OnHit in Explosive Barrel"));

	/// %s = string
	/// %f = float
	UE_LOG(LogTemp, Log, TEXT("OtherActor: %s, at gametime: %f"), *GetNameSafe(OtherActor), GetWorld()->TimeSeconds);

	FString CombinedString = FString::Printf(TEXT("Hit at location: %s"), *Hit.ImpactPoint.ToString());
	DrawDebugString(GetWorld(), Hit.ImpactPoint, CombinedString, nullptr, FColor::Green, 2.0f, true);

	if (OtherActor)
	{
		USAttributeComponent* AttributeComp = Cast<USAttributeComponent>(
			OtherActor->GetComponentByClass(USAttributeComponent::StaticClass()));
		if (AttributeComp)
		{
			AttributeComp->ApplyHealthChange(this, -50.0f);
		}
	}

	/// Detailed info on logging in ue4
	/// https://nerivec.github.io/old-ue4-wiki/pages/logs-printing-messages-to-yourself-during-runtime.html
}
