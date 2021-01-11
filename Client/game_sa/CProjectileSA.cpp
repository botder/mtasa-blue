/*****************************************************************************
 *
 *  PROJECT:     Multi Theft Auto v1.0
 *  LICENSE:     See LICENSE in the top level directory
 *  FILE:        game_sa/CProjectileSA.cpp
 *  PURPOSE:     Projectile entity
 *
 *  Multi Theft Auto is available from http://www.multitheftauto.com/
 *
 *****************************************************************************/

#include "StdInc.h"
#include "CProjectileSA.h"

/*
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
*/

void CProjectileSA::Destroy(bool blow) noexcept
{
}

bool CProjectileSA::CorrectPhysics() noexcept
{
    return true;
}
