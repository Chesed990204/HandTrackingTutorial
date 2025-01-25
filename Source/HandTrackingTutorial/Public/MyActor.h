#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Networking.h"
#include "Sockets.h"
#include "SocketSubsystem.h"
#include "MyActor.generated.h"

UCLASS()
class HANDTRACKINGTUTORIAL_API AMyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AMyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Create 3d vector with double dtype that exposed to blueprint interface
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vector")
	FVector vec;

	// Create a function that exposed to blueprint interface
	UFUNCTION(BlueprintCallable, Category = "Vector")
	void SendData(const FVector& NewVector, const int32& mode);

private:
	void InitializeSocket();
	void ReceiveData();

	FSocket* Socket;
};
