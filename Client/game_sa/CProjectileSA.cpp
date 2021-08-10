/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CProjectileSA.cpp
 *  PURPOSE:     Source file for projectile entity class
 *
 *  Multi Theft Auto is available from https://multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CProjectileSA.h"

CProjectileSA::CProjectileSA() : CObjectSA(nullptr)
{
}

CProjectileSA::~CProjectileSA()
{
    BeingDeleted = true;
    Destroy();
}

void CProjectileSA::Destroy(bool bBlow)
{
    if (m_pInterface && !m_isDestroyed)
    {
        m_isDestroyed = true;

        if (bBlow)
            pGame->GetProjectileInfo()->RemoveProjectile(m_projectileInfo, this);
        else
            pGame->GetProjectileInfo()->RemoveIfThisIsAProjectile(this);

        m_pInterface = nullptr;
    }
}

// Corrects errors in the physics engine that cause projectiles to be far away from the objects they attached to
// issue #8122
bool CProjectileSA::CorrectPhysics()
{
    // make sure we have an interface for our bomb/satchel
    CPhysicalSAInterface* pInterface = static_cast<CPhysicalSAInterface*>(m_pInterface);
    // make sure we have an interface

    if (pInterface != NULL)
    {
        // make sure we have an attached entity
        if (pInterface->m_pAttachedEntity)
        {
            // get our position
            CVector vecStart = *GetPosition();
            // get the entity we collided with
            CEntitySAInterface* pCollidedWithInterface = pInterface->m_pAttachedEntity;
            // get our end position by projecting forward a few velocities more
            CVector vecEnd = vecStart - (pInterface->m_vecCollisionImpactVelocity * 3);

            // process forward another 1 unit
            if (pGame->GetWorld()->CalculateImpactPosition(vecStart, vecEnd))
            {
                // setup some variables
                CVector vecRotation;
                CVector vecTemp;
                CVector vecCollisionOffset;
                // get our current offset ( we want the rotation! )
                GetAttachedOffsets(vecTemp, vecRotation);

                // create a matrix variable
                CMatrix attachedToMatrix;
                if (pCollidedWithInterface->Placeable.matrix != NULL)
                {
                    // get our matrix
                    pCollidedWithInterface->Placeable.matrix->ConvertToMatrix(attachedToMatrix);
                }
                else
                {
                    // get our matrix
                    attachedToMatrix = CMatrix(pCollidedWithInterface->Placeable.m_transform.m_translate);
                }

                // transform our matrix into local (attached) space
                CVector vecPosition = attachedToMatrix.Inverse().TransformVector(vecEnd);

                // offset by enough that it isn't sticking inside anything
                vecPosition += attachedToMatrix.Inverse() * (pInterface->m_vecCollisionImpactVelocity * CVector(0.2f, 0.2f, 0.3f));

                // set our attached offsets
                SetAttachedOffsets(vecPosition, vecRotation);
            }
            return true;
        }
    }
    return false;
}

void CProjectileSA::SetProjectileInterface(CProjectileSAInterface* projectile)
{
    m_pInterface = projectile;
    m_isDestroyed = (projectile == nullptr);

    Reset();

    if (m_pInterface)
    {
        m_pInterface->bDontStream = true;
        m_pInterface->bRemoveFromWorld = false;
    }
}
