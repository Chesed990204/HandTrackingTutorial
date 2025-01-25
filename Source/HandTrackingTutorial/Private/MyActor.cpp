#include "MyActor.h"
#include "Async/Async.h"

// Sets default values
AMyActor::AMyActor() : vec(0)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AMyActor::BeginPlay()
{
	Super::BeginPlay();

	InitializeSocket();

	//// Start the data receiving thread
	//AsyncTask(ENamedThreads::AnyBackgroundThreadNormalTask, [this]()
	//{
	//	while (true)
	//	{
	//		ReceiveData();
	//		FPlatformProcess::Sleep(0.001f); // Sleep for 1ms to achieve ~1kHz
	//	}
	//});

	UE_LOG(LogTemp, Log, TEXT("MyActor BeginPlay"));
}

void AMyActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (Socket)
	{
		Socket->Close();
		ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->DestroySocket(Socket);
		Socket = nullptr;
	}

	Super::EndPlay(EndPlayReason);
}

// Called every frame
void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Other Tick related code
	ReceiveData();
}

void AMyActor::InitializeSocket()
{
	// Create a UDP socket
	Socket = FUdpSocketBuilder(TEXT("UDPSocket"))
		.AsNonBlocking()
		.AsReusable()
		.BoundToAddress(FIPv4Address::Any)
		.BoundToPort(23456)
		.WithReceiveBufferSize(1024);

	// Check if the socket was created successfully
	if (!Socket)
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to create UDP socket"));
		return;
	}
}

void AMyActor::ReceiveData()
{
	if (!Socket)
	{
		return;
	}

	TArray<uint8> ReceivedData;
	uint32 Size;
	while (Socket->HasPendingData(Size))
	{
		ReceivedData.SetNumUninitialized(FMath::Min(Size, 65507u));

		int32 Read = 0;
		Socket->Recv(ReceivedData.GetData(), ReceivedData.Num(), Read);

		UE_LOG(LogTemp, Log, TEXT("Received %d bytes"), Read);

		// packet consist of consecutive 3 doubles
		if (Read >= sizeof(double) * 3)
		{
			FMemoryReader Reader(ReceivedData);
			Reader.SetByteSwapping(true); // handle network endianness

			double Value1, Value2, Value3;

			Reader << Value1;
			Reader << Value2;
			Reader << Value3;

			// Process the values
			UE_LOG(LogTemp, Log, TEXT("Received values: %f, %f, %f"), Value1, Value2, Value3);

			// meter to centimeter, and reverse y axis
			vec = FVector(100 * Value1, -100 * Value2, 100 * Value3);
		}
	}
}


void AMyActor::SendData(const FVector& Data, const int32& Mode)
{
    if (!Socket)
    {
        return;
    }

    TArray<uint8> SendData;
    FMemoryWriter Writer(SendData);
    Writer.SetByteSwapping(true); // handle network endianness

    double Value1 = Data.X / 100.0;
    double Value2 = -Data.Y / 100.0;
    double Value3 = Data.Z / 100.0;

    Writer << Value1;
    Writer << Value2;
    Writer << Value3;

    // Ensure the array has enough space for the mode data
    SendData.SetNum(28);

    SendData[24] = 0;
    SendData[25] = 0;
    SendData[26] = 0;
    SendData[27] = (uint8)Mode;

    int32 BytesSent = 0;

    // Create destination address
    TSharedRef<FInternetAddr> DestinationAddr = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
    
	FIPv4Address ip(192, 168, 0, 6);	// to client PC (ROS2)

    DestinationAddr->SetIp(ip.Value);
    DestinationAddr->SetPort(12345);

    Socket->SendTo(SendData.GetData(), SendData.Num(), BytesSent, *DestinationAddr);

    if (BytesSent != SendData.Num())
    {
        UE_LOG(LogTemp, Warning, TEXT("Failed to send all data"));
    }
}
