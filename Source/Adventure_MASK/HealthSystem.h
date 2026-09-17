// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthSystem.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ADVENTURE_MASK_API UHealthSystem : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthSystem();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "HealthSystem")
	void ReceiveAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "HealthSystem")
	void ReceivePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "HealthSystem")
	void ReceiveRadialDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin, const FHitResult& HitInfo, class AController* InstigatedBy, AActor* DamageCauser);
	
protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void ReceivePointDamage_Implementation(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);
	virtual void ReceiveAnyDamage_Implementation(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	virtual void ReceiveRadialDamage_Implementation(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, FVector Origin, const FHitResult& HitInfo, class AController* InstigatedBy, AActor* DamageCauser);
	
	
	void ProcessDamage(float DamageAmount);
	
	UPROPERTY(EditAnywhere, Category = "HealthSystem", meta = (AllowPrivateAccess = "true"))
	float MaxHealth;	
	UPROPERTY(EditAnywhere, Category = "HealthSystem", meta = (AllowPrivateAccess = "true"))
	float CurrentHealth = 0;
	UPROPERTY(EditAnywhere, Category = "HealthSystem", meta = (AllowPrivateAccess = "true"))
	bool bIsDead = false;
	UPROPERTY(EditAnywhere, Category = "HealthSystem", meta = (AllowPrivateAccess = "true"))
	ACharacter* OwnerCharacter;
	
	UPROPERTY(EditAnywhere, Category = "HealthSystem", meta = (AllowPrivateAccess = "true"))
	bool bBindReceiveDamage = false;	
	UPROPERTY(EditAnywhere, Category = "HealthSystem", meta = (AllowPrivateAccess = "true"))
	bool bBindReceiveAnyDamage = false;	
	UPROPERTY(EditAnywhere, Category = "HealthSystem", meta = (AllowPrivateAccess = "true"))
	bool bBindReceivePointDamage = true;	
	UPROPERTY(EditAnywhere, Category = "HealthSystem", meta = (AllowPrivateAccess = "true"))
	bool bBindReceiveRadialDamage= false;
};
