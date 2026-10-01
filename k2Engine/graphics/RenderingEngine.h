#pragma once

#include "MyRenderer.h"
#include "graphics/preRender/ShadowMapRender.h"
#include "graphics/postEffect/PostEffect.h"
#include "graphics/light/SceneLight.h"
#include "graphics/preRender/LightCulling.h"
#include "geometry/SceneGeometryData.h"
#include "graphics/light/VolumeLightRender.h"

namespace nsK2Engine {

    /// <summary>
    /// レンダリングエンジン。
    /// </summary>
    /// <remark>
    /// K2Engineの1フレームのレンダリングの流れは次のようになっています。
    /// 1.ComputeAnimatedVertex
    ///     ワールド空間に変換済みの頂点を計算する。
    ///     以降のモデル描画パスでは、このパスで計算された頂点を使い回す。
    ///     このパスでインスタンシング描画を行うモデルの頂点バッファは1つの大きなバッファにまとめられる。
    /// 2. シャドウマップの作成
    ///     カスケードシャドウ法を利用して4つのライトからのシャドウマップを作成する。
    /// 3. ZPrepass
    ///     ライトカリングのためのZPrepass。
    /// 4. ライトカリング
    ///     タイルごとのポイントライトとスポットライトの番号リストを作成。
    /// 5. G-Bufferへのレンダリング
    ///     アルベド＆深度値、法線、メタリック＆シャドウ＆Smoothの3つ。
    /// 6. ライトレーシングで影込み画像を作成する。
    /// 7. ディファードライティング
    ///     ディズニーベースのPBRライティングを行う。
    /// 8. フォワードレンダリング
    ///     透明オブジェクト等の簡易シェーディングの描画パス。
    /// 9. ポストエフェクト
    /// 10. 2D描画
    /// 11. 最終画像をフレームバッファにコピー。
    /// </remark>

    class RenderingEngine : public Noncopyable
    {

    public:
        // ディファードライティング用の定数バッファ
        struct SDeferredLightingCB
        {
            Light m_light;              // ライト
            Matrix mlvp[MAX_DIRECTIONAL_LIGHT][NUM_SHADOW_MAP]; // ライトビュープロジェクション行列。
            float m_iblLuminance;       // IBLの明るさ。
            int m_isIBL;                // IBLを行う。
            int m_isEnableRaytracing;   // レイトレーシングを行っている。
        };
        /// <summary>
        /// レイトレ用のライトデータ。
        /// </summary>
        struct RaytracingLightData {
            DirectionalLight m_directionalLight;  // ディレクショナルライト。
            Vector3 m_ambientLight;               // 環境光。IBLテクスチャが指定されていない場合に利用される。
            float m_iblIntencity;                 // IBL輝度。
            int m_enableIBLTexture;               // IBLテクスチャが指定されている。
        };
        // メインレンダリングターゲットのスナップショット
        enum class EnMainRTSnapshot
        {
            enDrawnOpacity,     // 不透明オブジェクトの描画完了した時点
            enNum,              // スナップショットの数
        };

        // レンダリングパス
        enum class EnRenderingPass
        {
            enComputeAnimatedVertex,    // アニメーション済み頂点計算。
            enRenderToShadowMap,        // シャドウマップへの描画パス
            enZPrepass,                 // ZPrepass
            enRenderToVolumeLightMap,   // ボリュームライトマップへの描画
            enLightCulling,             // ライトカリング。
            enRenderToGBuffer,          // G-Bufferへの描画パス
            enForwardRender,            // フォワードレンダリングの描画パス
            enPostEffect,               // ポストエフェクト
            enRender2D,                 // 2D描画。
        };
        /// <summary>
        /// イベント。
        /// </summary>
        enum EnEvent {
            enEventReInitIBLTexture,    // IBLテクスチャが再初期化された。
        };
        RenderingEngine();
        ~RenderingEngine();
        /// <summary>
        /// レンダリングパイプラインを初期化
        /// </summary>
        /// <param name="isSoftShadow">
        /// trueの場合、シャドウマップ法による、影をソフトシャドウにします。
        /// ハードシャドウにしたい場合は、falseを指定してください。
        /// </param>
        void Init(bool isSoftShadow);

