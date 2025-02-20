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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyActor")
	FString RemoteAddrString;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyActor")
	int32 ReceivePort;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MyActor")
	int32 SendPort;

	// Create a function that exposed to blueprint interface
	UFUNCTION(BlueprintCallable, Category = "MyActor")
	void SendData(const FVector& NewVector, const int32& mode);

	UFUNCTION(BlueprintCallable, Category = "MyActor")
	FVector GetReceivedPosition() const;

private:
	void ReceiveData();

	FSocket* Socket;

	std::atomic<bool> bThreadRunning;
	mutable FCriticalSection ReceiveMutex;

	FVector ReceivedPosition;
};
