#pragma once

#include "CascadeShadowMapMatrix.h"


namespace nsK2Engine {
    class IRenderer;
    /// <summary>
    /// シャドウマップへの描画処理。
    /// </summary>
    class ShadowMapRender : public Noncopyable
    {
    public:
        /// <summary>
        /// 初期化。
        /// </summary>
        /// <param name="isSoftShadow">
        /// trueの場合、シャドウマップ法による影をソフトシャドウにします。
        /// ハードシャドウにしたい場合は、falseを指定してください。
        /// </param>
        void Init(bool isSoftShadow);

        /// <summary>
        /// 描画。
        /// </summary>
        /// <param name="rc">レンダリングコンテキスト</param>
        /// <param name="sceneMaxPosition">シーンのAABB最大座標</param>
        /// <param name="sceneMinPosition">シーンのAABB最小座標</param>
        void Render(
            RenderContext& rc,
            int ligNo,
            Vector3& lightDirection,
            std::vector< IRenderer* >& renderObjects,
            const Vector3& sceneMaxPosition,
            const Vector3& sceneMinPosition
        );
        /// <summary>
        /// シャドウマップを取得。
        /// </summary>
        /// <param name="areaNo">エリア番号</param>
        Texture& GetShadowMap(int areaNo)
        {
            if (m_isSoftShadow) {
                return m_blur[areaNo].GetBokeTexture();
            }
            return m_shadowMaps[areaNo].GetRenderTargetTexture();
        }
        /// <summary>
        /// ライトビュープロジェクション行列を取得。
        /// </summary>
        const Matrix& GetLVPMatrix(int areaNo) const
        {
            return m_cascadeShadowMapMatrix.GetLightViewProjectionCropMatrix(areaNo);
        }
        /// <summary>
        /// 光源の高さを設定する。
        /// 大きいほど広い範囲の影をカバーできる。
        /// </summary>
        void SetLightMaxHeight(float h) { m_lightMaxHeight = h; }
        float GetLightMaxHeight() const { return m_lightMaxHeight; }
        /// <summary>
        /// カスケードシャドウのエリア率を設定。
        /// ゲームカメラの近平面から遠平面までのエリアを、
        /// nearArea%は近影用の高解像度のシャドウマップに、
        /// middleArea%は中影用のシャドウマップに描画するか指定します。
        /// 例: nearArea=0.1(10%), middleArea=0.3(30%), farArea=1.0(100%)
        /// </summary>
        void SetCascadeNearAreaRates(float nearArea, float middleArea, float farArea)
        {
            // 近影エリアの範囲が中影エリアの範囲より小さくなっていなければ
            // 計算が失敗するので修正。
            middleArea = max(nearArea + 0.01f, middleArea);
            // 中影エリアの範囲が遠影エリアの範囲より小さくなっていなければ
            // 計算が失敗するので修正。
            farArea = max(middleArea + 0.01f, farArea);
            m_cascadeAreaRateArray[SHADOW_MAP_AREA_NEAR] = nearArea;
            m_cascadeAreaRateArray[SHADOW_MAP_AREA_MIDDLE] = middleArea;
            m_cascadeAreaRateArray[SHADOW_MAP_AREA_FAR] = farArea;
        }
    private:
        CascadeShadowMapMatrix  m_cascadeShadowMapMatrix;            // カスケードシャドウマップの行列計算オブジェクト
        RenderTarget            m_shadowMaps[NUM_SHADOW_MAP];        // シャドウマップ
        std::vector<IRenderer*> m_renderers;                         // シャドウマップへのレンダラーの配列。
        float m_cascadeAreaRateArray[NUM_SHADOW_MAP] = { 0.05f, 0.3f, 1.0f };
        float m_lightMaxHeight = 5000.0f;                            // 光源の高さ
        GaussianBlur m_blur[NUM_SHADOW_MAP];                         // シャドウマップにブラーをかける処理。ソフトシャドウを行う際に使います。
        bool  m_isSoftShadow = false;                                // ソフトシャドウ？
    };
}
