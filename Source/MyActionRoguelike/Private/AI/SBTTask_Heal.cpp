// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/SBTTask_Heal.h"
#include "AIController.h"
#include "SAttributeComponent.h"

EBTNodeResult::Type USBTTask_Heal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	//return Super::ExecuteTask(OwnerComp, NodeMemory);

	APawn* AIPawn = OwnerComp.GetAIOwner()->GetPawn();

	if (ensure(AIPawn))
	{
		USAttributeComponent* AttributeComp = AIPawn->FindComponentByClass<USAttributeComponent>();
		if (AttributeComp == nullptr)
		{
			return EBTNodeResult::Failed;
		}
		AttributeComp->ApplyHealthChange(AttributeComp->GetMaxHealth());
		return EBTNodeResult::Succeeded;
	}
	return EBTNodeResult::Failed;
}
