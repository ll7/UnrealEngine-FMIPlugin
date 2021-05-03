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
	bWantsInitializeComponent = true;
	PrimaryComponentTick.bCanEverTick = true;
	FmuPath.FilePath = FPaths::ConvertRelativePathToFull(FPaths::ProjectDir() + "../PT2.fmu");
	// ...
}

void UFmuActorComponent::PostEditChangeProperty(FPropertyChangedEvent &PropertyChangedEvent)	{
	UE_LOG(LogTemp, Warning, TEXT("%s"), *(PropertyChangedEvent.MemberProperty->GetNameCPP()));
	
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
    if (PropertyName == GET_MEMBER_NAME_CHECKED(UFmuActorComponent, FmuPath))
     {
		UE_LOG(LogTemp, Display, TEXT("FmuPath changed: %s"), *(PropertyChangedEvent.MemberProperty->GetNameCPP()));
        mFmuExtractPath = extractFmu(FmuPath.FilePath);
		UE_LOG(LogTemp, Display, TEXT("Extracted to: %s"), *mFmuExtractPath);
		importFmuParameters();
     }
    Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UFmuActorComponent::InitializeComponent()	{
	Super::InitializeComponent();
	UE_LOG(LogTemp, Warning, TEXT("InitializeComponenent called"));
	mFmuExtractPath = extractFmu(FmuPath.FilePath);
	importFmuParameters();
}

// Called when the game starts
void UFmuActorComponent::BeginPlay()
{
	Super::BeginPlay();

	for (auto& Elem : FmuVariables)
{
    UE_LOG(LogTemp, Warning, TEXT("(%s, \"%d\")\n"), *Elem.Key, Elem.Value);
}
	if (overrideTolerance)
		mTolerance = simulationTolerance;

	UE_LOG(LogTemp, Warning, TEXT("%s %s %s %s %s"), *FString(mGuid.c_str()), *FString(mModelIdentifier.c_str()), *mFmuExtractPath, *FString(mInstanceName.c_str()), *(FPaths::DiffDir()));

	mFmu = MakeShared<fmikit::FMU2Slave, ESPMode::ThreadSafe>(mGuid, mModelIdentifier, std::string(TCHAR_TO_UTF8(*mFmuExtractPath)), mInstanceName);
	mFmu->instantiate(false);
	UE_LOG(LogTemp, Warning, TEXT("instantiate complete!"));

	mFmu->setupExperiment(true, mTolerance, mStartTime, finiteSimulation, mStopTime);
	UE_LOG(LogTemp, Warning, TEXT("setupExperiment complete!"));

	mFmu->enterInitializationMode();
	UE_LOG(LogTemp, Warning, TEXT("enterInitializationMode complete!"));

	mFmu->exitInitializationMode();
	UE_LOG(LogTemp, Warning, TEXT("exitInitializationMode complete!"));
	
	mLoaded = true;
}

FString UFmuActorComponent::extractFmu(FString sourcePath)	{
	FString tempFmuPath = FPaths::ConvertRelativePathToFull(*sourcePath);
	FString FmuParentPath = FPaths::GetPath(tempFmuPath);
	IPlatformFile& PlatformFileManager = FPlatformFileManager::Get().GetPlatformFile();

	FDateTime modifiedDate = PlatformFileManager.GetTimeStamp(*tempFmuPath);

	FString extractDir = FPaths::SetExtension(tempFmuPath, FString::Printf(TEXT("%lld"), modifiedDate.ToUnixTimestamp()));
	
	if (PlatformFileManager.DirectoryExists(*extractDir))	{
		return extractDir;
	}

	TArray<FString> foundFolders;

	FFileManagerGeneric FileMgr;
	FString wildcard = FString::Printf(TEXT("%s.*"), *FPaths::GetBaseFilename(*tempFmuPath));
 	FString search_path(FPaths::Combine(FmuParentPath, *wildcard));
	UE_LOG(LogTemp, Error, TEXT("SearchPath: %s"), *search_path);
	FileMgr.FindFiles(foundFolders, *search_path, false, true);

	for (FString folder : foundFolders)	{
		FString folderPath = FmuParentPath;
		folderPath.PathAppend(*folder, folder.Len());

		UE_LOG(LogTemp, Warning, TEXT("FoundFolder: %s"), *folderPath);
		PlatformFileManager.DeleteDirectoryRecursively(*folderPath);
	}

	extract(&tempFmuPath, &extractDir);
	return extractDir;
}

bool UFmuActorComponent::extract(FString *sourcePath, FString *targetPath)	{

	if (sourcePath->IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("Path to FMU %s is empty."), sourcePath);
		return false;
	}
	auto tempFmuPath = FPaths::ConvertRelativePathToFull(*sourcePath);
	auto tempTargetPath = FPaths::ConvertRelativePathToFull(*targetPath);

	IPlatformFile& FileManager = FPlatformFileManager::Get().GetPlatformFile();

	FileManager.DeleteDirectoryRecursively(*tempTargetPath);
	FileManager.CreateDirectory(*tempTargetPath);

	UE_LOG(LogTemp, Display, TEXT("FMU Path: %s FMU Extraction Dir: %s Platform: %s"), *tempFmuPath, *tempTargetPath, *(UGameplayStatics::GetPlatformName()));
	
	if ( UGameplayStatics::GetPlatformName() == "LINUX")	{
		FString unzipCommandArgs = FString::Printf(TEXT("-uo %s -d %s"), *tempFmuPath, *tempTargetPath);
		UE_LOG(LogTemp, Warning, TEXT("Extraction command: %s"), *unzipCommandArgs);

		FUnixPlatformProcess::ExecProcess(TEXT("/usr/bin/unzip"), *unzipCommandArgs, 0, 0, 0);

	}
	else {
		UE_LOG(LogTemp, Warning, TEXT("Platform is not LINUX, using elzip to extract FMU"));
		elz::extractZip(std::string(TCHAR_TO_UTF8(*tempFmuPath)), std::string(TCHAR_TO_UTF8(*tempTargetPath)));
	}
	
	return true;
}

