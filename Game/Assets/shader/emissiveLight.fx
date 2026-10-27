/**
 * emissiveLight.fx
 * 蛍光灯用の自己発光シェーダー。
 * ライティングは行わず、アルベドテクスチャに「発光色」を掛けて出力する。
 * 出力先はHDR(R16G16B16A16_FLOAT)なので、1.0を超える値を出せる。
 * ブルームのしきい値を超えると、にじんで光って見える。
 */

/** モデル用の定数バッファ(エンジン側が設定する) */
cbuffer ModelCb : register(b0)
{
    float4x4 mWorld;
    float4x4 mView;
    float4x4 mProj;
};

/**
 * 発光の設定(LightObject.cpp の EmissiveCB と並びを合わせること)
 * LightObject から毎フレーム更新される。
 */
cbuffer EmissiveCb : register(b1)
{
    float3 g_emissiveColor;     // 発光色(明るさ倍率を含む。1.0を超えてよい)
    float  g_maskThreshold;     // この輝度以上の部分だけ光らせる。0以下ならモデル全体が光る
    float  g_baseBrightness;    // 光らない部分の明るさ(アルベドに掛ける)
    float3 g_pad;               // 16バイト境界に合わせるための詰め物
};

/** 輝度マスクの境目のぼかし幅 */
static const float MASK_SOFTNESS = 0.1f;

/** 輝度を求めるための係数(Rec.601) */
static const float3 LUMINANCE_WEIGHT = float3(0.299f, 0.587f, 0.114f);

/** 頂点シェーダーへの入力 */
struct SVSIn
{
    float4 pos : POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 biNormal : BINORMAL;
    float2 uv : TEXCOORD0;
    int4   indices : BLENDINDICES0;
    float4 skinWeights : BLENDWEIGHT0;
};

/** ピクセルシェーダーへの入力 */
struct SPSIn
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

Texture2D<float4> g_albedo : register(t0);
sampler g_sampler : register(s0);

/** 頂点シェーダー */
SPSIn VSMain(SVSIn vsIn)
{
    SPSIn psIn;
    psIn.pos = mul(mWorld, vsIn.pos);
    psIn.pos = mul(mView, psIn.pos);
    psIn.pos = mul(mProj, psIn.pos);
    psIn.uv = vsIn.uv;
    return psIn;
}

/** ピクセルシェーダー */
float4 PSMain(SPSIn psIn) : SV_Target0
{
    float4 albedo = g_albedo.Sample(g_sampler, psIn.uv);

    /** 光らせる部分のマスク。しきい値が0以下ならモデル全体を光らせる */
    float mask = 1.0f;
    if (g_maskThreshold > 0.0f) {
        float luminance = dot(albedo.rgb, LUMINANCE_WEIGHT);
        mask = smoothstep(g_maskThreshold - MASK_SOFTNESS, g_maskThreshold + MASK_SOFTNESS, luminance);
    }

    /** ライティングなし:アルベド × (土台の明るさ + 発光色 × マスク) */
    float3 color = albedo.rgb * (g_baseBrightness + g_emissiveColor * mask);
    return float4(color, albedo.a);
}
