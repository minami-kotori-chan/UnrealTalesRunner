// Fill out your copyright notice in the Description page of Project Settings.


#include "RollingAnimInstance.h"
#include "../Characters/Runner.h"

void URollingAnimInstance::AnimNotify_EndRolling()
{
    USkeletalMeshComponent* MeshComp = GetSkelMeshComponent();

    if (MeshComp && MeshComp->GetOwner())
    {
        if (ARunner* Runner = Cast<ARunner>(MeshComp->GetOwner()))
        {
            Runner->ChangeRollingAnimationCanPlay();
        }
    }
}
