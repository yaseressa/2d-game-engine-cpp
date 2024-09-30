#pragma once
#include "../ECS/ECS.h"
#include <imgui/imgui.h>
#include <imgui/imgui_sdl.h>
#include <imgui/imgui_impl_sdl.h>
#include <algorithm>
class RenderGuiSystem : public System {
public:
	RenderGuiSystem() = default;
	void Update(unique_ptr<WorldCordinator>& worldCordinator, unique_ptr<AssetManager>& assetManager, SDL_Rect& camera) {
		ImGui::NewFrame();
		ImGui::SetNextWindowPos(ImVec2(Game::WindowWidth - 350, 0), ImGuiCond_Always, ImVec2(0, 0));
		ImGui::SetNextWindowBgAlpha(0.9f);
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoNav | ImGuiWindowFlags_AlwaysAutoResize;


		if (ImGui::Begin("Spawn Enemy", NULL, windowFlags)) {
			
			static int
				posX = 0,
				posY = 0,
				scaleY = 0,
				scaleX = 0,
				velX = 0,
				velY = 0,
				health = 100,
				projRepeat = 0,
				projDuration = 0,
				selectedSpriteIndex = 0;
			static float
				rotation = 0.0,
				projAngle = 0.0,
				projSpeed = 100.0;
			const vector<string>
				sprites = assetManager->GetTextureNames();
			
			ImGui::Text(("Mouse X: " + to_string(ImGui::GetIO().MousePos.x - camera.x) + ", Y: " + to_string(ImGui::GetIO().MousePos.y - camera.y)).c_str());


			if (ImGui::CollapsingHeader("Sprite", ImGuiTreeNodeFlags_DefaultOpen)) {
				const int n = sprites.size();
				char** sprites_arr = new char* [n];  
				copyVectorToArray<char>(sprites, sprites_arr); 
				ImGui::Combo("texture id", &selectedSpriteIndex, sprites_arr, n);

			}
			ImGui::Spacing();
			if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::InputInt("position x", &posX);
				ImGui::InputInt("position y", &posY);
				ImGui::SliderInt("scale x", &scaleX, 1, 10);
				ImGui::SliderInt("scale y", &scaleY, 1, 10);
				ImGui::SliderAngle("rotation (deg)", &rotation, 0, 360);

			}
			ImGui::Spacing();
			if (ImGui::CollapsingHeader("Rigid Body", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::InputInt("velocity x", &velX);
				ImGui::InputInt("velocit y", &velY);
			}
			ImGui::Spacing();
			
			if (ImGui::CollapsingHeader("Projectile Emitter", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::SliderAngle("angle (deg)", &projAngle, 0, 360);
				ImGui::SliderFloat("speed (px/sec)", &projSpeed, 10, 500);
				ImGui::InputInt("repeat (sec)", &projRepeat);
				ImGui::InputInt("duration (sec)", &projDuration);
			}
	
			ImGui::Spacing();
			if (ImGui::CollapsingHeader("Health", ImGuiTreeNodeFlags_DefaultOpen)) {
				ImGui::SliderInt("%", &health, 0, 100);
			}
			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();


			if (ImGui::Button("Create Enemy")) {

				
				Entity enemy = worldCordinator->CreateEntity();
				enemy.Group("enemies");
				enemy.AddComponent<TransformComponent>(vec2(posX, posY), vec2(scaleX, scaleY), degrees(rotation));
				enemy.AddComponent<RigidBodyComponent>(vec2(velX, velY));
				Logger::Debug(sprites[selectedSpriteIndex]);
				enemy.AddComponent<SpriteComponent>(sprites[selectedSpriteIndex], 32, 32, NULL, NULL, 1);
				enemy.AddComponent<BoxCollidorComponent>(32, 32, vec2(5, 5));
				double projVelX = cos(projAngle) * projSpeed;
				double projVelY = sin(projAngle) * projSpeed;
				enemy.AddComponent<ProjectileEmitterComponent>(vec2(projVelX, projVelY), projRepeat * 1000,projDuration * 1000, 10, false);
				enemy.AddComponent<HealthComponent>(health);
				posX = posY = rotation = projAngle = 0;
				scaleY = scaleX = 1;
				projRepeat = projDuration = 10;
				projSpeed = health = 100;

			}
		}
		ImGui::End();
		ImGui::Render();
		ImGuiSDL::Render(ImGui::GetDrawData());
	}

	template<typename T>
	void copyVectorToArray(const std::vector<std::string>& v, T* arr[]) {
		std::transform(v.begin(), v.end(), arr, [](const std::string& s) {
			return _strdup(s.c_str());  
			});
	}
};
