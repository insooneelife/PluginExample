// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class PLUGINEXAMPLE_API IInferenceEngine
{
public:
    IInferenceEngine() {}
    virtual ~IInferenceEngine() {}

    virtual void RunTestcase() = 0;
    virtual IInferenceEngine* GetInferenceEngine() = 0;

    static IInferenceEngine* Get(const FName& SubsystemName = NAME_None);
};
