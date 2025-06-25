#include "CleaningUi.h"

#include <format>

#include "Audio/AudioManager.h"
#include "Engine/Components/BloodSystem/BloodManager.h"
#include "Engine/Components/Game/ThrashManager.h"
#include "Engine/Input/InputManager.h"
#include "GLFW/glfw3.h"
#include "Engine/Textures/TextureManager.h"
#include "Materials/Ui/BasicImageMaterial.h"
#include "Math/Easings.h"

namespace Engine::Ui
{
    CleaningUi::CleaningUi()
    {
        TaskListBackground = AddElement<Image>(nullptr);
        TaskListBackground->GetRect().SetPositionPixels(glm::vec3(1280, 0, 0));
        TaskListBackground->GetRect().SetSizePixels(glm::vec2(960, 880));
        Materials::UiMaterial* imageMaterial = new Materials::BasicImageMaterial();
        TaskListBackground->SetMaterial(imageMaterial);
        TaskListBackground->SetTexture(TextureManager::GetTexture("./res/textures/CleaningUi/TaskListBackground.dds"));

        Text* TasksText = AddElement<Text>(TaskListBackground);
        TasksText->GetRect().SetPositionPixels(glm::vec3(-180, 400, 0));
        TasksText->GetRect().SetSizePixels(glm::vec2(480, 70));
        TasksText->SetFont("EagleLakeRegular");
        TasksText->SetText("Tasks:");
        TasksText->SetColor(HeaderColor);

        TrashText = AddElement<Text>(TaskListBackground);
        TrashText->GetRect().SetPositionPixels(glm::vec3(-150, 270, 0));
        TrashText->GetRect().SetSizePixels(glm::vec2(700, TaskListFontSize));
        TrashText->SetFont("EagleLakeRegular");
        TrashText->SetText("Dispose of the trash");

        FloorText = AddElement<Text>(TaskListBackground);
        FloorText->GetRect().SetPositionPixels(glm::vec3(-150, 230, 0));
        FloorText->GetRect().SetSizePixels(glm::vec2(700, TaskListFontSize));
        FloorText->SetFont("EagleLakeRegular");
        FloorText->SetText("Mop the floor");

        TrashProgressText = AddElement<Text>(TaskListBackground);
        TrashProgressText->GetRect().SetPositionPixels(glm::vec3(200, 270, 0));
        TrashProgressText->GetRect().SetSizePixels(glm::vec2(700, TaskListFontSize));
        TrashProgressText->SetFont("EagleLakeRegular");
        TrashProgressText->SetText("(xx/xx)");

        FloorProgressText = AddElement<Text>(TaskListBackground);
        FloorProgressText->GetRect().SetPositionPixels(glm::vec3(200, 230, 0));
        FloorProgressText->GetRect().SetSizePixels(glm::vec2(700, TaskListFontSize));
        FloorProgressText->SetFont("EagleLakeRegular");
        FloorProgressText->SetText("(xx%)");

        WeaponText = AddElement<Text>(TaskListBackground);
        WeaponText->GetRect().SetPositionPixels(glm::vec3(-150, 190, 0));
        WeaponText->GetRect().SetSizePixels(glm::vec2(550, TaskListFontSize));
        WeaponText->SetFont("EagleLakeRegular");
        WeaponText->SetText("Put the weapons on the rack");

        WeaponProgressText = AddElement<Text>(TaskListBackground);
        WeaponProgressText->GetRect().SetPositionPixels(glm::vec3(200, 190, 0));
        WeaponProgressText->GetRect().SetSizePixels(glm::vec2(700, TaskListFontSize));
        WeaponProgressText->SetFont("EagleLakeRegular");
        WeaponProgressText->SetText("(xx/xx)");

        BooksText = AddElement<Text>(TaskListBackground);
        BooksText->GetRect().SetPositionPixels(glm::vec3(-150, 110, 0));
        BooksText->GetRect().SetSizePixels(glm::vec2(550, TaskListFontSize));
        BooksText->SetFont("EagleLakeRegular");
        BooksText->SetText("Place the books back on the shelf");

        BooksProgressText = AddElement<Text>(TaskListBackground);
        BooksProgressText->GetRect().SetPositionPixels(glm::vec3(200, 110, 0));
        BooksProgressText->GetRect().SetSizePixels(glm::vec2(700, TaskListFontSize));
        BooksProgressText->SetFont("EagleLakeRegular");
        BooksProgressText->SetText("(xx/xx)");

        CoinsText = AddElement<Text>(TaskListBackground);
        CoinsText->GetRect().SetPositionPixels(glm::vec3(-150, 30, 0));
        CoinsText->GetRect().SetSizePixels(glm::vec2(550, TaskListFontSize));
        CoinsText->SetFont("EagleLakeRegular");
        CoinsText->SetText("Put the coins in the chest");

        CoinsProgressText = AddElement<Text>(TaskListBackground);
        CoinsProgressText->GetRect().SetPositionPixels(glm::vec3(200, 30, 0));
        CoinsProgressText->GetRect().SetSizePixels(glm::vec2(700, TaskListFontSize));
        CoinsProgressText->SetFont("EagleLakeRegular");
        CoinsProgressText->SetText("(xx/xx)");

        AdditionalTasks = AddElement<Text>(TaskListBackground);
        AdditionalTasks->GetRect().SetPositionPixels(glm::vec3(-180, -10, 0));
        AdditionalTasks->GetRect().SetSizePixels(glm::vec2(550, 70));
        AdditionalTasks->SetFont("EagleLakeRegular");
        AdditionalTasks->SetText("Optional:");
        AdditionalTasks->SetColor(HeaderColor);


        DontFurniture = AddElement<Text>(TaskListBackground);
        DontFurniture->GetRect().SetPositionPixels(glm::vec3(-150, -140, 0));
        DontFurniture->GetRect().SetSizePixels(glm::vec2(550, TaskListFontSize));
        DontFurniture->SetFont("EagleLakeRegular");
        DontFurniture->SetText("Do not dispose of the furniture");
        DontFurniture->SetColor(PositiveColor);

        DontBooks = AddElement<Text>(TaskListBackground);
        DontBooks->GetRect().SetPositionPixels(glm::vec3(-150, -180, 0));
        DontBooks->GetRect().SetSizePixels(glm::vec2(550, TaskListFontSize));
        DontBooks->SetFont("EagleLakeRegular");
        DontBooks->SetText("Do not dispose of the books");
        DontBooks->SetColor(PositiveColor);

        DontCoins = AddElement<Text>(TaskListBackground);
        DontCoins->GetRect().SetPositionPixels(glm::vec3(-150, -220, 0));
        DontCoins->GetRect().SetSizePixels(glm::vec2(550, TaskListFontSize));
        DontCoins->SetFont("EagleLakeRegular");
        DontCoins->SetText("Do not throw out any coins");
        DontCoins->SetColor(FailedColor);

        DontWeapons = AddElement<Text>(TaskListBackground);
        DontWeapons->GetRect().SetPositionPixels(glm::vec3(-150, -260, 0));
        DontWeapons->GetRect().SetSizePixels(glm::vec2(550, TaskListFontSize));
        DontWeapons->SetFont("EagleLakeRegular");
        DontWeapons->SetText("Do not throw out any weapons");
        DontWeapons->SetColor(PositiveColor);

        AudioManager::GetInstance().ConfigureSoundAttenuation(ListSound, 1.0f, 100.0f, 0.0f);
        AudioManager::GetInstance().ConfigureSoundAttenuation(TaskSound, 1.0f, 100.0f, 0.0f);
        AudioManager::GetInstance().SetVolume(ListSound, 0.5f);
        AudioManager::GetInstance().SetVolume(TaskSound, 0.5f);
    }

