#pragma once

namespace nsK2Engine {

    /// <summary>
    /// カスケードシャドウマップ法で使用する行列クラス。
    /// </summary>
    class CascadeShadowMapMatrix : public Noncopyable
    {
    public:
        /// <summary>
        /// ライトビュープロジェクションクロップ行列を計算する。
        /// </summary>
        /// <param name="lightDirection">ライトの方向</param>
        /// <param name="cascadeAreaRateTbl">カスケードエリア割合テーブル</param>
        /// <param name="sceneMaxPosition">シーンAABBの最大座標</param>
        /// <param name="sceneMinPosition">シーンAABBの最小座標</param>
        /// <param name="lightMaxHeight">光源の高さ</param>
        void CalcLightViewProjectionCropMatrix(
            Vector3 lightDirection,
            float cascadeAreaRateTbl[NUM_SHADOW_MAP],
            const Vector3& sceneMaxPosition,
            const Vector3& sceneMinPosition,
            float lightMaxHeight = 5000.0f
        );
        /// <summary>
        /// 計算されたライトビュープロジェクションクロップ行列を取得する。
        /// </summary>
        /// <param name="shadowMapNo">シャドウマップの番号</param>
        const Matrix& GetLightViewProjectionCropMatrix(int shadowMapNo) const
        {
            return m_lvpcMatrix[shadowMapNo];
        }
    private:
        Matrix m_lvpcMatrix[NUM_SHADOW_MAP]; // ライトビュークロップ行列
        float m_near[NUM_SHADOW_MAP];
        float m_far[NUM_SHADOW_MAP];
    };
}
