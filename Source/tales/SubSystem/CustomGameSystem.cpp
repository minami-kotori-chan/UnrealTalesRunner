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

    // ���� �н��� �ٿ�� �ڽ� ���� ��������
    FBox BoundingBox = DungeonPath->GetComponentsBoundingBox(true);  // true�� ���� �ٿ�尡 �ƴ� ���� �ٿ�带 ������

    // �ٿ�� �ڽ��� �ּ�/�ִ� ��ǥ ����
    FVector MinPoint = BoundingBox.Min;
    FVector MaxPoint = BoundingBox.Max;

    // ������ ��ǥ ����
    StartX = MinPoint.X;
    EndX = MaxPoint.X;

    StartY = MinPoint.Y;
    EndY = MaxPoint.Y;

    StartZ = MinPoint.Z;
    EndZ = MaxPoint.Z;

    // �������� ���� ���� ����
    DungeonStartVector = MinPoint;
    DungeonEndVector = MaxPoint;

    //UE_LOG(LogTemp, Log, TEXT("Dungeon Path Initialized: Min(%f, %f, %f), Max(%f, %f, %f)"),MinPoint.X, MinPoint.Y, MinPoint.Z,MaxPoint.X, MaxPoint.Y, MaxPoint.Z);
}
