#pragma once
#include "CoreMinimal.h"

DECLARE_LOG_CATEGORY_EXTERN(Log_InteractCore, Log, All);

#if !UE_BUILD_SHIPPING

#define LOG(Format, ...) \
    UE_LOG(Log_InteractCore, Log, TEXT(Format), ##__VA_ARGS__)

#define LOG_WARNING(Format, ...) \
    UE_LOG(Log_InteractCore, Warning, TEXT(Format), ##__VA_ARGS__)

#define LOG_ERROR(Format, ...) \
    UE_LOG(Log_InteractCore, Error, TEXT(Format), ##__VA_ARGS__)

#define PRINT(Format, ...)                                     \
    do                                                         \
    {                                                          \
        if (GEngine)                                           \
        {                                                      \
            GEngine->AddOnScreenDebugMessage(                  \
                -1, 5.0f, FColor::Black,                       \
                FString::Printf(TEXT(Format), ##__VA_ARGS__)); \
        }                                                      \
    } while (0)

#else

#define LOG(...)
#define LOG_WARNING(...)
#define LOG_ERROR(...)
#define PRINT(...)

#endif