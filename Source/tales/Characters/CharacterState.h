#pragma once

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	ECS_Idle UMETA(DisplayName = "Character Idle"),
	ECS_Running UMETA(DisplayName = "Character Running"),
	ECS_Jumping UMETA(DisplayName = "Character Jumping"),
	ECS_DoubleJumping UMETA(DisplayName = "Character DoubleJumping"),
	ECS_Stun UMETA(DisplayName = "Character Stun"),
	ECS_KnockBack UMETA(DisplayName = "Character KnockBack"),
	ECS_BurnOut UMETA(DisplayName = "Character BurnOut"),
};
UENUM(BlueprintType)
enum class ECharacterAngry : uint8
{
	ECS_Normal UMETA(DisplayName = "Character Normal"),
	ECS_Angry UMETA(DisplayName = "Character Angry"),
	ECS_Charge UMETA(DisplayName = "Character Charge"),
};

UENUM(BlueprintType)
enum class ECharacterDash : uint8
{
	ECS_Normal UMETA(DisplayName = "Character Normal"),
	ECS_Dash UMETA(DisplayName = "Character Dash"),
};

UENUM(BlueprintType)
enum class ECharacterStartFalling : uint8
{
	ECS_Normal UMETA(DisplayName = "Character Normal"),
	ECS_StartFalling UMETA(DisplayName = "Character StartFalling"),
};