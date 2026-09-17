// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "HealthSystem.h"
#include "PlayerCharacter.generated.h"


struct FInputActionValue;
class UInputMappingContext;
class UInputAction;

UCLASS()
class ADVENTURE_MASK_API APlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UHealthSystem* HealthSystem;
};
