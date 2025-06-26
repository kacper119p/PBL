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

//TODO: dac warunki do lvli jak juz beda

namespace Engine::Ui
{
    CleaningUi::CleaningUi()
    {
        if (!ThrashManager::GetInstance()->IsCurrentLevelCompleted())
        {
            SetupDuringLevel();
            DialogueWidget = AddElement<class DialogueWidget>(nullptr);
            DialogueStart();
        }
        else
        {
            SetupAfterLevel();
        }
        PicturesLoaded = false;
    }

    void CleaningUi::Update(float DeltaTime)
    {
        if (ThrashManager::GetInstance()->IsCurrentLevelCompleted())
        {
            if (!PicturesLoaded)
            {
                SetupAfterLevel();
                switch (ThrashManager::GetInstance()->GetPlayerGrade())
                {
                    case (6):
                    {
                        Grade = AddElement<Image>(SummaryBackground);
                        Grade->GetRect().SetPositionPixels(glm::vec3(300, -100, 0));
                        Grade->GetRect().SetSizePixels(glm::vec2(0, 0));
                        Materials::UiMaterial* gradeSMaterial = new Materials::BasicImageMaterial();
                        Grade->SetMaterial(gradeSMaterial);
                        Grade->SetTexture(TextureManager::GetTexture("./res/textures/CleaningUi/GradeS.dds"));
                    }
                    break;
                    case (5):
                    {
                        Grade = AddElement<Image>(SummaryBackground);
                        Grade->GetRect().SetPositionPixels(glm::vec3(300, -100, 0));
                        Grade->GetRect().SetSizePixels(glm::vec2(0, 0));
                        Materials::UiMaterial* gradeAMaterial = new Materials::BasicImageMaterial();
                        Grade->SetMaterial(gradeAMaterial);
                        Grade->SetTexture(TextureManager::GetTexture("./res/textures/CleaningUi/GradeA.dds"));
                    }
                    break;
                    case (4):
                    {
                        Grade = AddElement<Image>(SummaryBackground);
                        Grade->GetRect().SetPositionPixels(glm::vec3(300, -100, 0));
                        Grade->GetRect().SetSizePixels(glm::vec2(0, 0));
                        Materials::UiMaterial* gradeBMaterial = new Materials::BasicImageMaterial();
                        Grade->SetMaterial(gradeBMaterial);
                        Grade->SetTexture(TextureManager::GetTexture("./res/textures/CleaningUi/GradeB.dds"));
                    }
                    break;
                    case (3):
                    {
                        Grade = AddElement<Image>(SummaryBackground);
                        Grade->GetRect().SetPositionPixels(glm::vec3(300, -100, 0));
                        Grade->GetRect().SetSizePixels(glm::vec2(0, 0));
                        Materials::UiMaterial* gradeCMaterial = new Materials::BasicImageMaterial();
                        Grade->SetMaterial(gradeCMaterial);
                        Grade->SetTexture(TextureManager::GetTexture("./res/textures/CleaningUi/GradeC.dds"));
                    }
                    break;
                    case (2):
                    {
                        Grade = AddElement<Image>(SummaryBackground);
                        Grade->GetRect().SetPositionPixels(glm::vec3(300, -100, 0));
                        Grade->GetRect().SetSizePixels(glm::vec2(0, 0));
                        Materials::UiMaterial* gradeDMaterial = new Materials::BasicImageMaterial();
                        Grade->SetMaterial(gradeDMaterial);
                        Grade->SetTexture(TextureManager::GetTexture("./res/textures/CleaningUi/GradeD.dds"));
                    }
                    break;
                }
                PicturesLoaded = true;
            }
        }
        if (!ThrashManager::GetInstance()->IsCurrentLevelCompleted())
        {
            const bool hideKeyPressed = InputManager::GetInstance().IsKeyPressed(GLFW_KEY_J) ||
                                        InputManager::GetInstance().IsGamepadButtonPressed(GLFW_GAMEPAD_BUTTON_Y);

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
            if (TaskListBackground != NULL)
            TaskListBackground->GetRect().SetPositionPixels(a + t * (b - a));

            bool taskCompleted = false;
            if (ThrashManager::GetInstance()->GetCurrentLevel() == 2)
            {
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
            }

            if (ThrashManager* trashManager = ThrashManager::GetInstance())
            {
                const int32_t trashCount = trashManager->GetThrashCount();
                if (trashCount > ReferenceTrashCount)
                {
                    ReferenceTrashCount = trashCount;
                }

                TrashProgressText->SetText(
                        std::format("({:02}/{:02})", ReferenceTrashCount - trashCount, ReferenceTrashCount));

                if (trashCount >= ReferenceTrashCount && !TrashTaskCompleted)
                {
                    AudioManager::GetInstance().PlayAudio(TaskSound);
                    TrashTaskCompleted = true;
                }
                if (ThrashManager::GetInstance()->GetCurrentLevel() == 2)
                {
                    WeaponProgressText->SetText(std::format("({:02}/{:02})", trashManager->GetCleanedUpWeaponCount(),
                                                            trashManager->GetWeaponCount()));
                    BooksProgressText->SetText(std::format("({:02}/{:02})", trashManager->GetCleanedUpBookCount(),
                                                           trashManager->GetBookCount()));
                    CoinsProgressText->SetText(std::format("({:02}/{:02})", trashManager->GetCleanedUpCoinCount(),
                                                           trashManager->GetCoinCount()));

                    if (trashManager->IsBookTaskFailed())
                    {
                        DontBooks->SetColor(FailedColor);
                    }
                    else
                    {
                        DontBooks->SetColor(PositiveColor);
                    }
                    if (trashManager->IsFurnitureTaskFailed())
                    {
                        DontFurniture->SetColor(FailedColor);
                    }
                    else
                    {
                        DontFurniture->SetColor(PositiveColor);
                    }
                    if (trashManager->IsCoinTaskFailed())
                    {
                        DontCoins->SetColor(FailedColor);
                    }
                    else
                    {
                        DontCoins->SetColor(PositiveColor);
                    }
                    if (trashManager->IsWeaponTaskFailed())
                    {
                        DontWeapons->SetColor(FailedColor);
                    }
                    else
                    {
                        DontWeapons->SetColor(PositiveColor);
                    }
                }
            }
        }
        else
        {
            if (!SummaryAppeared)
            {
                ThrashManager* trashManager = ThrashManager::GetInstance();
                if (const BloodManager* bloodManager = BloodManager::GetCurrent())
                    SummaryAnimationTime += DeltaTime / 2.0f;
                float t = std::min(SummaryAnimationTime, 2.0f);
                float eased = Math::EaseInOutExpo(t);
                glm::vec3 startPos = glm::vec3(0, 0, 0);
                glm::vec3 endPos = glm::vec3(-1920, 0, 0);
                SummaryBackground->GetRect().SetPositionPixels(startPos + (1 - eased) * (endPos - startPos));

                if (t >= 1.0f)
                {
                    SummaryAppeared = true;
                }
                TrashProgressText->SetText(std::format("({:02}/{:02})", ReferenceTrashCount, ReferenceTrashCount));
                if (ThrashManager::GetInstance()->GetCurrentLevel() == 2)
                {
                    WeaponProgressText->SetText(std::format("({:02}/{:02})", trashManager->GetWeaponCount(),
                                                            trashManager->GetWeaponCount()));
                    BooksProgressText->SetText(
                            std::format("({:02}/{:02})", trashManager->GetBookCount(), trashManager->GetBookCount()));
                    CoinsProgressText->SetText(
                            std::format("({:02}/{:02})", trashManager->GetCoinCount(), trashManager->GetCoinCount()));
                    FloorProgressText->SetText("(100.00)%");

                    if (trashManager->IsBookTaskFailed())
                    {
                        DontBooks->SetColor(FailedColor);
                    }
                    else
                    {
                        DontBooks->SetColor(PositiveColor);
                    }
                    if (trashManager->IsFurnitureTaskFailed())
                    {
                        DontFurniture->SetColor(FailedColor);
                    }
                    else
                    {
                        DontFurniture->SetColor(PositiveColor);
                    }
                    if (trashManager->IsCoinTaskFailed())
                    {
                        DontCoins->SetColor(FailedColor);
                    }
                    else
                    {
                        DontCoins->SetColor(PositiveColor);
                    }
                    if (trashManager->IsWeaponTaskFailed())
                    {
                        DontWeapons->SetColor(FailedColor);
                    }
                    else
                    {
                        DontWeapons->SetColor(PositiveColor);
                    }
                }

            }
            if (SummaryAppeared && Grade && !GradeAnimationStarted && !GradeAnimationFinished)
            {
                GradeAnimationStarted = true;
                GradeAnimationTime = 0.0f;
                Grade->GetRect().SetSizePixels(glm::vec2(10000, 10000));
            }

            if (GradeAnimationStarted && !GradeAnimationFinished)
            {
                GradeAnimationTime += DeltaTime;
                float animDuration = 1.0f;
                float t = std::min(GradeAnimationTime / animDuration, 1.0f);
                float eased = Math::EaseOutBounce(t);
                float currentSize = 800.0f - 288.0f * eased;
                Grade->GetRect().SetSizePixels(glm::vec2(currentSize, currentSize));

                if (t >= 1.0f)
                {
                    GradeAnimationStarted = false;
                    GradeAnimationFinished = true;
                }
            }


        }
        if (DialogueWidget!=NULL)
        DialogueUpdate(DeltaTime);
    }

