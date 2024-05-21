// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowableActor.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "InteractInterface.h"
#include "TantrumnCharacterBase.h"

// Sets default values
AThrowableActor::AThrowableActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMeshComponent");
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
	RootComponent = StaticMeshComponent;
}

// Called every frame
//void AThrowableActor::Tick(float DeltaTime)
//{
//	Super::Tick(DeltaTime);
//}

// Called when the game starts or when spawned
void AThrowableActor::BeginPlay()
{
	Super::BeginPlay();
	ProjectileMovementComponent->OnProjectileStop.AddDynamic(this, &AThrowableActor::ProjectileStop);
}

void AThrowableActor::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	ProjectileMovementComponent->OnProjectileStop.RemoveDynamic(this, &AThrowableActor::ProjectileStop);
	Super::EndPlay(EndPlayReason);
}

void AThrowableActor::NotifyHit(UPrimitiveComponent* MyComp, AActor* Other, UPrimitiveComponent* OtherComp, bool bSelfMoved, FVector HitLocation, FVector HitNormal, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::NotifyHit(MyComp, Other, OtherComp, bSelfMoved, HitLocation, HitNormal, NormalImpulse, Hit);
	if (State == EState::Idle || State == EState::Attached || State == EState::Dropped)
	{
		return;
	}

	// Three options when hit
	// If in attached ignore

	// If not our actor is being pulled or launched
	// pulled we want to check that the hit is of the actor pulling
	// in which case it's a successful attach

	// If launched and hit a character that is not the launcher
	// do damage of whatever we want

	if (State == EState::Launch)
	{
		IInteractInterface* I = Cast<IInteractInterface>(Other);
		if (I)
		{
			I->Execute_ApplyEffect(Other, EffectType, false);
		}
	}

	// Ignore all other hits
	
	// This will wait until the projectile comes to a natural stop before returning it to idle


	if (PullActor && State == EState::Pull)
	{
		if (ATantrumnCharacterBase* TantrumnCharacter = Cast<ATantrumnCharacterBase>(PullActor))
		{
			if (Other == PullActor)
			{
				FName HandSocketName = TEXT("hand_l");
				USkeletalMeshComponent* CharacterMesh = TantrumnCharacter->GetMesh();
				if (CharacterMesh && CharacterMesh->DoesSocketExist(HandSocketName))
				{
					FAttachmentTransformRules AttachmentRules = FAttachmentTransformRules::SnapToTargetNotIncludingScale; // Ignore scale
                    AttachToComponent(CharacterMesh, AttachmentRules, HandSocketName);
                    SetOwner(TantrumnCharacter);
                    ProjectileMovementComponent->Deactivate();
                    State = EState::Attached;
                    TantrumnCharacter->OnThrowableAttached(this);
				}
			}
			else
			{
				TantrumnCharacter->ResetThrowableObject();
				State = EState::Dropped;
			}
		}
	}

	ProjectileMovementComponent->HomingTargetComponent = nullptr;
	PullActor = nullptr;
}

void AThrowableActor::ProjectileStop(const FHitResult& ImpactResult)
{
	if (State == EState::Launch || State == EState::Dropped)
	{
		State = EState::Idle;
	}
}

bool AThrowableActor::SetHomingTarget(AActor* Target)
{
	if (Target)
	{
		if (USceneComponent* SceneComponent = Cast<USceneComponent>(Target->GetComponentByClass(USceneComponent::StaticClass())))
		{
			if (USceneComponent* ThrowableSceneComponent = Cast <USceneComponent>(GetComponentByClass(USceneComponent::StaticClass())))
			{
				//I DID IT I ****ING DID IT!
				float Speed = 1000.f;
				FVector Direction = Target->GetActorLocation() - GetActorLocation();
				Direction.Z += 2000.0f * FVector::DotProduct(Direction, Direction) / (Speed * Speed);
				Direction.Normalize();

				ProjectileMovementComponent->Velocity = Direction * Speed;
				
				ProjectileMovementComponent->SetUpdatedComponent(ThrowableSceneComponent);
				ProjectileMovementComponent->Activate(true);
				ProjectileMovementComponent->HomingTargetComponent = TWeakObjectPtr<USceneComponent>(SceneComponent);
				ProjectileMovementComponent->bIsHomingProjectile = true;
				return true;
			}
		}
	}

	return false;
}

bool AThrowableActor::Pull(AActor* InActor)
{
	if (State != EState::Idle)
	{
		return false;
	}

	if (SetHomingTarget(InActor))
	{
		ToggleHighlight(false);
		State = EState::Pull;
		PullActor = InActor;
		return true;
	}

	return false;
}

void AThrowableActor::Launch(const FVector& InitialVelocity, AActor* Target)
{
	if (State == EState::Pull || State == EState::Attached)
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		ProjectileMovementComponent->Activate(true);
		ProjectileMovementComponent->HomingTargetComponent = nullptr;

		State = EState::Launch;

		if (Target)
		{
			if (USceneComponent* SceneComponent = Cast<USceneComponent>(Target->GetComponentByClass(USceneComponent::StaticClass())))
			{
				ProjectileMovementComponent->HomingTargetComponent = TWeakObjectPtr<USceneComponent>(SceneComponent);
				return;
			}
		}

		ProjectileMovementComponent->Velocity = InitialVelocity;
	}
}

void AThrowableActor::Drop()
{
	if (State == EState::Attached)
	{
		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}

	ProjectileMovementComponent->Activate(true);
	ProjectileMovementComponent->HomingTargetComponent = nullptr;
	State = EState::Dropped;
}

void AThrowableActor::ToggleHighlight(bool bIsOn)
{
	StaticMeshComponent->SetRenderCustomDepth(bIsOn);
}

EEffectType AThrowableActor::GetEffectType()
{
	return EffectType;
}
