#pragma once

#define DEBUG_MESSAGE(key, length, color, message) if(GEngine) GEngine->AddOnScreenDebugMessage(key, length, color, message);
#define DEBUG_MSG(color, message) if(GEngine) GEngine->AddOnScreenDebugMessage(-1, 5, FColor::color, message);