    void CleaningUi::SetupDuringLevel()
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

        TrashProgressText = AddElement<Text>(TaskListBackground);
        TrashProgressText->GetRect().SetPositionPixels(glm::vec3(200, 270, 0));
        TrashProgressText->GetRect().SetSizePixels(glm::vec2(700, TaskListFontSize));
        TrashProgressText->SetFont("EagleLakeRegular");
        TrashProgressText->SetText("(xx/xx)");

        if (ThrashManager::GetInstance()->GetCurrentLevel()==2)
        {
            FloorText = AddElement<Text>(TaskListBackground);
            FloorText->GetRect().SetPositionPixels(glm::vec3(-150, 230, 0));
            FloorText->GetRect().SetSizePixels(glm::vec2(700, TaskListFontSize));
            FloorText->SetFont("EagleLakeRegular");
            FloorText->SetText("Mop the floor");

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
        }

        AudioManager::GetInstance().ConfigureSoundAttenuation(ListSound, 1.0f, 100.0f, 0.0f);
        AudioManager::GetInstance().ConfigureSoundAttenuation(TaskSound, 1.0f, 100.0f, 0.0f);
        AudioManager::GetInstance().SetVolume(ListSound, 0.5f);
        AudioManager::GetInstance().SetVolume(TaskSound, 0.5f);
    }

    void CleaningUi::SetupAfterLevel()
    {
        SummaryBackground = AddElement<Image>(nullptr);
        SummaryBackground->GetRect().SetPositionPixels(glm::vec3(1920, 0, 0));
        SummaryBackground->GetRect().SetSizePixels(glm::vec2(1920, 1080));
        Materials::UiMaterial* imageMaterial = new Materials::BasicImageMaterial();
        SummaryBackground->SetMaterial(imageMaterial);
        SummaryBackground->SetTexture(TextureManager::GetTexture("./res/textures/CleaningUi/SummaryBackgroud.dds"));

        TaskListFontSize = 35.0f;

        TrashText = AddElement<Text>(SummaryBackground);
        TrashText->GetRect().SetPositionPixels(glm::vec3(-650, 350, 0));
        TrashText->GetRect().SetSizePixels(glm::vec2(700, TaskListFontSize));
        TrashText->SetFont("EagleLakeRegular");
        TrashText->SetText("Dispose of the trash");

        

        TrashProgressText = AddElement<Text>(SummaryBackground);
        TrashProgressText->GetRect().SetPositionPixels(glm::vec3(-260, 350, 0));
        TrashProgressText->GetRect().SetSizePixels(glm::vec2(700, TaskListFontSize));
        TrashProgressText->SetFont("EagleLakeRegular");
        TrashProgressText->SetText("(xx/xx)");

        if (ThrashManager::GetInstance()->GetCurrentLevel() == 2)
        {
            FloorText = AddElement<Text>(SummaryBackground);
            FloorText->GetRect().SetPositionPixels(glm::vec3(-650, 310, 0));
            FloorText->GetRect().SetSizePixels(glm::vec2(700, TaskListFontSize));
            FloorText->SetFont("EagleLakeRegular");
            FloorText->SetText("Mop the floor");

            FloorProgressText = AddElement<Text>(SummaryBackground);
            FloorProgressText->GetRect().SetPositionPixels(glm::vec3(-260, 310, 0));
            FloorProgressText->GetRect().SetSizePixels(glm::vec2(700, TaskListFontSize));
            FloorProgressText->SetFont("EagleLakeRegular");
            FloorProgressText->SetText("(xx%)");

            WeaponText = AddElement<Text>(SummaryBackground);
            WeaponText->GetRect().SetPositionPixels(glm::vec3(-650, 265, 0));
            WeaponText->GetRect().SetSizePixels(glm::vec2(550, TaskListFontSize));
            WeaponText->SetFont("EagleLakeRegular");
            WeaponText->SetText("Put the weapons on the rack");

            WeaponProgressText = AddElement<Text>(SummaryBackground);
            WeaponProgressText->GetRect().SetPositionPixels(glm::vec3(-260, 265, 0));
            WeaponProgressText->GetRect().SetSizePixels(glm::vec2(700, TaskListFontSize));
            WeaponProgressText->SetFont("EagleLakeRegular");
            WeaponProgressText->SetText("(xx/xx)");

            BooksText = AddElement<Text>(SummaryBackground);
            BooksText->GetRect().SetPositionPixels(glm::vec3(-650, 160, 0));
            BooksText->GetRect().SetSizePixels(glm::vec2(550, TaskListFontSize));
            BooksText->SetFont("EagleLakeRegular");
            BooksText->SetText("Place the books back on the shelf");

            BooksProgressText = AddElement<Text>(SummaryBackground);
            BooksProgressText->GetRect().SetPositionPixels(glm::vec3(-260, 160, 0));
            BooksProgressText->GetRect().SetSizePixels(glm::vec2(700, TaskListFontSize));
            BooksProgressText->SetFont("EagleLakeRegular");
            BooksProgressText->SetText("(xx/xx)");

            CoinsText = AddElement<Text>(SummaryBackground);
            CoinsText->GetRect().SetPositionPixels(glm::vec3(-650, 55, 0));
            CoinsText->GetRect().SetSizePixels(glm::vec2(550, TaskListFontSize));
            CoinsText->SetFont("EagleLakeRegular");
            CoinsText->SetText("Put the coins in the chest");

            CoinsProgressText = AddElement<Text>(SummaryBackground);
            CoinsProgressText->GetRect().SetPositionPixels(glm::vec3(-260, 55, 0));
            CoinsProgressText->GetRect().SetSizePixels(glm::vec2(700, TaskListFontSize));
            CoinsProgressText->SetFont("EagleLakeRegular");
            CoinsProgressText->SetText("(xx/xx)");


            DontFurniture = AddElement<Text>(SummaryBackground);
            DontFurniture->GetRect().SetPositionPixels(glm::vec3(-650, -90, 0));
            DontFurniture->GetRect().SetSizePixels(glm::vec2(1000, TaskListFontSize));
            DontFurniture->SetFont("EagleLakeRegular");
            DontFurniture->SetText("Do not dispose of the furniture");

            DontBooks = AddElement<Text>(SummaryBackground);
            DontBooks->GetRect().SetPositionPixels(glm::vec3(-650, -140, 0));
            DontBooks->GetRect().SetSizePixels(glm::vec2(1000, TaskListFontSize));
            DontBooks->SetFont("EagleLakeRegular");
            DontBooks->SetText("Do not dispose of the books");

            DontCoins = AddElement<Text>(SummaryBackground);
            DontCoins->GetRect().SetPositionPixels(glm::vec3(-650, -190, 0));
            DontCoins->GetRect().SetSizePixels(glm::vec2(1000, TaskListFontSize));
            DontCoins->SetFont("EagleLakeRegular");
            DontCoins->SetText("Do not throw out any coins");

            DontWeapons = AddElement<Text>(SummaryBackground);
            DontWeapons->GetRect().SetPositionPixels(glm::vec3(-650, -240, 0));
            DontWeapons->GetRect().SetSizePixels(glm::vec2(1000, TaskListFontSize));
            DontWeapons->SetFont("EagleLakeRegular");
            DontWeapons->SetText("Do not throw out any weapons");

            Frame = AddElement<Image>(SummaryBackground);
            Frame->GetRect().SetPositionPixels(glm::vec3(-380, 0, 0));
            Frame->GetRect().SetSizePixels(glm::vec2(665, 760));
            Materials::UiMaterial* frameMaterial = new Materials::BasicImageMaterial();
            Frame->SetMaterial(frameMaterial);
            Frame->SetTexture(TextureManager::GetTexture("./res/textures/CleaningUi/Frame.dds"));

            TimerText = AddElement<Text>(SummaryBackground);
            TimerText->GetRect().SetPositionPixels(glm::vec3(20, 420, 0));
            TimerText->GetRect().SetSizePixels(glm::vec2(800, 70));
            TimerText->SetFont("EagleLakeRegular");
            TimerText->SetText(
                    std::format("Completed in: {:.02f}", ThrashManager::GetInstance()->GetLevelEndTime() -
                                                                 ThrashManager::GetInstance()->GetLevelStartTime()));
            TimerText = AddElement<Text>(SummaryBackground);
            TimerText->GetRect().SetPositionPixels(glm::vec3(20, 420, 0));
            TimerText->GetRect().SetSizePixels(glm::vec2(800, 70));
            TimerText->SetFont("EagleLakeRegular");
            TimerText->SetText(
                    std::format("Completed in: {:.02f}", ThrashManager::GetInstance()->GetLevelEndTime() -
                                                                 ThrashManager::GetInstance()->GetLevelStartTime()));
        }

        switch (ThrashManager::GetInstance()->GetPlayerGrade())
        {
            case (6):
            {
                Grade = AddElement<Image>(SummaryBackground);
                Grade->GetRect().SetPositionPixels(glm::vec3(300, -100, 0));
                Grade->GetRect().SetSizePixels(glm::vec2(0, 0));
                Materials::UiMaterial* gradeSMaterial = new Materials::BasicImageMaterial();
                Grade->SetMaterial(gradeSMaterial);
                Grade->SetTexture(TextureManager::GetTexture("./res/textures/CleaningUi/GradeS.dds"));

            }
            break;
        }

    }
}


