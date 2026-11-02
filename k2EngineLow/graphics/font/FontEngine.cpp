#include "k2EngineLowPreCompile.h"
#include "FontEngine.h"

using namespace std;
using namespace DirectX;

namespace nsK2EngineLow {
	FontEngine::~FontEngine()
	{
		if (m_srvDescriptorHeap != nullptr) {
			m_srvDescriptorHeap->Release();
		}
	}
	void FontEngine::Init()
	{
		auto d3dDevice = g_graphicsEngine->GetD3DDevice();

		// ディスクリプタヒープを作成。
		D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};

		srvHeapDesc.NumDescriptors = 1;
		srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		auto hr = d3dDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvDescriptorHeap));

		ResourceUploadBatch re(d3dDevice);
		re.Begin();
		// SpriteBatchのパイプラインステートを作成する。
		RenderTargetState renderTargetState;
		renderTargetState.rtvFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		renderTargetState.numRenderTargets = 1;
		renderTargetState.dsvFormat = DXGI_FORMAT_D32_FLOAT;
		renderTargetState.sampleMask = UINT_MAX;
		renderTargetState.sampleDesc.Count = 1;

		SpriteBatchPipelineStateDescription sprBatchDesc(renderTargetState);

		D3D12_VIEWPORT viewport;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		viewport.Width = static_cast<FLOAT>(1920);
		viewport.Height = static_cast<FLOAT>(1080);
		// Spriteバッチを作成。
		m_spriteBatch = make_unique<SpriteBatch>(
			d3dDevice,
			re,
			sprBatchDesc,
			&viewport);
		// SpriteFontを作成。
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = m_srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
		m_spriteFont = make_unique<SpriteFont>(
			d3dDevice,
			re,
			L"Assets/font/yumincho.spritefont",
			cpuHandle,
			gpuHandle);

		re.End(g_graphicsEngine->GetCommandQueue());
	}

	void FontEngine::BeginDraw(RenderContext& rc)
	{
		auto commandList = g_graphicsEngine->GetCommandList();
		auto d3dDevice = g_graphicsEngine->GetD3DDevice();
		m_spriteBatch->Begin(
			commandList,
			SpriteSortMode_Deferred,
			g_matIdentity
		);
		commandList->SetDescriptorHeaps(1, &m_srvDescriptorHeap);
	}
	void FontEngine::EndDraw(RenderContext& rc)
	{
		m_spriteBatch->End();
	}

	void FontEngine::Draw(
		const wchar_t* text,
		const Vector2& position,
		const Vector4& color,
		float rotation,
		float scale,
		Vector2 pivot
	)
	{
		/** 元の処理 */
		/*m_spriteFont->DrawString(
			m_spriteBatch.get(),
			text,
			position.vec,
			color,
			rotation,
			DirectX::XMFLOAT2(pivot.x, pivot.y),
			scale
		);*/

		/** 新しい処理 */
		/** 縁取りの太さ（画面上のピクセル相当）*/
		const float outline = 0.8f;

		/** 黒い縁取りを8方向に描画 */
		const Vector2 offsets[] = {
			Vector2(-outline,  0.0f),
			Vector2(outline,  0.0f),
			Vector2(0.0f, -outline),
			Vector2(0.0f,  outline),
			Vector2(-outline, -outline),
			Vector2(outline, -outline),
			Vector2(-outline,  outline),
			Vector2(outline,  outline)
		};

		/** 縁取りの色（黒）*/
		const Vector4 outlineColor(
			0.0f, 0.0f, 0.0f, 0.85f
		);

		/** 縁取りを描画 */
		for (const auto& offset : offsets)
		{
			/** オフセットを加算して描画位置を計算 */
			Vector2 drawPosition(
				position.x + offset.x,
				position.y + offset.y
			);

			/** 縁取りの文字を描画 */
			/** DrawStringで文字の色やフォントサイズなどを指定 */
			m_spriteFont->DrawString(
				m_spriteBatch.get(),
				text,
				drawPosition.vec,
				outlineColor,
				rotation,
				DirectX::XMFLOAT2(pivot.x, pivot.y),
				scale
			);
		}

		/** 最後に本来の文字を描画 */
		m_spriteFont->DrawString(
			m_spriteBatch.get(),
			text,
			position.vec,
			color,
			rotation,
			DirectX::XMFLOAT2(pivot.x, pivot.y),
			scale
		);
	}
}