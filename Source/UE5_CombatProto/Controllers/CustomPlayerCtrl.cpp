// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomPlayerCtrl.h"


ACustomPlayerCtrl::ACustomPlayerCtrl()
{
    TeamId = 1;
}

FGenericTeamId ACustomPlayerCtrl::GetGenericTeamId() const
{
    return TeamId;
}
