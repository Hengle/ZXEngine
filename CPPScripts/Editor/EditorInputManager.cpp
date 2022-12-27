#include "EditorInputManager.h"
#include "EditorGUIManager.h"
#include "EditorAssetPreviewer.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_internal.h"
#include "../ImGui/imgui_impl_glfw.h"
#include "../ImGui/imgui_impl_opengl3.h"
#include "../ProjectSetting.h"
#include "../GlobalData.h"
#include "../InputManager.h"

namespace ZXEngine
{
	EditorInputManager* EditorInputManager::mInstance = nullptr;

	void EditorInputManager::Create()
	{
		mInstance = new EditorInputManager();
	}

	EditorInputManager* EditorInputManager::GetInstance()
	{
		return mInstance;
	}

	void EditorInputManager::Update()
	{
		if (IsProcessGameInput())
			return;


		if (CheckCurMousePos() == EditorAreaType::EAT_AssetPreview)
		{
			// Preview����������������༭������ק״̬
			if (ImGui::IsMouseDown(0))
				isDragging = true;
		}

		if (isDragging)
		{
			ImGuiIO& io = ImGui::GetIO();
			// ��ק״̬����תԤ��ģ��
			EditorGUIManager::GetInstance()->assetPreviewer->UpdateModelRotation(io.MouseDelta.x, io.MouseDelta.y);

			// ��ק״̬���ɿ��������˳���ק״̬
			if (!ImGui::IsMouseDown(0))
				isDragging = false;
		}
	}

	EditorAreaType EditorInputManager::CheckCurMousePos()
	{
		ImGuiIO& io = ImGui::GetIO();
		float posX = io.MousePos.x;
		float posY = io.MousePos.y;
		posX = Math::Max(0.0f, posX);
		posY = Math::Max(0.0f, posY);
		if (posX > ProjectSetting::hierarchyWidth && posX < ProjectSetting::hierarchyWidth + GlobalData::srcWidth
			&& posY > ProjectSetting::mainBarHeight && posY < ProjectSetting::mainBarHeight + GlobalData::srcHeight)
			return EditorAreaType::EAT_Game;
		else if (posX > ProjectSetting::projectWidth && posY > ProjectSetting::mainBarHeight + ProjectSetting::inspectorHeight)
			return EditorAreaType::EAT_AssetPreview;
		else
			return EditorAreaType::EAT_None;
	}

	bool EditorInputManager::IsProcessGameInput()
	{
		// �����ǰδ���ڱ༭����ק״̬��������걻��Ϸ������ߵ�ǰλ������Ϸ���������ڣ�����Ҫ������Ϸ����
		return !isDragging && (!InputManager::GetInstance()->isCursorShow || CheckCurMousePos() == EditorAreaType::EAT_Game);
	}

	void EditorInputManager::UpdateMouseScroll(float xOffset, float yOffset)
	{
		// �����༭������������ͳһ��ImGuiIO�ģ�����ImGuiIO��MouseWheel����Ҫ��ImGui::Begin��End֮����ò�����Ч
		if (CheckCurMousePos() == EditorAreaType::EAT_AssetPreview)
			EditorGUIManager::GetInstance()->assetPreviewer->UpdateModelScale(yOffset);
	}
}