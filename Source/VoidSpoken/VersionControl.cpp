 // Fill out your copyright notice in the Description page of Project Settings.


#include "VersionControl.h"

void UVersionControl::NativeConstruct() {

    FString ProjectVersion;
    GConfig->GetString(
        TEXT("/Script/EngineSettings.GeneralProjectSettings"),
        TEXT("ProjectVersion"),
        ProjectVersion,
        GGameIni
    );

    VersionText->SetText(FText::FromString(FlatVerstionString + "-" + ProjectVersion));
}