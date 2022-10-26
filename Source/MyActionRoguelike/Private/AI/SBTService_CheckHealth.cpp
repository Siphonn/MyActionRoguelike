// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTService_CheckHealth.h"

#include "AIController.h"
#include "SAttributeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

void USBTService_CheckHealth::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	/// TODO
	// Check if Health is low
	// If health low, move to a position were the player cannot be seen (use EQS to find position)
	// when in position, HEAL (BT Task)
	// behaviour can only run once every 60 seconds
	// Once healed, return to fighting

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (ensure(BlackboardComp))
	{
		APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();

		if (ensure(AIPawn))
		{
			USAttributeComponent* AttributeComp = AIPawn->FindComponentByClass<USAttributeComponent>();
			if (ensure(AttributeComp))
			{
				BlackboardComp->SetValueAsBool(HealthKey.SelectedKeyName, AttributeComp->GetHealth() < 50.0f);
			}
		}
	}
}