        /// <summary>
        /// 描画オブジェクトを追加。
        /// </summary>
        void AddRenderObject(IRenderer* renderObject)
        {
            m_renderObjects.push_back(renderObject);
        }
        /// <summary>
        /// レイトレワールドにモデルを追加。
        /// </summary>
        void AddModelToRaytracingWorld(Model& model)
        {
            g_graphicsEngine->RegistModelToRaytracingWorld(model);
        }
        /// <summary>
        /// レイトレワールドの再構築リクエストを送る。
        /// </summary>
        void RequestRebuildRaytracingWorld()
        {
            g_graphicsEngine->RequestRebuildRaytracingWorld();
        }
        /// <summary>
        /// レイトレワールドからモデルを削除。
        /// </summary>
        void RemoveModelFromRaytracingWorld(Model& model)
        {
            g_graphicsEngine->RemoveModelFromRaytracingWorld(model);
        }
        /// <summary>
        /// ZPrepassで作成された深度テクスチャを取得
        /// </summary>
        Texture& GetZPrepassDepthTexture()
        {
            return m_zprepassRenderTarget.GetRenderTargetTexture();
        }
        /// <summary>
        /// GBufferのアルベドテクスチャを取得
        /// </summary>
        Texture& GetGBufferAlbedoTexture()
        {
            return m_gBuffer[enGBufferAlbedoDepth].GetRenderTargetTexture();
        }
        /// <summary>
        /// GBufferの法線テクスチャを取得。
        /// </summary>
        Texture& GetGBufferNormalTexture()
        {
            return m_gBuffer[enGBufferNormal].GetRenderTargetTexture();
        }
        /// <summary>
        /// 不透明オブジェクトの描画完了時のメインレンダリングターゲットの
        /// スナップショットを取得
        /// </summary>
        Texture& GetMainRenderTargetSnapshotDrawnOpacity()
        {
            return m_mainRTSnapshots[(int)EnMainRTSnapshot::enDrawnOpacity].GetRenderTargetTexture();
        }
        /// <summary>
        /// シーンの平均輝度テクスチャを取得。
        /// </summary>
        Texture& GetLuminanceAvgTextureInScene()
        {
            return m_postEffect.GetLuminanceAvgTextureInScene();
        }
        /// <summary>
        /// レンダリングパイプラインを実行
        /// </summary>
        void Execute(RenderContext& rc);
        /// <summary>
        /// ディレクショナルライトのパラメータを設定
        /// </summary>
        void SetDirectionLight(int lightNo, Vector3 direction, Vector3 color)
        {
            m_sceneLight.SetDirectionLight(lightNo, direction, color);
        }
        void SetMainRenderTargetAndDepthStencilBuffer(RenderContext& rc)
        {
            rc.SetRenderTarget(m_mainRenderTarget.GetRTVCpuDescriptorHandle(), m_zprepassRenderTarget.GetDSVCpuDescriptorHandle());
        }
        /// <summary>
        /// ソフトシャドウを行っているか。
        /// </summary>
        bool IsSoftShadow() const
        {
            return m_isSoftShadow;
        }
        /// <summary>
        /// 影パラメータを設定する。
        /// カスケードシャドウ法で使用するエリアの割合と光源の高さを設定します。
        /// </summary>
        /// <param name="nearRate">近影エリア率。0.0f～1.0f</param>
        /// <param name="middleRate">中影エリア率。0.0f～1.0f</param>
        /// <param name="farRate">遠影エリア率。0.0f～1.0f</param>
        /// <param name="lightHeght">光源の高さ。大きいほど広い範囲をカバー</param>
        void SetShadowParameter(float nearRate, float middleRate, float farRate, float lightHeght);
        /// <summary>
        /// カスケードシャドウのエリア率を設定。
        /// nearArea=0.1(10%), middleArea=0.3(30%), farArea=1.0(100%) のように指定します。
        /// </summary>
        void SetCascadeNearAreaRates(float nearArea, float middleArea, float farArea)
        {
            for (auto& renderer : m_shadowMapRenders) {
                renderer.SetCascadeNearAreaRates(nearArea, middleArea, farArea);
            }
        }
        /// <summary>
        /// ビューカリングのためのビュープロジェクション行列を取得。
        /// </summary>
        const Matrix& GetViewProjectionMatrixForViewCulling() const
        {
            return m_viewProjMatrixForViewCulling;
        }
        /// <summary>
        /// シーンの切り替えが起きたことを通知する。
        /// </summary>
        void NotifyChangeScene(float changeSceneTime)
        {
            m_postEffect.NotifyChangeScene(changeSceneTime);
        }
        /// <summary>
        /// 幾何学データを登録
        /// </summary>
        void RegisterGeometryData(GemometryData* geomData)
        {
            m_sceneGeometryData.RegisterGeometryData(geomData);
        }
        /// <summary>
        /// 幾何学データの登録を解除。
        /// </summary>
        void UnregisterGeometryData(GemometryData* geomData)
        {
            m_sceneGeometryData.UnregisterGeometryData(geomData);
        }
        /// <summary>
        /// 更新。
        /// </summary>
        void Update();
        /// <summary>
        /// イベントリスナーを追加。
        /// </summary>
        void AddEventListener(
            void* pListenerObj,
            std::function<void(EnEvent enEvent)> listenerFunc
        )
        {
            m_eventListeners.push_back({ pListenerObj, listenerFunc });
        }
        /// <summary>
        /// イベントリスナーを削除。
        /// </summary>
        void RemoveEventListener(void* pListenerObj)
        {
            auto it = std::find_if(
                m_eventListeners.begin(),
                m_eventListeners.end(),
                [&](const SEventListenerData& listenerData) {return listenerData.pListenerObj == pListenerObj; }
            );
            if (it != m_eventListeners.end()) {
                m_eventListeners.erase(it);
            }
        }
        /// <summary>
        /// IBLを再初期化。
        /// </summary>
        void ReInitIBL(const wchar_t* iblTexFilePath, float luminance);
        /// <summary>
        /// IBLテクスチャを取得。
        /// </summary>
        Texture& GetIBLTexture()
        {
            return m_iblData.m_texture;
        }
        /// <summary>
        /// シャドウマップテクスチャにクエリを行う。
        /// </summary>
        void QueryShadowMapTexture(std::function< void(Texture& shadowMap) > queryFunc)
        {
            for (int i = 0; i < MAX_DIRECTIONAL_LIGHT; i++)
            {
                for (int areaNo = 0; areaNo < NUM_SHADOW_MAP; areaNo++)
                {
                    queryFunc(m_shadowMapRenders[i].GetShadowMap(areaNo));
                }
            }
        }
        SDeferredLightingCB& GetDeferredLightingCB()
        {
            return m_deferredLightingCB;
        }
        /// <summary>
        /// トーンマップを無効にする
        /// </summary>
        void DisableTonemap()
        {
            m_postEffect.DisableTonemap();
        }
        /// <summary>
        /// トーンマップを有効にする。
        /// </summary>
        void EnableTonemap()
        {
            m_postEffect.EnableTonemap();
        }
        /// <summary>
        /// トーンマップが有効か判定する。
        /// </summary>
        bool IsEnableTonemap() const
        {
            return m_postEffect.IsEnableTonemap();
        }
        /// <summary>
        /// シーンの中間の明るさの中間輝度値を指定する。
        /// </summary>
        void SetSceneMiddleGray(float luminance)
        {
            m_postEffect.SetTonemapMiddlegray(luminance);
        }
        /// <summary>
        /// シーンの中間の明るさの中間輝度値を取得する。
        /// </summary>
        float GetSceneMiddleGray() const
        {
            return m_postEffect.GetTonemapMiddlegray();
        }
        /// <summary>
        /// ブルームが発生する輝度の閾値を設定。
        /// </summary>
        void SetBloomThreshold(float value)
        {
            m_postEffect.SetBloomThreshold(value);
        }
        /// <summary>
        /// ブルームが発生する輝度の閾値を取得。
        /// </summary>
        float GetBloomThreshold() const
        {
            return m_postEffect.GetBloomThreshold();
        }
        /// <summary>
        /// ボリュームスポットライトをシーンに追加
        /// </summary>
        void AddVolumeSpotLight(VolumeLightBase& lig)
        {
            m_volumeLightRender.AddVolumeSpotLight(lig);
        }
        /// <summary>
        /// ボリュームスポットライトをシーンから削除
        /// </summary>
        void RemoveVolumeSpotLight(VolumeLightBase& lig)
        {
            m_volumeLightRender.RemoveVolumeSpotLight(lig);
        }
        /// <summary>
        /// ボリュームライトレンダラーを取得。
        /// </summary>
        VolumeLightRender& GetVolumeLightRender()
        {
            return m_volumeLightRender;
        }
        /// <summary>
        /// 環境光の計算のためのIBLテクスチャを設定。
        /// </summary>
        void SetAmbientByIBLTexture(const wchar_t* textureFilePath, float luminance)
        {
            ReInitIBL(textureFilePath, luminance);
        }
        /// <summary>
        /// IBL環境光を無効にする。
        /// </summary>
        void DisableIBLTextureForAmbient()
        {
            m_sceneLight.DisableIBLTextureForAmbient();
        }
        /// <summary>
        /// 環境光を設定。
        /// </summary>
        void SetAmbient(Vector3 ambient)
        {
            m_sceneLight.SetAmbinet(ambient);
        }
        /// <summary>
        /// レイトレーシングが有効かどうかを判定する。
        /// </summary>
        bool IsEnableRaytracing() const
        {
            return m_isEnableRaytracing && g_graphicsEngine->IsPossibleRaytracing();
        }
        /// <summary>
        /// レイトレーシングを有効にします。
        /// </summary>
        void EnableRaytracing()
        {
            m_isEnableRaytracing = true && IsEnableRaytracing();
        }
        /// <summary>
        /// レイトレーシングを無効にします。
        /// </summary>
        void DisableRaytracing()
        {
            m_isEnableRaytracing = false && IsEnableRaytracing();
        }
        /// <summary>
        /// レイトレ用のライトデータを取得。
        /// </summary>
        RaytracingLightData& GetRaytracingLightData()
        {
            return m_raytracingLightData;
        }
    private:
        /// <summary>
        /// イメージベースドライティング(IBL)のためのデータを初期化する。
        /// </summary>
        void InitIBLData(const wchar_t* iblTexFilePath, float intencity);
        /// <summary>
        /// G-Bufferを初期化
        /// </summary>
        void InitGBuffer();
        /// <summary>
        /// ディファードライティングの初期化
        /// </summary>
        void InitDeferredLighting();
        /// <summary>
        /// ディファードライティングで使用するスプライトを初期化。
        /// </summary>
        void InitDefferedLighting_Sprite();
        /// <summary>
        /// アニメーション済み頂点の計算。
        /// </summary>
        void ComputeAnimatedVertex(RenderContext& rc);
        /// <summary>
        /// シャドウマップに描画
        /// </summary>
        void RenderToShadowMap(RenderContext& rc);
        /// <summary>
        /// ZPrepass
        /// </summary>
        void ZPrepass(RenderContext& rc);
        /// <summary>
        /// G-Bufferへの描画
        /// </summary>
        void RenderToGBuffer(RenderContext& rc);
        /// <summary>
        /// ディファードライティング
        /// </summary>
        void DeferredLighting(RenderContext& rc);
        /// <summary>
        /// 2D描画
        /// </summary>
        void Render2D(RenderContext& rc);
        /// <summary>
        /// メインレンダリングターゲットの内容をフレームバッファにコピーする
        /// </summary>
        void CopyMainRenderTargetToFrameBuffer(RenderContext& rc);
        /// <summary>
        /// フォワードレンダリング
        /// </summary>
        void ForwardRendering(RenderContext& rc);
        /// <summary>
        /// メインレンダリングターゲットを初期化
        /// </summary>
        void InitMainRenderTarget();
        /// <summary>
        /// メインレンダリングターゲットのスナップショット用レンダリングターゲットを初期化
        /// </summary>
        void InitMainRTSnapshotRenderTarget();
        /// <summary>
        /// メインレンダリングターゲットのカラーバッファの内容を
        /// フレームバッファにコピーするためのスプライトを初期化する
        /// </summary>
        void InitCopyMainRenderTargetToFrameBufferSprite();
        /// <summary>
        /// ZPrepass用のレンダリングターゲットを初期化
        /// </summary>
        void InitZPrepassRenderTarget();
        /// <summary>
        /// メインレンダリングターゲットのスナップショットを撮影
        /// </summary>
        void SnapshotMainRenderTarget(RenderContext& rc, EnMainRTSnapshot enSnapshot);
        /// <summary>
        /// シャドウマップへの描画処理を初期化する
        /// </summary>
        void InitShadowMapRender();
        /// <summary>
        /// ビューカリング用のビュープロジェクション行列を計算。
        /// </summary>
        void CalcViewProjectionMatrixForViewCulling();
        /// <summary>
        /// 2D描画用のレンダリングターゲットを初期化
        /// </summary>
        void Init2DRenderTarget();
    private:
        // GBufferの定義
        enum EnGBuffer
        {
            enGBufferAlbedoDepth,           // アルベドと深度値。Aに深度値が記憶されています。
            enGBufferNormal,                // 法線
            enGBufferMetaricShadowSmooth,   // メタリック、影パラメータ、スムース。
            // メタリックはr、影パラメータはg、スムースはa。gは未使用。
            enGBufferNum,                   // G-Bufferの数
        };
        /// <summary>
        /// IBLデータ
        /// </summary>
        struct SIBLData {
            Texture m_texture;          // IBLテクスチャ
            float m_intencity = 1.0f;   // 輝度。
        };

