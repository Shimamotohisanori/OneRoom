#include "stdafx.h"
#include "Flashlight.h"

namespace
{
	/** モデルファイル(自分のパスに合わせて変更) */
	constexpr const char* MODEL_PATH = "Assets/modelData/FlashLight/FlashLight.tkm";

	/** カメラ基準のモデル位置オフセット(右・下・前) */
	constexpr float OFFSET_RIGHT = 18.0f;
	constexpr float OFFSET_UP = -14.0f;
	constexpr float OFFSET_FORWARD = 30.0f;

	/** モデル先端(光の出口)までの距離。モデルの前方向に沿って */
	constexpr float MUZZLE_DISTANCE = 12.0f;

	/** ライトが狙う注視点までの距離。右腕から出た光が画面中央に収束する */
	constexpr float AIM_DISTANCE = 400.0f;

	/** 向きがカメラに追従する速さ(小さいほど遅れる=手持ち感) */
	constexpr float FOLLOW_SPEED = 10.0f;

	/** 真上・真下付近とみなす閾値(外積が潰れる対策) */
	constexpr float POLE_THRESHOLD = 0.001f;

	/** モデルを前後反転するか(180度回転) */
	constexpr bool IS_MODEL_FLIPPED = true;

	/** 点灯・消灯のフェード速度(1秒あたり) */
	constexpr float FADE_IN_SPEED = 12.0f;
	constexpr float FADE_OUT_SPEED = 8.0f;

	/** ライトの基本色(少し黄色寄りの白熱電球色)と強さ */
	const Vector3 LIGHT_COLOR = { 1.0f, 0.92f, 0.75f };
	constexpr float LIGHT_POWER = 25.0f;

	/** 光が届く距離 */
	constexpr float LIGHT_RANGE = 900.0f;

	/** 光の広がり(度)。内側の明るい部分と外側の淡い部分 */
	constexpr float SPOT_ANGLE_DEG = 28.0f;

	/** 距離減衰・角度減衰の累乗。大きいほど急に暗く/縁がぼける */
	constexpr float RANGE_POW = 2.0f;
	constexpr float ANGLE_POW = 1.5f;

	/** ちらつきの強さ(0.03 = ±3%) */
	constexpr float FLICKER_AMOUNT = 0.03f;
}

Flashlight::Flashlight()
{}

Flashlight::~Flashlight()
{}

void Flashlight::Start()
{
	/** モデルを初期化 */
	m_model.Init(MODEL_PATH);

	/** スポットライトの初期化 */
	m_spotLight.Init();

	/** 最初は消灯状態 */
	m_spotLight.SetColor(0.0f, 0.0f, 0.0f);

	/** 光の届く範囲 */
	m_spotLight.SetRange(LIGHT_RANGE);

	/** スポットライトの角度は度からラジアンに変換して設定 */
	m_spotLight.SetAngle(Math::DegToRad(SPOT_ANGLE_DEG));

	/** 距離による減衰の累乗を設定 */
	m_spotLight.SetRangeAffectPowParam(RANGE_POW);

	/** 角度による減衰の累乗を設定 */
	m_spotLight.SetAngleAffectPowParam(ANGLE_POW);

	/** カメラの向きで初期化 */
	m_smoothedForward = g_camera3D->GetForward();
}

void Flashlight::Update()
{
	/** 点灯・消灯のフェードを更新 */
	UpdateIntensity();

	/** モデルの位置・向き、光の位置・向きを更新 */
	UpdateTransform();

	/** スポットライトのパラメータを反映 */
	UpdateSpotLight();
}

void Flashlight::UpdateIntensity()
{
	/** フレーム経過時間を取得 */
	float dt = g_gameTime->GetFrameDeltaTime();

	/** 点灯・消灯の目標値を決定 */
	float target = m_isOn ? 1.0f : 0.0f;

	/** 点灯・消灯の速度を決定 */
	float speed = m_isOn ? FADE_IN_SPEED : FADE_OUT_SPEED;

	/** 目標に向かって近づける */
	if (m_intensity < target) {
		m_intensity = min(m_intensity + speed * dt, target);
	}
	else if (m_intensity > target) {
		m_intensity = max(m_intensity - speed * dt, target);
	}

	m_flickerTimer += dt;
}

