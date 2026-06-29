#pragma once
#include "Math/Vector/Vector3.hpp"
#include <string>
#ifndef BOOL3_H
#define BOOL3_H

enum class Bool3 {
    True,
    False,
    Unknown
};
enum class XPose {
    Standing = 0, // Em pé (parado ou andando normalmente)
    Crouching = 1,// Agachado
    Dashing = 2,  // Correndo
    Creeping = 3,    // Deitado (barriga no chão)
    Jumping = 11, // Pulo (no ar)
    Knocked = 8   // Caído (ferido/incapacitado)
};

class Player {
public:
    bool IsBot;
    bool IsKnown;
    bool IsDead;
    bool IsVisible;
    //por que deletuo isso manito

    int Team = -1;

    int Level;  // Nivel del jugador
    int CurrentShield;
    uint64_t ID;
    XPose Pose;
    float Distance;
    short Health;
    short Shield;
    short Gun;
    Bool3 IsTeam = Bool3::Unknown;

    std::string Name;
    uint32_t Address;
    Vector3 Head;       // Cabeça
    Vector3 Neck;       // Pescoço
    Vector3 RightShoulder; // Ombro direito
    Vector3 LeftShoulder;  // Ombro esquerdo
    Vector3 RightElbow;    // Cotovelo direito
    Vector3 LeftElbow;     // Cotovelo esquerdo
    Vector3 RightWrist;    // Pulso direito
    Vector3 LeftWrist;     // Pulso esquerdo
    Vector3 LeftHand;      // Mão esquerda
    Vector3 RightHand;     // Mão direita

    Vector3 Hip;          // Quadril
    Vector3 Groin;        // Virilha

    Vector3 RightAnkle;   // Tornozelo direito
    Vector3 LeftAnkle;    // Tornozelo esquerdo
    Vector3 LeftFoot;     // Pé esquerdo
    Vector3 RightFoot;    // Pé direito

    Vector3 Root;         // Raiz do corpo
    Vector3 RootBone;     // Raiz adicional do corpo
};

#endif