    void CleaningUi::Update(float DeltaTime)
    {
        const bool hideKeyPressed = InputManager::GetInstance().IsKeyPressed(GLFW_KEY_J)
                                    || InputManager::GetInstance().IsGamepadButtonPressed(GLFW_GAMEPAD_BUTTON_Y);

        if (hideKeyPressed && !HideKeyPressedLastFrame)
        {
            Hidden = !Hidden;
            AudioManager::GetInstance().StopSound(ListSound);
            AudioManager::GetInstance().PlayAudio(ListSound);
        }

        HideKeyPressedLastFrame = hideKeyPressed;

        HiddenTime += (Hidden ? 1.0f : -1.0f) * DeltaTime / 0.66f;
        HiddenTime = std::min<float>(std::max<float>(HiddenTime, 0.0f), 1.0f);
        constexpr glm::vec a = glm::vec3(600, 0, 0);
        constexpr glm::vec b = glm::vec3(1280, 0, 0);
        const float t = Math::EaseInOutBack(HiddenTime);
        TaskListBackground->GetRect().SetPositionPixels(a + t * (b - a));

        bool taskCompleted = false;

        if (const BloodManager* bloodManager = BloodManager::GetCurrent())
        {
            const float bloodFill = bloodManager->GetBloodFill();
            if (bloodFill > ReferenceBloodFill)
            {
                ReferenceBloodFill = bloodFill;
            }

            const float cleanedPercent = (1.0f - bloodFill / ReferenceBloodFill) * 100.0f;
            FloorProgressText->SetText(std::format("({:.2f}%)", cleanedPercent));

            if (cleanedPercent >= 90.0f && !FloorTaskCompleted)
            {
                AudioManager::GetInstance().PlayAudio(TaskSound);
                FloorTaskCompleted = true;
            }
        }

        if (ThrashManager* trashManager = ThrashManager::GetInstance())
        {
            const int32_t trashCount = trashManager->GetThrashCount();
            if (trashCount > ReferenceTrashCount)
            {
                ReferenceTrashCount = trashCount;
            }

            TrashProgressText->SetText(std::format("({:02}/{:02})", ReferenceTrashCount - trashCount, ReferenceTrashCount));

            if (trashCount >= ReferenceTrashCount && !TrashTaskCompleted)
            {
                AudioManager::GetInstance().PlayAudio(TaskSound);
                TrashTaskCompleted = true;
            }

            WeaponProgressText->SetText(std::format("({:02}/{:02})", trashManager->GetCleanedUpWeaponCount(),trashManager->GetWeaponCount()));
            BooksProgressText->SetText(std::format("({:02}/{:02})", trashManager->GetCleanedUpBookCount(), trashManager->GetBookCount()));
            CoinsProgressText->SetText(std::format("({:02}/{:02})", trashManager->GetCleanedUpCoinCount(), trashManager->GetCoinCount()));
        }
    }
}
