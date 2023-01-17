// Fill out your copyright notice in the Description page of Project Settings.


#include "JsonLibrary.h"
#include "JsonUtilities/Public/JsonUtilities.h"
#include "Serialization/JsonReader.h"
#include "Misc/DateTime.h"

static const FString RootName("InventoryObject");

static const FString WritePath(FPaths::ProjectSavedDir() / "Json");
static const FString PlayerFileName("PlayerData.json");
static const FString FilepathFull(WritePath / PlayerFileName);

UJsonLibrary::UJsonLibrary()
{
}
//Temp
void UJsonLibrary::GenerateBaseFile()
{
	JsonObjectPtr root = MakeShareable(new FJsonObject());

	const FString SaveDate = FDateTime::UtcNow().ToString();
	root->SetStringField("SaveDate", SaveDate);

	root->SetStringField("Character", FString());

	TArray<TSharedPtr<FJsonValue>> InventoryItemObjects;
	root->SetArrayField("Items: ", InventoryItemObjects);

	TArray<TSharedPtr<FJsonValue>> inventoryItemCountArray;
	root->SetArrayField("Item Count: ", inventoryItemCountArray);

	WriteJsonFile(root, FilepathFull);
}

/// <summary>
/// 
/// </summary>
/// <param name="CharName"></param>
/// <param name="InventoryItems"></param>
/// <param name="InventoryCount"></param>
/// <returns></returns>
bool UJsonLibrary::WriteInventoryData(FString CharName, TArray<AActor*> InventoryItems, TArray<int> InventoryCount)
{
	TSharedPtr<FJsonObject> JsonRootObject = MakeShareable(new FJsonObject);
	//JsonObjectPtr JsonRootObject = MakeShareable(new FJsonObject);

	const FString SaveDate = FDateTime::UtcNow().ToString();
	JsonRootObject->SetStringField("SaveDate", SaveDate);
	JsonRootObject->SetStringField("Character", CharName);

	TArray<TSharedPtr<FJsonValue>> InventoryItemObjects;
	for (AActor* object : InventoryItems)
	{
		InventoryItemObjects.Add(MakeShareable(new FJsonValueString(object->GetFullName())));
	}
	JsonRootObject->SetArrayField("Items: ", InventoryItemObjects);

	TArray<TSharedPtr<FJsonValue>> inventoryItemCountArray;
	for (int i : InventoryCount)
	{
		inventoryItemCountArray.Add(MakeShareable(new FJsonValueNumber(i)));
	}
	JsonRootObject->SetArrayField("Item Count: ", inventoryItemCountArray);

	/// <summary>
	/// return value of an object
	/// </summary>
	/// <param name="JsonRootObject"> list of data of an object to store</param>
	/// <param name="FilepathFull"> store object to the filepath </param>
	/// <param name="true/false"> align data in json </param>
	/// <returns></returns>
	return WriteJsonFile(JsonRootObject, FilepathFull);
}

bool UJsonLibrary::ReadInvenotyData(FDateTime& SaveDate, FString& CharName, TArray<FString>& InventoryItems, TArray<int>& InventoryCount)
{
	JsonObjectPtr JsonRootObject = ReadJsonFile(FilepathFull);

	if (JsonRootObject)
	{
		FDateTime::Parse(JsonRootObject->GetStringField("SaveDate"), SaveDate);
		CharName = JsonRootObject->GetStringField("Character");

		for (TSharedPtr<FJsonValue> V : JsonRootObject->GetArrayField("Items: "))
		{
			//const FString fullname = V->AsString();
			InventoryItems.Add(V->AsString());
		}

		for (TSharedPtr<FJsonValue> V : JsonRootObject->GetArrayField("Item Count: "))
		{
			//const FString fullname = V->AsString();
			InventoryCount.Add(V->AsNumber());
		}
		return true;
	}
	return false;

}

/// <summary>
/// 
/// </summary>
/// <param name="PlayerDataInfo"></param>
/// <returns></returns>
bool UJsonLibrary::ReadPlayerData(TArray<FPlayerData>& OutPlayerData)
{
	// Read the file contents into a string
	FString FileString;
	if (!FFileHelper::LoadFileToString(FileString, *FilepathFull))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to load file %s"), *FilepathFull);
		return false;
	}

	// Parse the string into a JSON object
	TSharedPtr<FJsonObject> JsonObject = MakeShared<FJsonObject>();
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(FileString);
	if (!FJsonSerializer::Deserialize(Reader, JsonObject))
	{
		UE_LOG(LogTemp, Error, TEXT("Failed to parse JSON data from file %s"), *FilepathFull);
		return false;
	}

	// Extract the array of player data from the JSON object
	TArray<TSharedPtr<FJsonValue>> PlayerDataArray = JsonObject->GetArrayField("PlayerData");

	// Iterate through the array and extract the data for each player
	for (TSharedPtr<FJsonValue> PlayerDataValue : PlayerDataArray)
	{
		TSharedPtr<FJsonObject> PlayerDataObject = PlayerDataValue->AsObject();

		FPlayerData PlayerData;
		PlayerData.isActive = PlayerDataObject->GetBoolField("WasActive");
		PlayerData.UserName = PlayerDataObject->GetStringField("Name");
		PlayerData.UserNumber = PlayerDataObject->GetNumberField("Number");
		PlayerData.Health = PlayerDataObject->GetNumberField("Health");

		TArray<TSharedPtr<FJsonValue>> LocationsArray = PlayerDataObject->GetArrayField("PreviousLocations");
		for (TSharedPtr<FJsonValue> LocationValue : LocationsArray)
		{
			FVector Location;
			Location.InitFromString(LocationValue->AsString());
			PlayerData.Locations.Add(Location);
		}

		FVector CurrentLocation;
		CurrentLocation.InitFromString(PlayerDataObject->GetStringField("CurrentLocation"));
		PlayerData.CurrentLocation = CurrentLocation;

		OutPlayerData.Add(PlayerData);
	}

	return true;
}