void Engine::Ui::CleaningUi::DialogueStart()
{
    switch (ThrashManager::GetInstance()->GetCurrentLevel())
    {
        case 1:
        {
            DialogueWidget->Show();
            DialogueWidget->PushLine("What now?", DialogueWidget::Speaker::Player);
            DialogueWidget->PushLine(
                    "Finally, you're here, Gryzia. Come closer, we need to clean up the mess left by the hero.",
                    DialogueWidget::Speaker::Boss);
            DialogueWidget->PushLine("Start by throwing away the mess he made to that pit.",
                                     DialogueWidget::Speaker::Boss);
            DialogueWidget->PushLine("Don't screw this up...", DialogueWidget::Speaker::Boss);
            break;
        }
        case 2:
        {
            printf("level 2");
            DialogueWidget->Show();
            DialogueWidget->PushLine("What a mess!", DialogueWidget::Speaker::Player);
            DialogueWidget->PushLine(
                    "There is a lot of blood around here. I better mop it up!",
                    DialogueWidget::Speaker::Player);
            break;
        }
        default:
        {
            break;
        }
    }
}

void Engine::Ui::CleaningUi::DialogueUpdate(float DeltaTime)
{
    switch (ThrashManager::GetInstance()->GetCurrentLevel())
    {
        case 1:
        {
            DialogueWidget->PopLine();
            if (DialogueWidget->IsAnimationFinished())
            {
                DialogueWidget->Hide();
            }
            break;
        }
        case 2:
        {
            DialogueWidget->PopLine();
            if (DialogueWidget->IsAnimationFinished())
            {
                DialogueWidget->Hide();
            }
            break;
        }
        default:
        {
            break;
        }
    }
    DialogueWidget->Update(DeltaTime);
}
