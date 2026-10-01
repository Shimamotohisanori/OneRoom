#include "k2EnginePreCompile.h"
#include "CascadeShadowMapMatrix.h"

namespace nsK2Engine {
    void CascadeShadowMapMatrix::CalcLightViewProjectionCropMatrix(
        Vector3 lightDirection,
        float cascadeAreaRateTbl[NUM_SHADOW_MAP],
        const Vector3& sceneMaxPosition,
        const Vector3& sceneMinPosition,
        float lightMaxHeight
    )
    {
        // �ő�t�@�[��150m
        float maxFar = g_camera3D->GetFar() * cascadeAreaRateTbl[NUM_SHADOW_MAP - 1];
        //�r���[�s����v�Z����B
        Matrix viewMatrix;
        Vector3 lightTarget = g_camera3D->GetPosition();
        Vector3 lightPos = lightTarget;
        lightPos += (lightDirection) * (lightMaxHeight / lightDirection.y);
        //�������ݒ�
        if (fabsf(lightDirection.y) > 0.9999f) {
            //�قڐ^��A�^���������Ă���
            viewMatrix.MakeLookAt(lightPos, lightTarget, g_vec3AxisX);
        }
        else {
            viewMatrix.MakeLookAt(lightPos, lightTarget, g_vec3AxisY);
        }
        float lightToSceneDist = lightMaxHeight / max(fabsf(lightDirection.y), 0.01f);
        float shadowFar = lightToSceneDist * 3.0f;
        Matrix projMatrix;
        projMatrix.MakeOrthoProjectionMatrix(
            5000.0f,
            5000.0f,
            1.0f,
            shadowFar
        );

        // �����G���A�̍ő�[�x�l���`����
        float cascadeAreaTbl[NUM_SHADOW_MAP] = {
            shadowFar * cascadeAreaRateTbl[SHADOW_MAP_AREA_NEAR],     // �߉e���f���ő�[�x�l
            shadowFar * cascadeAreaRateTbl[SHADOW_MAP_AREA_MIDDLE],   // ���e���f���ő�[�x�l
            shadowFar * cascadeAreaRateTbl[SHADOW_MAP_AREA_FAR] ,     // ���e���f���ő�[�x�l�B
        };
        // �J�����̑O�����A�E�����A����������߂�
        // �O�����ƉE�����͂��łɌv�Z�ς݂Ȃ̂ŁA��������������Ă���
        const auto& cameraForward = g_camera3D->GetForward();
        const auto& cameraRight = g_camera3D->GetRight();

        // �J�����̏�����͑O�����ƉE�����̊O�ςŋ��߂�
        Vector3 cameraUp;
        cameraUp.Cross(cameraForward, cameraRight);
        // nearDepth�̓G���A�̍ŏ��[�x�l��\��
            // ��ԋ߂��G���A�̍ŏ��[�x�l�̓J�����̃j�A�N���b�v
        float nearDepth = g_camera3D->GetNear();
        for (int areaNo = 0; areaNo < NUM_SHADOW_MAP; areaNo++)
        {
            // step-7 �G���A�����鎋����̂W���_�����߂�
            // �G���A�̋ߕ��ʂ̒��S����̏�ʁA���ʂ܂ł̋��������߂�
            float nearY = tanf(g_camera3D->GetViewAngle() * 0.5f) * nearDepth;

            // �G���A�̋ߕ��ʂ̒��S����̉E�ʁA���ʂ܂ł̋��������߂�
            float nearX = nearY * g_camera3D->GetAspect();

            // �G���A�̉����ʂ̒��S����̏�ʁA���ʂ܂ł̋��������߂�
            float farY = tanf(g_camera3D->GetViewAngle() * 0.5f) * cascadeAreaTbl[areaNo];

            // �G���A�̉����ʂ̒��S����̉E�ʁA���ʂ܂ł̋��������߂�
            float farX = farY * g_camera3D->GetAspect();

            // �G���A�̋ߕ��ʂ̒��S���W�����߂�
            Vector3 nearPos = g_camera3D->GetPosition() + cameraForward * nearDepth;

            // �G���A�̉����ʂ̒��S���W�����߂�
            Vector3 farPos = g_camera3D->GetPosition() + cameraForward * cascadeAreaTbl[areaNo];

            // 8���_�����߂�
            Vector3 vertex[8];

            // �ߕ��ʂ̉E��̒��_
            vertex[0] = nearPos + cameraUp * nearY + cameraRight * nearX;

            // �ߕ��ʂ̍���̒��_
            vertex[1] = nearPos + cameraUp * nearY + cameraRight * -nearX;

            // �ߕ��ʂ̉E���̒��_
            vertex[2] = nearPos + cameraUp * -nearY + cameraRight * nearX;

            // �ߕ��ʂ̍����̒��_
            vertex[3] = nearPos + cameraUp * -nearY + cameraRight * -nearX;

            // �����ʂ̉E��̒��_
            vertex[4] = farPos + cameraUp * farY + cameraRight * farX;

            // �����ʂ̍���̒��_
            vertex[5] = farPos + cameraUp * farY + cameraRight * -farX;

            // �����ʂ̉E���̒��_
            vertex[6] = farPos + cameraUp * -farY + cameraRight * farX;

            // �����ʂ̍����̒��_
            vertex[7] = farPos + cameraUp * -farY + cameraRight * -farX;

            // 8���_���J������Ԃɕϊ����āA�ߕ��ʂƉ����ʂ����߂�B
            float nearZ = FLT_MAX, farZ = -FLT_MAX;
            for (auto v : vertex) {
                viewMatrix.Apply(v);
                nearZ = max(0.0f, min(v.z, nearZ));
                farZ = max(v.z, farZ);
            }

            // 8���_�����C�g�r���[�v���W�F�N�V������Ԃɕϊ����āA8���_�̍ő�l�A�ŏ��l�����߂�
            Matrix lvpMatrix = viewMatrix * projMatrix;
            Vector3 vMax, vMin;
            vMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };
            vMin = { FLT_MAX,  FLT_MAX,  FLT_MAX };
            for (auto v : vertex)
            {
                lvpMatrix.Apply(v);
                vMax.Max(v);
                vMin.Min(v);
            }

            // step-9 �N���b�v�s������߂�
            float xScale = 2.0f / (vMax.x - vMin.x);
            float yScale = 2.0f / (vMax.y - vMin.y);
            float xOffset = (vMax.x + vMin.x) * -0.5f * xScale;
            float yOffset = (vMax.y + vMin.y) * -0.5f * yScale;
            Matrix clopMatrix;
            clopMatrix.m[0][0] = xScale;
            clopMatrix.m[1][1] = yScale;
            clopMatrix.m[3][0] = xOffset;
            clopMatrix.m[3][1] = yOffset;

            // ���C�g�r���[�v���W�F�N�V�����s��ɃN���b�v�s�����Z����
            m_lvpcMatrix[areaNo] = lvpMatrix * clopMatrix;

            // ���̃G���A�̋ߕ��ʂ܂ł̋�����������
            nearDepth = cascadeAreaTbl[areaNo];
        }
    };
}