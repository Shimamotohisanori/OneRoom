#pragma once
/**
 * GameObjectCleaner.h
 * ゲームオブジェクトを削除するクラス
 * ゲーム中に生成されるオブジェクトは大体ここで消す
 */
namespace GameObjectCleaner
{
	/** ゲームオブジェクトをすべて削除する */
	void DeleteAllGameObjects();

	/** ゲームオブジェクトを1段階ずつ削除する(全部消し終わったらtrueを返す) */
	bool DeleteGameObjectsStep(int step);
}