        /// <summary>
        /// GIテクスチャを作るためのブラー数。
        /// </summary>
        enum EGITextureBlur {
            eGITextureBlur_1024x1024,   // 1024x1024
            eGITextureBlur_512x512,     // 512x512
            eGITextureBlur_256x256,     // 256x256
            eGITextureBlur_128x128,     // 128x128
            eGITextureBlur_Num,
        };
        RaytracingLightData m_raytracingLightData;                      // レイトレ用のライトデータ。
        LightCulling m_lightCulling;                                    // ライトカリング。
        ShadowMapRender m_shadowMapRenders[MAX_DIRECTIONAL_LIGHT];      // シャドウマップへの描画処理
        VolumeLightRender m_volumeLightRender;                          // ボリュームライトレンダラー。
        SDeferredLightingCB m_deferredLightingCB;                       // ディファードライティング用の定数バッファ
        Sprite m_copyMainRtToFrameBufferSprite;                         // メインレンダリングターゲットをフレームバッファにコピーするためのスプライト
        Sprite m_diferredLightingSprite;                                // ディファードライティングを行うためのスプライト
        RenderTarget m_zprepassRenderTarget;                            // ZPrepass描画用のレンダリングターゲット
        RenderTarget m_mainRenderTarget;                                // メインレンダリングターゲット
        RenderTarget m_mainRTSnapshots[(int)EnMainRTSnapshot::enNum];   // メインレンダリングターゲットのスナップショット
        RenderTarget m_gBuffer[enGBufferNum];                           // G-Buffer
        PostEffect m_postEffect;                                        // ポストエフェクト
        RWStructuredBuffer m_pointLightNoListInTileUAV;                 // タイルごとのポイントライトのリストのUAV。
        RWStructuredBuffer m_spotLightNoListInTileUAV;                  // タイルごとのスポットライトのリストのUAV。
        std::vector< IRenderer* > m_renderObjects;                      // 描画オブジェクトのリスト。
        SceneLight m_sceneLight;                                        // シーンライト。
        bool m_isSoftShadow = false;                                    // ソフトシャドウフラグ。
        Matrix m_viewProjMatrixForViewCulling;                          // ビューカリング用のビュープロジェクション行列。
        SceneGeometryData m_sceneGeometryData;                          // シーンのジオメトリデータ。
        static RenderingEngine* m_instance;                             // 唯一のインスタンスのアドレスを記録する変数。
        RenderTarget m_2DRenderTarget;                                  // 2D描画用のレンダリングターゲット。
        Sprite m_2DSprite;                                              // 2D描画用のスプライト。
        Sprite m_mainSprite;
        SIBLData m_iblData;                                             // IBLデータ。
        bool m_isEnableRaytracing = true;                               // レイトレーシングが有効？
        GaussianBlur m_giTextureBlur[eGITextureBlur_Num];               // GIテクスチャにブラーをかける処理。
        /// <summary>
        /// イベントリスナーのデータ。
        /// </summary>
        struct SEventListenerData {
            void* pListenerObj;     // リスナーオブジェクト
            std::function<void(EnEvent enEvent)> listenerFunc;
        };

        std::list< SEventListenerData > m_eventListeners;                // イベントリスナー。
    };
}
