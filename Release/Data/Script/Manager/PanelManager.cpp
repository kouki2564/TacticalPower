#include "PanelManager.h"
#include "DxLib.h"

namespace
{
}

PanelManager::PanelManager()
{
	for (int i = 0; i < 10; i++)
	{
		auto string = ("Data/Image/number_" + std::to_string(i) + ".png").c_str();
		m_numberHandle[i] = LoadGraph(string);
	}
}

PanelManager::~PanelManager()
{
	for (int i = 0; i < 10; i++)
	{
		DeleteGraph(m_numberHandle[i]);
	}
}

void PanelManager::PanelUpdate()
{
	DrawNumber();
	DrawPanel();
}

void PanelManager::RegisterImage(std::string name, int handle, int startX, int startY, int endX, int endY)
{
	ImageData initData;
	initData.handle = handle;
	initData.startX = startX;
	initData.startY = startY;
	initData.endX = endX;
	initData.endY = endY;
	initData.isDraw = true;
	m_imageData.insert(std::make_pair(name, initData));
}

void PanelManager::Draw()
{
	DrawNumber();
	DrawPanel();
}

void PanelManager::DrawPanel()
{
	for (auto& image : m_imageData)
	{
		if (image.second.isDraw)
			DrawExtendGraph(image.second.startX, image.second.startY, image.second.endX, image.second.endY, image.second.handle, TRUE);
	}
}

void PanelManager::SetIsDrawImage(std::string name, bool isDraw)
{
	m_imageData[name].isDraw = isDraw;
}

void PanelManager::UpdateImagePos(std::string name, int startX, int startY, int endX, int endY)
{
	m_imageData[name].startX = startX;
	m_imageData[name].startY = startY;
	m_imageData[name].endX = endX;
	m_imageData[name].endY = endY;
}

void PanelManager::SetNumber(int number, VECTOR pos, float scale, int drawTime, VECTOR cameraDir)
{
	numData initData;
	// 数値を桁ごとに分解してvectorに格納
	while (number > 0)
	{
		initData.number.push_back(number % 10);
		number /= 10;
	}
	initData.decAlpha = 255 / drawTime;
	initData.time = drawTime;
	initData.pos = pos;
	initData.pos.y += 20.0f;
	initData.addScale = scale / drawTime;
	initData.cameraDir = cameraDir;
	m_numberData.push_back(initData);
}

void PanelManager::DrawNumber()
{
	for (auto& num : m_numberData)
	{
		// 描画時間が残っている間
		if (num.time > 0)
		{
			// 描画する数値のアルファ値を時間経過で低下
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, num.decAlpha * num.time);

			// 1の位から描画
			for (int i = 0; i < num.number.size(); i++)
			{
				// 桁の中央
				float center = (num.number.size() - 1) * 0.5f;
				// 各位の数値を取得
				int drawNum = num.number[i];

				// カメラ方向によって描画位置を変更
				// スケール値
				auto scale = (i - center) * (5);
				// 座標からカメラ方向に合わせてXZ平面上での描画位置を計算
				// 右方向
				auto rightDir = VCross(num.cameraDir, VGet(0, 1, 0));
				// 正規化
				rightDir = VNorm(rightDir);
				// 描画位置を計算
				auto drawPos = VAdd(num.pos, VScale(rightDir, scale));
				drawPos = ConvWorldPosToScreenPos(drawPos);

				if (drawPos.z > 0.0f && drawPos.z < 1.0f)
				{
					// 描画
					// DrawGraph(drawPos.x, drawPos.y, m_numberHandle[drawNum], TRUE);
					DrawExtendGraph(drawPos.x - 20 + (center - i) * (10 - 1 * (num.addScale * num.time)), drawPos.y - 60 * (1 + num.addScale * i), drawPos.x + 20 + (center - i) * (10 - 1 * (num.addScale * num.time)), drawPos.y, m_numberHandle[drawNum], TRUE);
				}

				// 各位の数値を描画
				/*DrawExtendGraph3D(num.pos.x - (i - center) * 20 * (1 + num.addScale * i), num.pos.y, num.pos.z,
								num.pos.x + (20 - (i - center) * 20) * (1 + num.addScale * i), num.pos.y + 20 * (1 + num.addScale * i),
								m_numberHandle[drawNum], TRUE);*/
				// DrawBillboard3D(VGet(num.pos.x - (i - center) * (10 - 5 * (num.addScale * num.time)), num.pos.y, num.pos.z), 0.5f, 0.5f, 10 - 5 * (num.addScale * num.time), 0.0f, m_numberHandle[drawNum], TRUE);
			}
			// 描画時間を減らす
			num.time--;
			// 表示位置を上昇
			num.pos.y += 0.75f;
		}
		else
		{
			// 描画時間が終了したら削除
			m_numberData.erase(m_numberData.begin());
		}
	}
	// 描画終了後にアルファ値を元に戻す
	SetDrawBlendMode(DX_BLENDMODE_ALPHA, 255);
}
