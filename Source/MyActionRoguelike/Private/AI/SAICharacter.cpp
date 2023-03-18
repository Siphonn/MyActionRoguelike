// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SAICharacter.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/UserWidget.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Perception/PawnSensingComponent.h"
#include "SActionComponent.h"
#include "SAttributeComponent.h"
#include "SWorldUserWidget.h"


ASAICharacter::ASAICharacter()
{
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>("PawnSensingComp");
	AttributeComp = CreateDefaultSubobject<USAttributeComponent>("AttributeComp");
	ActionComp = CreateDefaultSubobject<USActionComponent>("ActionComp");
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);

	TimeToHitParam = "TimeToHit";
	TargetActorKey = "TargetActor";
}

void ASAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &ASAICharacter::OnPawnSeen);
	AttributeComp->OnHealthChanged.AddDynamic(this, &ASAICharacter::OnHealthChanged);
}

// Set the TargetActor value in the BlackBoard component
void ASAICharacter::SetTargetActor(AActor* NewTarget)
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		UBlackboardComponent* BlackBoardComp = AIController->GetBlackboardComponent();

		BlackBoardComp->SetValueAsObject(TargetActorKey, NewTarget);
	}
}

AActor* ASAICharacter::GetTargetActor() const
{
	AAIController* AIController = Cast<AAIController>(GetController());
	if (AIController)
	{
		return Cast<AActor>(AIController->GetBlackboardComponent()->GetValueAsObject(TargetActorKey));
	}
	return nullptr;
}

void ASAICharacter::OnPawnSeen(APawn* Pawn)
{
	if(GetTargetActor() != Pawn)
	{
		SetTargetActor(Pawn);
		

		// TODO: Enable Exclamation mark widget
		// Disable after set time
		if (ExclamationMarkInstance == nullptr && ensure(ExclamationMarkWidget))
		{
			ExclamationMarkInstance = CreateWidget<USWorldUserWidget>(GetWorld(), ExclamationMarkWidget);
		}

		if (ExclamationMarkInstance)
		{
			ExclamationMarkInstance->AttachedActor = this;

			if (!ExclamationMarkInstance->IsInViewport())
			{
				ExclamationMarkInstance->AddToViewport(10);
			}
		}
	}
	// DrawDebugString(GetWorld(), GetActorLocation(), "PLAYER SPOTTED", nullptr, FColor::White, 4.0f, true);
}

void ASAICharacter::OnHealthChanged(AActor* InstigatorActor, USAttributeComponent* OwningComp, float NewHealth, float Delta)
{
	if (Delta < 0.0f)
	{
		if (InstigatorActor != this)
		{
			SetTargetActor(InstigatorActor);
		}

		if (ActiveHealthBar == nullptr)
		{
			ActiveHealthBar = CreateWidget<USWorldUserWidget>(GetWorld(), HealthBarWidgetClass);
			if (ActiveHealthBar)
			{
				ActiveHealthBar->AttachedActor = this;
				ActiveHealthBar->AddToViewport();
			}
		}

		GetMesh()->SetScalarParameterValueOnMaterials(TimeToHitParam, GetWorld()->TimeSeconds);

		// Died
		if (NewHealth <= 0.0f)
		{
			// stop BT
			AAIController* AIC = Cast<AAIController>(GetController());
			if (AIC)
			{
				AIC->GetBrainComponent()->StopLogic("Killed");
			}

			// ragdoll - death animation
			GetMesh()->SetAllBodiesSimulatePhysics(true);
			GetMesh()->SetCollisionProfileName("Ragdoll");

			// disable collision & movement
			GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
			GetCharacterMovement()->DisableMovement();

			// set lifespan (Death)
			SetLifeSpan(10.0f);
		}
	}
}
