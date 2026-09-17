// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Navigation/CrowdAgentInterface.h"
#include "CrowdAgentComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(
	FOnMovementUpdate, // Name of the structure that will be generated
	// Parameters of the delegate (Type, Name):
	bool, bIsMoving // New value
);

class UCapsuleComponent;
class APAwn;
class UCharacterMovementComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ADVENTURE_MASK_API UCrowdAgentComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bAvoid = true;

private:

	UPROPERTY(BlueprintAssignable, meta = (AllowPrivateAccess = "true"))
	FOnMovementUpdate OnMovementUpdate;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	UCapsuleComponent* AvoidanceCapsule;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float AvoidanceRadius = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float AvoidanceStrength = 10000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float PullStrength = 7500.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float PullAcceptanceRadius = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float TurnSpeed = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MinRandomWalkSpeedMultiplier = 0.75f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (AllowPrivateAccess = "true"))
	float MaxRandomWalkSpeedMultiplier = 1.5f;

	bool bIsMoving = false;
	bool bPushed = false;
	bool bLocked = false;
	bool bHasOriginalLocation = false;
	float PullTime = 0.f;
	FVector OriginalLocation = FVector::ZeroVector;
	FVector OriginalForward = FVector::ZeroVector;
	FRotator TargetRotation = FRotator::ZeroRotator;
	APawn* Owner = nullptr;
	UCharacterMovementComponent* MoveComp = nullptr;

public:	
	// Sets default values for this component's properties
	UCrowdAgentComponent();

	void MoveTo(FVector Location);
	void LookAt(FVector Location);
	void Lock();
	void Unlock();

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnRegister() override;

	UFUNCTION(BlueprintCallable)
	void ResetOriginalLocation();
	UFUNCTION(BlueprintCallable)
	void SetAvoidance(bool bAvoidance);
};