void UFmuActorComponent::importFmuParameters()	{

		FString fXmlFile = mFmuExtractPath;
		fXmlFile = FPaths::Combine(mFmuExtractPath, TEXT("modelDescription.xml"));

		IPlatformFile &FileManager = FPlatformFileManager::Get().GetPlatformFile();

		UE_LOG(LogTemp, Display, TEXT("FMU modelDescription.xml Path: %s"), *fXmlFile);
		if (!FileManager.FileExists(*fXmlFile))	{
			UE_LOG(LogTemp, Error, TEXT("No modelDesctription.xml found"));
			return;
		}
		
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
			FmuVariables.Add(key, value);
		}
		mGuid = TCHAR_TO_UTF8(*root->GetAttribute("guid"));
		mModelIdentifier = TCHAR_TO_UTF8(*root->GetAttribute("modelName"));;
		mInstanceName = "instance";
		mStartTime = FCString::Atof(*defaultExperiment->GetAttribute("startTime"));
		mStopTime = FCString::Atof(*defaultExperiment->GetAttribute("stopTime")) * StopTimeMultiplier;
		mTolerance = FCString::Atof(*defaultExperiment->GetAttribute("tolerance"));;
		mTimeLastTick = mStartTime;
		mTimeNow = mStartTime;
}

void UFmuActorComponent::EndPlay
(
    const EEndPlayReason::Type EndPlayReason
) {
	mFmu.Reset();
	Super::EndPlay(EndPlayReason);
}


// Called every frame
void UFmuActorComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (!mLoaded)
		return;
	
	mTimeNow += DeltaTime;
	if (finiteSimulation && mTimeLastTick >= mStopTime)
		return;

	if (staticTicking)	{
		if (!(mTimeNow > mTimeLastTick + stepSize))
			return;
		mTimeLastTick += stepSize;
		mFmu->doStep(stepSize);
	} else {
		mTimeLastTick += DeltaTime;
		mFmu->doStep(DeltaTime);
	}
}

float UFmuActorComponent::getReal(FString Name) {
    return mFmu->getReal(FmuVariables[Name]);
}

void UFmuActorComponent::setReal(FString Name, float Value) {
	mFmu->setReal(FmuVariables[Name], Value);
}

bool UFmuActorComponent::getBoolean(FString Name) {
    return mFmu->getBoolean(FmuVariables[Name]);
}

void UFmuActorComponent::setBoolean(FString Name, bool Value) {
	mFmu->setBoolean(FmuVariables[Name], Value);
}

int UFmuActorComponent::getInteger(FString Name) {
    return mFmu->getInteger(FmuVariables[Name]);
}

void UFmuActorComponent::setInteger(FString Name, int Value) {
	mFmu->setInteger(FmuVariables[Name], Value);
}

FString UFmuActorComponent::getString(FString Name) {
    return FString(mFmu->getString(FmuVariables[Name]).c_str());
}

void UFmuActorComponent::setString(FString Name, FString Value) {
	mFmu->setString(FmuVariables[Name], std::string(TCHAR_TO_UTF8(*Value)));
}

