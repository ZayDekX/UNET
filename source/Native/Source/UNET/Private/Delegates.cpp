#include "Delegates.h"

DEFINE_LOG_CATEGORY(LogUNETManaged);

static void UNET::LogManaged(ELogVerbosity::Type level, TCHAR* message) {
    switch (level)
    {
    case ELogVerbosity::Fatal:
        UE_LOG(LogUNETManaged, Fatal, TEXT("%s"), message);
        break;
    case ELogVerbosity::Error:
        UE_LOG(LogUNETManaged, Error, TEXT("%s"), message);
        break;
    case ELogVerbosity::Warning:
        UE_LOG(LogUNETManaged, Warning, TEXT("%s"), message);
        break;
    case ELogVerbosity::Display:
        UE_LOG(LogUNETManaged, Display, TEXT("%s"), message);
        break;
    case ELogVerbosity::Log:
        UE_LOG(LogUNETManaged, Log, TEXT("%s"), message);
        break;
    case ELogVerbosity::Verbose:
        UE_LOG(LogUNETManaged, Verbose, TEXT("%s"), message);
        break;
    case ELogVerbosity::VeryVerbose:
        UE_LOG(LogUNETManaged, VeryVerbose, TEXT("%s"), message);
        break;

        //TODO: implement additional logging features
        //case ELogVerbosity::NumVerbosity:
        //    break;
        //case ELogVerbosity::VerbosityMask:
        //    break;
        //case ELogVerbosity::SetColor:
        //    break;
        //case ELogVerbosity::BreakOnLog:
        //    break;
    default:
        break;
    }
}