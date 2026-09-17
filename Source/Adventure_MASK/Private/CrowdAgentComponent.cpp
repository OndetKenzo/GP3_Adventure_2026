// Fill out your copyright notice in the Description page of Project Settings.


#include "CrowdAgentComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"
#include "CrowdSubsystem.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"


// Sets default values for this component's properties
UCrowdAgentComponent::UCrowdAgentComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	AvoidanceCapsule = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Avoidance Capsule"));
	AvoidanceCapsule->SetCapsuleRadius(44.f);
	AvoidanceCapsule->SetCapsuleHalfHeight(88.f);
}

void UCrowdAgentComponent::ResetOriginalLocation()
{
	bHasOriginalLocation = false;
}

void UCrowdAgentComponent::MoveTo(FVector Location)
{
	if (!Owner)
		return;

	AAIController* Controller = Cast<AAIController>(Owner->GetController());
	if (!Controller)
	{
		UE_LOG(LogTemp, Warning, TEXT("No controller found!"));
		return;
	}

	Controller->MoveToLocation(Location, 5.f);
}

void UCrowdAgentComponent::LookAt(FVector Location)
{
	FVector AdjustedLocation = Location;
	AdjustedLocation.Z = GetOwner()->GetActorLocation().Z;
	FRotator Rot = UKismetMathLibrary::FindLookAtRotation(GetOwner()->GetActorLocation(), AdjustedLocation);
	bool bSuccess = GetOwner()->SetActorRotation(Rot);
}

void UCrowdAgentComponent::SetAvoidance(bool bAvoidance)
{
	bAvoid = bAvoidance;
}

void UCrowdAgentComponent::Lock()
{
	bLocked = true;
}

void UCrowdAgentComponent::Unlock()
{
	bLocked = false;
	bPushed = true;

	if (!Owner)
		return;

	AAIController* Controller = Cast<AAIController>(Owner->GetController());
	if (!Controller)
	{
		UE_LOG(LogTemp, Warning, TEXT("No controller found!"));
		return;
	}

	Controller->StopMovement();
}


// Called when the game starts
void UCrowdAgentComponent::BeginPlay()
{
	Super::BeginPlay();


	UCrowdSubsystem* CrowdSubsystem = GetWorld()->GetSubsystem<UCrowdSubsystem>();
	if (CrowdSubsystem)
	{
		CrowdSubsystem->RegisterComponent(this);
	}

	Owner = Cast<APawn>(GetOwner());
	if (!Owner)
		return;

	MoveComp = Cast<UCharacterMovementComponent>(Owner->GetMovementComponent());
	if (!MoveComp)
		return;

	MoveComp->MaxWalkSpeed *= FMath::FRandRange(MinRandomWalkSpeedMultiplier, MaxRandomWalkSpeedMultiplier);
}


void UCrowdAgentComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (!bHasOriginalLocation)
	{
		if (!MoveComp->IsFalling())
		{
			OriginalLocation = Owner->GetActorLocation();
			OriginalForward = Owner->GetActorForwardVector();
			bHasOriginalLocation = true;
		}
		else
		{
			return;
		}
	}

	if (!Owner || !MoveComp)
		return;

	if (bAvoid)
	{
		if (!AvoidanceCapsule)
			return;

		TArray<AActor*> OverlappingActors;
		AvoidanceCapsule->GetOverlappingActors(OverlappingActors, ACharacter::StaticClass());
		OverlappingActors.Remove(Owner);

		FVector AvoidanceVector = FVector::ZeroVector;
		for (int i = 0; i < OverlappingActors.Num(); ++i)
		{
			UCrowdAgentComponent* Comp = OverlappingActors[i]->GetComponentByClass<UCrowdAgentComponent>();
			if (Comp)
			{
				FVector AwayVector = Owner->GetActorLocation() - OverlappingActors[i]->GetActorLocation();
				float Distance = AwayVector.Length();

				if (Distance <= AvoidanceRadius + Comp->AvoidanceRadius)
				{
					// If overlapping actor is Playeror Elite, we have been pushed
					if (!Comp->bAvoid)
						bPushed = true;

					// Use an exponential falloff to smooth avoidance steering
					float NormalizedDistance = Distance / (AvoidanceRadius + Comp->AvoidanceRadius);
					float Weight = FMath::Exp(-NormalizedDistance * 3.f);
					AwayVector.Normalize();
					//TODO: Add rotation to better avoid moving comp
					AvoidanceVector += AwayVector * Weight;
				}
			}
		}

		// Go back to original position
		FVector PullVector = FVector::ZeroVector;
		if (bPushed && !bLocked)
		{
			FVector ReturnVector = OriginalLocation - GetOwner()->GetActorLocation();
			float Distance = ReturnVector.Length();
			if (Distance <= PullAcceptanceRadius)
				bPushed = false;
			else
				PullVector = ReturnVector.GetSafeNormal() * PullStrength;
		}

		// Blend between avoidance and going back to original position
		float AvoidanceWeight = FMath::Clamp(AvoidanceVector.Length(), 0.f, 1.f);
		float PullWeight = 1.f - AvoidanceWeight;

		if(FMath::IsNearlyZero(AvoidanceWeight))
		{
			PullTime = GetWorld()->GetTime().GetWorldTimeSeconds();
		}

		FVector FinalAvoidanceVector = AvoidanceVector * AvoidanceStrength;
		FVector FinalPullVector = PullVector * PullWeight;

		bool bHasAvoidance = !FMath::IsNearlyZero(FinalAvoidanceVector.Length());
		bool bHasPull = !FMath::IsNearlyZero(FinalPullVector.Length());

		if (bHasAvoidance)
			MoveComp->AddForce(FinalAvoidanceVector);
		if (bHasPull)
			MoveComp->AddForce(FinalPullVector);

		if (!bLocked)
		{
			FVector TargetVector = bHasAvoidance ? FinalAvoidanceVector.GetSafeNormal() : (bHasPull ? FinalPullVector.GetSafeNormal() : OriginalForward);
			FRotator CurrentTargetRotation = UKismetMathLibrary::FindLookAtRotation(Owner->GetActorLocation(), Owner->GetActorLocation() + TargetVector);
			TargetRotation = FMath::RInterpTo(TargetRotation, CurrentTargetRotation, DeltaTime, TurnSpeed);
			FRotator Current = FMath::RInterpTo(Owner->GetActorRotation(), TargetRotation, DeltaTime, TurnSpeed);;
			Owner->SetActorRotation(Current);
		}
	}

	// Broadcast movement updates for animations
	if (MoveComp->Velocity.Length() > 50.f)
	{
		if (!bIsMoving)
		{
			bIsMoving = true;
			OnMovementUpdate.Broadcast(true);
		}
	}
	else
	{
		if (bIsMoving)
		{
			bIsMoving = false;
			OnMovementUpdate.Broadcast(false);
		}
	}
}

void UCrowdAgentComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UCrowdSubsystem* CrowdSubsystem = GetWorld()->GetSubsystem<UCrowdSubsystem>();
	if (CrowdSubsystem)
	{
		CrowdSubsystem->UnregisterComponent(this);
	}
}

void UCrowdAgentComponent::OnRegister()
{
	Super::OnRegister();

	if (AvoidanceCapsule && GetOwner() && GetOwner()->GetRootComponent())
	{
		AvoidanceCapsule->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		AvoidanceCapsule->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
		AvoidanceCapsule->SetRelativeRotation(FRotator::ZeroRotator);
	}
}