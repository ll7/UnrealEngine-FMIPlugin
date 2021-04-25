// Fill out your copyright notice in the Description page of Project Settings.

#include "FmuActorComponent.h"
#include "GameFramework/Actor.h"
#include "XmlFile.h"
#include "elzip.hpp"


// Sets default values for this component's properties
UFmuActorComponent::UFmuActorComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	FmuPath.FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() + "../PT2.fmu");
	// ...
}

void UFmuActorComponent::InitializeComponent()	{
	Super::InitializeComponent();
}

// Called when the game starts
void UFmuActorComponent::BeginPlay()
{
	Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("Initializing FMU component"));
	extractFmu();
	importFmuParameters();
	for (auto& Elem : mValRefMap)
{
    UE_LOG(LogTemp, Warning, TEXT("(%s, \"%d\")\n"), *Elem.Key, Elem.Value);
}

	UE_LOG(LogTemp, Warning, TEXT("%s %s %s %s %s"), *FString(mGuid.c_str()), *FString(mModelIdentifier.c_str()), *mFmuWorkingPath, *FString(mInstanceName.c_str()), *(FPaths::DiffDir()));

	mFmu = new fmikit::FMU2Slave(mGuid, mModelIdentifier, std::string(TCHAR_TO_UTF8(*mFmuWorkingPath)), mInstanceName);
	mFmu->instantiate(false);
	UE_LOG(LogTemp, Warning, TEXT("instantiate complete!"));

	mFmu->setupExperiment(true, mTolerance, mStartTime, true, mStopTime);
	UE_LOG(LogTemp, Warning, TEXT("setupExperiment complete!"));

	mFmu->enterInitializationMode();
	UE_LOG(LogTemp, Warning, TEXT("enterInitializationMode complete!"));

	mFmu->exitInitializationMode();
	UE_LOG(LogTemp, Warning, TEXT("exitInitializationMode complete!"));
	
	mLoaded = true;
}

bool UFmuActorComponent::extractFmu()	{

	if (FmuPath.FilePath.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Path to FMU %s is empty."), *(FmuPath.FilePath));
		return false;
	}
	mFmuPath = std::string(TCHAR_TO_UTF8(*FPaths::ConvertRelativePathToFull(FmuPath.FilePath)));
	mFmuWorkingPath = FPaths::GetBaseFilename(FPaths::ConvertRelativePathToFull(FmuPath.FilePath), false).Append("/");

	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();
	//TODO: Check if .fmu file and extracted Dir are the same and skip extraction
	FileManager.DeleteDirectoryRecursively(*mFmuWorkingPath);
	FileManager.CreateDirectory(*mFmuWorkingPath);

	UE_LOG(LogTemp, Display, TEXT("FMU Path: %s FMU Extraction Dir: %s Platform: %s"), UTF8_TO_TCHAR(mFmuPath.c_str()), *mFmuWorkingPath, *(UGameplayStatics::GetPlatformName()));
	
	if ( UGameplayStatics::GetPlatformName() == "LINUX")	{
		FString unzipCommandArgs = FString::Printf(TEXT("-uo %s -d %s"), *FString(mFmuPath.c_str()), *mFmuWorkingPath);
		UE_LOG(LogTemp, Warning, TEXT("Extraction command: %s"), *unzipCommandArgs);
		int32* OutReturnCode = new int32;
		FString* OutStdOut = new FString;
		FString* OutStdErr = new FString;
		FUnixPlatformProcess::ExecProcess(TEXT("/usr/bin/unzip"), *unzipCommandArgs, 0, 0, 0);
		UE_LOG(LogTemp, Warning, TEXT("Extraction command: %s"), OutStdOut, OutStdErr, OutReturnCode);
		delete OutReturnCode;
		delete OutStdOut;
		delete OutStdErr;
	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Platform is not LINUX, using elzip to extract FMU"));
		elz::extractZip(mFmuPath, std::string(TCHAR_TO_UTF8(*mFmuWorkingPath)));
	}
	
	
	return true;
}

void UFmuActorComponent::importFmuParameters()	{

		FString fXmlFile = mFmuWorkingPath;
		fXmlFile.Append(TEXT("modelDescription.xml"));

		UE_LOG(LogTemp, Display, TEXT("FMU modelDescription.xml Path: %s"), *fXmlFile);
		
		FXmlFile model(fXmlFile, EConstructMethod::ConstructFromFile);
		FXmlNode *root = model.GetRootNode();
		FXmlNode *defaultExperiment = root->FindChildNode("DefaultExperiment");
		FXmlNode *modelVariables = root->FindChildNode("ModelVariables");
		TArray<FXmlNode*> nodes = modelVariables->GetChildrenNodes();
		for (FXmlNode* node : nodes)
		{
			FString key = node->GetAttribute("name");
			int value = FCString::Atoi(*node->GetAttribute("valueReference"));
			UE_LOG(LogTemp, Display, TEXT("Found Model Var: %s : %d"), *key, value);
			mValRefMap.Add(key, value);
		}
		mGuid = TCHAR_TO_UTF8(*root->GetAttribute("guid"));
		mModelIdentifier = TCHAR_TO_UTF8(*root->GetAttribute("modelName"));;
		mInstanceName = "instance";
		mStartTime = FCString::Atof(*defaultExperiment->GetAttribute("startTime"));
		mStopTime = FCString::Atof(*defaultExperiment->GetAttribute("stopTime")) * StopTimeMultiplier;
		mStepSize = 0.1;
		mTolerance = FCString::Atof(*defaultExperiment->GetAttribute("tolerance"));;
		mTimeLast = mStartTime;
		mTimeNow = mStartTime;
}

void UFmuActorComponent::EndPlay
(
    const EEndPlayReason::Type EndPlayReason
) {
	delete mFmu;
	Super::EndPlay(EndPlayReason);
}


// Called every frame
void UFmuActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!mLoaded)
		return;

	mTimeNow += DeltaTime;
	if (!(mTimeNow > mTimeLast + mStepSize / SpeedMultiplier))
		return;

	if (mTimeLast >= mStopTime / SpeedMultiplier)
		return;

    mTimeLast += mStepSize / SpeedMultiplier;
	mFmu->doStep(mStepSize);
}

float UFmuActorComponent::getReal(FString Name) {
    return mFmu->getReal(mValRefMap[Name]);
}

void UFmuActorComponent::setReal(FString Name, float Value) {
	mFmu->setReal(mValRefMap[Name], Value);
}

bool UFmuActorComponent::getBoolean(FString Name) {
    return mFmu->getBoolean(mValRefMap[Name]);
}

void UFmuActorComponent::setBoolean(FString Name, bool Value) {
	mFmu->setBoolean(mValRefMap[Name], Value);
}

int UFmuActorComponent::getInteger(FString Name) {
    return mFmu->getInteger(mValRefMap[Name]);
}

void UFmuActorComponent::setInteger(FString Name, int Value) {
	mFmu->setInteger(mValRefMap[Name], Value);
}

FString UFmuActorComponent::getString(FString Name) {
    return FString(mFmu->getString(mValRefMap[Name]).c_str());
}

void UFmuActorComponent::setString(FString Name, FString Value) {
	mFmu->setString(mValRefMap[Name], std::string(TCHAR_TO_UTF8(*Value)));
}