bool UJsonLibrary::WritePlayerData(TArray<FPlayerData> PlayerDataInfo)
{
	//Root object
	TSharedRef<FJsonObject> Root = MakeShared<FJsonObject>();
	const FString SaveDate = FDateTime::UtcNow().ToString();
	Root->SetStringField("SaveDate", SaveDate);
	//Array of player data
	TArray<TSharedPtr<FJsonValue>> PlayersArray;

	for (FPlayerData Player : PlayerDataInfo)
	{
		//Player object
		TSharedRef<FJsonObject> PlayerObject = MakeShared<FJsonObject>();

		//Add player data to the object
		PlayerObject->SetBoolField("WasActive", Player.isActive);
		PlayerObject->SetStringField("Name", Player.UserName);
		PlayerObject->SetNumberField("Number", Player.UserNumber);
		PlayerObject->SetNumberField("Health", Player.Health);

		//Add player's previous locations to the object
		TArray<TSharedPtr<FJsonValue>> LocationsArray;
		for (FVector Location : Player.Locations)
		{
			//Convert the FVector to a string
			FString LocationString = Location.ToString();
			LocationsArray.Add(MakeShared<FJsonValueString>(LocationString));
		}
		PlayerObject->SetArrayField("PreviousLocations", LocationsArray);
		PlayerObject->SetStringField("CurrentLocation", Player.CurrentLocation.ToString());

		//Add the player object to the array
		PlayersArray.Add(MakeShared<FJsonValueObject>(PlayerObject));
	}

	// Add array of structs to MainJson
	Root->SetArrayField("PlayerData", PlayersArray);

	return WriteJsonFile(Root, FilepathFull);
}

/// <summary>
/// Serialize
/// </summary>
/// <param name="Object"></param>
/// <param name="FullPath"></param>
/// <param name="bCondensed"></param>
/// <returns></returns>
bool UJsonLibrary::WriteJsonFile(JsonObjectPtr Object, FString FullPath, bool bCondensed)
{
	FString OutputString;

	if (bCondensed)
	{
		TSharedPtr<TJsonWriter<TCHAR, TCondensedJsonPrintPolicy< TCHAR >>> Writer = TJsonWriterFactory<TCHAR, TCondensedJsonPrintPolicy< TCHAR >>::Create(&OutputString);
		FJsonSerializer::Serialize(Object.ToSharedRef(), Writer.ToSharedRef());
	}
	else
	{
		TSharedPtr<TJsonWriter<TCHAR, TPrettyJsonPrintPolicy< TCHAR >>> Writer = TJsonWriterFactory<>::Create(&OutputString);
		FJsonSerializer::Serialize(Object.ToSharedRef(), Writer.ToSharedRef());
	}
	return FFileHelper::SaveStringToFile(OutputString, *FullPath);
}

/// <summary>
/// Deserialize
/// </summary>
/// <param name="FullPath"></param>
/// <returns></returns>
JsonObjectPtr UJsonLibrary::ReadJsonFile(FString FullPath)
{
	/// <summary>
	/// FString to take in data that is in the text file with json file
	/// </summary>
	FString RawData;

	/// <summary>
	/// load the file into raw data
	/// </summary>
	bool bLoadFile = FFileHelper::LoadFileToString(RawData, *FullPath);

	/// <summary>
	/// Construct a new tshared pointer of a json object, pass in json reader
	/// </summary>
	JsonObjectPtr JsonRootObject = MakeShareable(new FJsonObject());

	if (bLoadFile)
	{
		/// <summary>
		/// Create the reader and pass in the raw data into string
		/// </summary>
		TSharedRef<TJsonReader<>> JsonReader = TJsonReaderFactory<>::Create(RawData);

		/// <summary>
		/// Deserialize file
		/// </summary>
		FJsonSerializer::Deserialize(JsonReader, JsonRootObject);
	}
	return JsonRootObject;
}


bool UJsonLibrary::GetProjectVersion(FString& ProjectVersion)
{
	const FString ProjectFile = FPaths::ProjectDir() / "Build.json";
	JsonObjectPtr Root = ReadJsonFile(ProjectFile);

	if (Root)
	{
		return Root->TryGetStringField("Version", ProjectVersion);
	}
	return false;
}