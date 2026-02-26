#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "LoaiDanCuaTui.generated.h"


UCLASS()
class MYPROJECT_API ALoaiDanCuaTui : public AActor
{
	GENERATED_BODY()

public:
	ALoaiDanCuaTui();

protected:
	virtual void BeginPlay() override;
	
	// Bind a delegate to our projectile OnComponentHit
	UFUNCTION()
	void OnProjectileHit( UPrimitiveComponent* HitComponent,AActor* OtherActor,
		UPrimitiveComponent* OtherComp,FVector NormalImpulse,const FHitResult& Hit);

	
	UFUNCTION()
	void SelfDestroy();

	FTimerHandle DestroyTimerHandle;
	FTimerHandle DelayTimerHandle;
	
public:
	virtual void Tick(float DeltaTime) override;
	
	// Tao bien mau
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
	FLinearColor DoiMauThanh = FLinearColor::Red;
		
	// Khoi tao: Sphere Collision - Scene Root + Mesh + Projectile
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> CollisionSphere;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> ThayMeshVienDanVaoDay;
	
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	
	// === Owner & Spawn Logic ===
	
	UPROPERTY(BlueprintReadWrite, Category = "Bullet")
	AActor* OwnerActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bullet")
	float SpawnForwardOffset = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bullet")
	float SpawnUpOffset = 60.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Bullet")
	float SpawnRightOffset = 5.f;
	

};