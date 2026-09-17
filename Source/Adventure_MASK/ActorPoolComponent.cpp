// Fill out your copyright notice in the Description page of Project Settings.


#include "ActorPoolComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
UActorPoolComponent::UActorPoolComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UActorPoolComponent::BeginPlay()
{
	Super::BeginPlay();
	AvailableActors.Reserve(MaxPoolCount);
	for (int32 i = 0; i < DefaultInstanceCount; i++)
	{
		AActor* Actor = GetWorld()->SpawnActor<AActor>(ActorClass);
		if (IsValid(Actor))
		{
			Actor->SetActorHiddenInGame(true);
			Actor->SetActorEnableCollision(false);
			Actor->SetActorTickEnabled(false);
			AvailableActors.Add(Actor);
		}
	}
}

void UActorPoolComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	for (int32 i = 0; i < AvailableActors.Num(); i++)
	{
		AActor* Actor = AvailableActors[i];
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}
}

AActor* UActorPoolComponent::Pull()
{
	AActor* Actor = nullptr;
	if (!AvailableActors.IsEmpty())
	{
		Actor = AvailableActors.Pop();
	}
	else
	{
		for (int i = 0; i < ExpendPoolCount - 1; i++)
		{
			AActor* NewActor = GetWorld()->SpawnActor<AActor>(ActorClass);
			if (IsValid(NewActor))
			{
				NewActor->SetActorHiddenInGame(true);
				NewActor->SetActorEnableCollision(false);
				NewActor->SetActorTickEnabled(false);
				AvailableActors.Add(NewActor);
			}
		}
		
		Actor = GetWorld()->SpawnActor<AActor>(ActorClass);
	}
	
	if (IsValid(Actor))
	{
		Actor->SetActorHiddenInGame(false);
		Actor->SetActorEnableCollision(true);
		Actor->SetActorTickEnabled(true);
		TArray<UActorComponent*> Components;
		Actor->GetComponents(Components);
		for (UActorComponent* Comp : Components)
		{
			if (IsValid(Comp))
			{
				Comp->Activate(true); 
		
				if (Comp->PrimaryComponentTick.bCanEverTick)
				{
					Comp->SetComponentTickEnabled(true);
					Comp->RegisterAllComponentTickFunctions(true);
				}
			}
		}
		
		if (UCharacterMovementComponent* CMC = Actor->FindComponentByClass<UCharacterMovementComponent>())
		{
			CMC->SetMovementMode(EMovementMode::MOVE_Falling);
		}
		
		return Actor;
	}
	
	return Actor;
}

TArray<AActor*> UActorPoolComponent::PullArray(int32 Count)
{
	TArray<AActor*> PulledActors;
	PulledActors.Reserve(Count);
	for (int i = 0; i < Count; i++)
	{
		AActor* Actor = Pull();
		if (IsValid(Actor))
		{
			PulledActors.Add(Actor);
		}
		else
		{
			break;
		}
	}
	return PulledActors;
}

void UActorPoolComponent::Push(AActor* Actor)
{
	if (IsValid(Actor) && Actor->GetClass() == ActorClass && AvailableActors.Num() + 1 < MaxPoolCount)
	{
		Actor->SetActorHiddenInGame(true);
		Actor->SetActorEnableCollision(false);
		Actor->SetActorTickEnabled(false);

		TArray<UActorComponent*> Components;
		Actor->GetComponents(Components);
		for (UActorComponent* Comp : Components)
		{
			if (IsValid(Comp))
			{
				Comp->SetComponentTickEnabled(false);
				Comp->Deactivate();
			}
		}

		AvailableActors.Add(Actor);
	}
	else if (IsValid(Actor))
	{
		Actor->Destroy();
	}
}

void UActorPoolComponent::PushArray(TArray<AActor*> Actors)
{
	for (int i = 0; i < Actors.Num(); i++)
	{
		Push(Actors[i]);
	}
}
