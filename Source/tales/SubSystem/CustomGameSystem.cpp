// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomGameSystem.h"
#include "../Actors/DungeonPath.h"

void UCustomGameSystem::InitDungeonPathData(ADungeonPath* DungeonPath)
{
    if (!DungeonPath)
    {
        UE_LOG(LogTemp, Warning, TEXT("InitDungeonPathData: DungeonPath is nullptr!"));
        return;
    }

    // 던전 패스의 바운딩 박스 정보 가져오기
    FBox BoundingBox = DungeonPath->GetComponentsBoundingBox(true);  // true는 로컬 바운드가 아닌 월드 바운드를 가져옴

    // 바운딩 박스의 최소/최대 좌표 저장
    FVector MinPoint = BoundingBox.Min;
    FVector MaxPoint = BoundingBox.Max;

    // 구간별 좌표 저장
    StartX = MinPoint.X;
    EndX = MaxPoint.X;

    StartY = MinPoint.Y;
    EndY = MaxPoint.Y;

    StartZ = MinPoint.Z;
    EndZ = MaxPoint.Z;

    // 시작점과 끝점 벡터 저장
    DungeonStartVector = MinPoint;
    DungeonEndVector = MaxPoint;

    //UE_LOG(LogTemp, Log, TEXT("Dungeon Path Initialized: Min(%f, %f, %f), Max(%f, %f, %f)"),MinPoint.X, MinPoint.Y, MinPoint.Z,MaxPoint.X, MaxPoint.Y, MaxPoint.Z);
}