void Flashlight::UpdateTransform()
{
	const Vector3 camPos = g_camera3D->GetPosition();
	const Vector3 camForward = g_camera3D->GetForward();
	const Vector3 camRight = g_camera3D->GetRight();
	const Vector3 camUp = g_camera3D->GetUp();

	/** 手持ちの遅れ:前方向ベクトルを丸ごと補間するので、
	 *  左右(ヨー)も上下(ピッチ)も同時に追従する */
	float t = min(FOLLOW_SPEED * g_gameTime->GetFrameDeltaTime(), 1.0f);
	m_smoothedForward.Lerp(t, m_smoothedForward, camForward);
	m_smoothedForward.Normalize();

	/** モデル位置:カメラの右下前(位置自体はカメラに固定) */
	Vector3 modelPos = camPos
		+ camRight * OFFSET_RIGHT
		+ camUp * OFFSET_UP
		+ camForward * OFFSET_FORWARD;

	/** 追従した前方向から、右・上ベクトルを作る */
	Vector3 right;
	right.Cross(Vector3::AxisY, m_smoothedForward);
	if (right.Length() < POLE_THRESHOLD) {
		/** ほぼ真上・真下を向いたときは、カメラの右を代わりに使う */
		right = camRight;
	}
	right.Normalize();

	Vector3 up;
	up.Cross(m_smoothedForward, right);
	up.Normalize();

	/** 光は反転の影響を受けないので、モデル用の基底だけ別に作る */
	Vector3 modelForward = m_smoothedForward;
	Vector3 modelRight = right;
	if (IS_MODEL_FLIPPED) {
		/** 上軸まわりに180度回転 = 右と前を反転 */
		modelForward *= -1.0f;
		modelRight *= -1.0f;
	}

	Matrix rotMat;
	rotMat.m[0][0] = modelRight.x;    rotMat.m[0][1] = modelRight.y;    rotMat.m[0][2] = modelRight.z;    rotMat.m[0][3] = 0.0f;
	rotMat.m[1][0] = up.x;            rotMat.m[1][1] = up.y;            rotMat.m[1][2] = up.z;            rotMat.m[1][3] = 0.0f;
	rotMat.m[2][0] = modelForward.x;  rotMat.m[2][1] = modelForward.y;  rotMat.m[2][2] = modelForward.z;  rotMat.m[2][3] = 0.0f;
	rotMat.m[3][0] = 0.0f;            rotMat.m[3][1] = 0.0f;            rotMat.m[3][2] = 0.0f;            rotMat.m[3][3] = 1.0f;

	Quaternion rot;
	rot.SetRotation(rotMat);

	m_model.SetPosition(modelPos);
	m_model.SetRotation(rot);
	m_model.Update();

	/** 光の出口はモデル先端(モデルの向きと同じ追従した方向) */
	m_lightPosition = modelPos + m_smoothedForward * MUZZLE_DISTANCE;

	/** 狙いも追従した前方向を基準にして、モデルと光がズレないようにする */
	Vector3 aimPoint = camPos + m_smoothedForward * AIM_DISTANCE;
	m_lightDirection = aimPoint - m_lightPosition;
	m_lightDirection.Normalize();
}

void Flashlight::UpdateSpotLight()
{
	/** ちらつき:周期の違うsinを重ねて不規則に見せる */
	float flicker = 1.0f
		+ FLICKER_AMOUNT * sinf(m_flickerTimer * 47.0f)
		* sinf(m_flickerTimer * 13.0f);

	float power = LIGHT_POWER * m_intensity * flicker;

	m_spotLight.SetPosition(m_lightPosition);
	m_spotLight.SetDirection(m_lightDirection);
	m_spotLight.SetColor(LIGHT_COLOR.x * power, LIGHT_COLOR.y * power, LIGHT_COLOR.z * power);
	m_spotLight.Update();
}

void Flashlight::Render(RenderContext& rc)
{
	/** 完全に消灯したらモデルも非表示 */
	if (!m_isOn && m_intensity <= 0.0f) {
		return;
	}
	m_model.Draw(rc);
}
