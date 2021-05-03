// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Containers/Map.h"
#include "FMU2.h"
#include "Engine/EngineTypes.h"
#include "CoreMinimal.h"
#include "Misc/Paths.h"
#include "Components/ActorComponent.h"
#include "GenericPlatform/GenericPlatformFile.h" 
#include "HAL/FileManagerGeneric.h" 
#include "Kismet/GameplayStatics.h"
#include "FmuActorComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FMIKIT_API UFmuActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFmuActorComponent();

	UFUNCTION(BlueprintCallable)
		float getReal(FString Name);
	UFUNCTION(BlueprintCallable)
		void setReal(FString Name, float Value);
	
	UFUNCTION(BlueprintCallable)
		void setBoolean(FString Name, bool Value);
	UFUNCTION(BlueprintCallable)
		bool getBoolean(FString Name);

	UFUNCTION(BlueprintCallable)
		void setInteger(FString Name, int Value);
	UFUNCTION(BlueprintCallable)
		int getInteger(FString Name);

	UFUNCTION(BlueprintCallable)
		void setString(FString Name, FString Value);
	UFUNCTION(BlueprintCallable)
		FString getString(FString Name);
protected:
	
	virtual void InitializeComponent() override;
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent) override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float SpeedMultiplier = 1.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		FVector DistanceMultiplier = {1.f,1.f,1.f};
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Settings")
		float StopTimeMultiplier = 1.f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
        FFilePath FmuPath;
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "MapsAndSets")
        TMap<FString, int> FmuVariables;

private:
	static FString extractFmu(FString sourcePath);
	static bool extract(FString *sourcePath, FString *targetPath);
	void importFmuParameters();

	TSharedPtr<fmikit::FMU2Slave, ESPMode::ThreadSafe> mFmu;

	FString mFmuExtractPath;

	std::string mFmuPath,  mGuid, mModelIdentifier, mInstanceName;
	fmi2Real mStartTime;
	fmi2Real mStopTime;
	fmi2Real mStepSize;
	fmi2Real mTolerance;
	fmi2Real mTimeLast;
	fmi2Real mTimeNow;
	bool mLoaded = false;
};
