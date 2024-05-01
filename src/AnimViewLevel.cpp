#include "AnimViewLevel.h"
#include "Application.h"
#include <algorithm>

std::string findNextFile(const std::filesystem::path &dir_, const std::string &currentFile)
{
    std::string firstItem = "";
    bool passedCurrent = false;
    for (const auto &el : std::filesystem::directory_iterator(dir_))
    {
        auto filename = el.path().filename().string();

        if (firstItem == "")
            firstItem = filename;
        
        if (filename == currentFile)
            passedCurrent = true;
        else if (passedCurrent)
            return filename;
    }

    return firstItem;
}

int FileTabCompletition(ImGuiInputTextCallbackData* data_)
{
    std::string selected = data_->Buf;
    
    if (std::filesystem::exists(selected))
    {
        std::filesystem::path selobj(selected);
        if (std::filesystem::is_directory(selobj) && (selected[selected.size() - 1] == '/' || selected[selected.size() - 1] == '\\'))
        {
            for (const auto &el : std::filesystem::directory_iterator(selected))
            {
                auto entryFilename = el.path().filename().string();
                data_->InsertChars(data_->BufTextLen, entryFilename.c_str());
                break;
            }
        }
        else
        {
            std::filesystem::path origpath(selected);
            std::filesystem::path par = origpath.parent_path();
            std::string filename = origpath.filename().string();
            auto nextItem = findNextFile(par, filename);
            data_->DeleteChars(par.string().size() + 1, filename.size());
            data_->InsertChars(data_->BufTextLen, nextItem.c_str());
        }
    }
    else
    {
        std::filesystem::path origpath(selected);
        std::filesystem::path par = origpath.parent_path();
        std::string filename = origpath.filename().string();
        if (std::filesystem::exists(par))
        {
            for (const auto &el : std::filesystem::directory_iterator(par))
            {
                auto entryFilename = el.path().filename().string();
                if (entryFilename.find(filename) == 0)
                {
                    data_->DeleteChars(par.string().size() + 1, filename.size());
                    data_->InsertChars(data_->BufTextLen, entryFilename.c_str());
                    break;
                }
            }
        }
    }

    return 0;
}

AnimViewLevel::AnimViewLevel(Application *application_, const Vector2<float> &size_, int lvlId_) :
    Level(application_, size_, lvlId_),
    m_camera(gamedata::stages::startingCameraPos, {gamedata::global::cameraWidth, gamedata::global::cameraHeight}, m_size),
    m_db(std::shared_ptr<DBAccessor>(new DBAccessor("editor.db")))
{
    subscribe(EVENTS::RMB);
    subscribe(EVENTS::MOUSE_MOVEMENT);
    subscribe(EVENTS::WHEEL_SCROLL);
}

void AnimViewLevel::enter()
{
    Level::enter();

    m_stage = SelectionStage::SELECT_PATH;

    m_camera.setPos(gamedata::stages::startingCameraPos);
    m_camera.setScale(gamedata::stages::startingCameraScale);

    auto path = m_application->getBasePath();

    m_anim = new EngineAnimation();

    updateLastFilesList();
}

void AnimViewLevel::receiveInput(EVENTS event, const float scale_)
{
    Level::receiveInput(event, scale_);
    
    if (event == EVENTS::RMB)
    {
        if (scale_ == 1.0f)
            m_holdingCamera = true;
        else
            m_holdingCamera = false;
    }
    else if (event == EVENTS::WHEEL_SCROLL)
    {
        m_camera.setScale(utils::clamp(m_camera.getScale() + scale_ * -0.1f, gamedata::stages::minCameraScale, gamedata::stages::maxCameraScale));
    }
}

void AnimViewLevel::receiveMouseMovement(const Vector2<float> &offset_)
{
    if (m_holdingCamera)
    {
        m_camera.setPos(m_camera.getPos() - offset_);
    }
}

void AnimViewLevel::setDirectory(const std::string &path_)
{
    m_originalPath = path_;
    while (m_originalPath[m_originalPath.size() - 1] == '/' || m_originalPath[m_originalPath.size() - 1] == '\\')
        m_originalPath.pop_back();

	for (const auto &entry : std::filesystem::directory_iterator(path_))
	{
		std::filesystem::path dirpath = entry.path();
        if (entry.is_regular_file() && (dirpath.extension() == ".png" || dirpath.extension() == ".bmp"))
        {
            std::cout << "Found: " << dirpath.filename() << std::endl;
            m_sprites.push_back(dirpath);
        }
	}

    m_stage = SelectionStage::REORDER_SPRITES;

    std::filesystem::path dirpath(path_);
    auto filename = dirpath.filename().string() + ".panm";
    filename.copy(m_filename, 1024);
}

void AnimViewLevel::setAnimFile(const std::string &path_)
{
    Renderer *renderer = m_application->getRenderer();

    std::filesystem::path dirpath(path_);
    m_originalPath = dirpath.parent_path().string();

    m_anim->loadAnimation(path_, *renderer);

    m_stage = SelectionStage::EDIT_ANIM;

    m_animFrames.setProps(&m_anim->m_framesData);
    m_animFrames.setLimits(0, m_anim->m_surfaces.size() - 1);

    auto filename = dirpath.filename().string();
    filename.copy(m_filename, 1024);

    m_db.pushFile(path_, filename);
}

void AnimViewLevel::returnToStart()
{
    m_sprites.clear();
    delete m_anim;
    m_anim = new EngineAnimation();
    m_stage = SelectionStage::SELECT_PATH;
    updateLastFilesList();
}

void AnimViewLevel::updateLastFilesList()
{
    m_lastFiles = m_db.getFileManager().getLastFiles(10);
}

AnimViewLevel::~AnimViewLevel()
{
    
}

void AnimViewLevel::update()
{
    m_camera.update();

    ImGui_ImplSDLRenderer2_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    if (m_stage == SelectionStage::SELECT_PATH)
    {
        ImGui::Begin("Sprite order", &m_winOpen);

        ImGui::SeparatorText("Create new animation");
        static char dirname[1024];
        static char filename[1024];

        ImGui::PushItemWidth(400);
        ImGui::InputText("Folder path", dirname, 1024, ImGuiInputTextFlags_CallbackCompletion, FileTabCompletition);
        if (ImGui::Button("Load sprites"))
        {
            setDirectory(std::string(dirname));
        }

        ImGui::SeparatorText("Load existing animation");

        ImGui::PushItemWidth(400);
        ImGui::InputText("*.panm file path", filename, 1024, ImGuiInputTextFlags_CallbackCompletion, FileTabCompletition);
        if (ImGui::Button("Load animation"))
        {
            setAnimFile(std::string(filename));
        }

        ImGui::SeparatorText("Existing animations");
        for (auto &el : m_lastFiles)
        {
            if (ImGui::Selectable(el.filepath.c_str()))
                setAnimFile(el.filepath);
        }

        ImGui::End();
    }
    else if (m_stage == SelectionStage::EDIT_ANIM)
    {
        if (m_runAnimation)
            currentFrame++;

        if (currentFrame >= m_anim->m_duration)
        {
            if (m_loopAnimation)
                currentFrame = 0;
            else
            {
                currentFrame = m_anim->m_duration - 1;
                m_runAnimation = false;
            }
        }

        ImGui::Begin("Editor", &m_winOpen);

        ImGui::SeparatorText("Animation data");

        int animId = 0;
        if (m_anim)
            animId = m_anim->m_framesData[currentFrame % m_anim->m_duration];
        ImGui::SliderInt((std::string("Frame [") + std::to_string(animId) + "]").c_str(), &currentFrame, 0, (!m_anim ? 0 : m_anim->m_duration - 1));

        if (ImGui::Button("Start"))
        {
            m_runAnimation = true;

            if (currentFrame >= m_anim->m_duration - 1)
                currentFrame = 0;
        }

        ImGui::SameLine();
        if (ImGui::Button("Stop"))
        {
            m_runAnimation = false;
        }

        ImGui::SameLine();
        ImGui::Checkbox("Loop animation", &m_loopAnimation);

        ImGui::SeparatorText("Animation frames");

        m_animFrames.proceed(currentFrame);

        if (ImGui::InputInt("Duration", &m_anim->m_duration))
        {
            if (m_anim->m_duration < 1)
                m_anim->m_duration = 1;
        }

        ImGui::SeparatorText("Size");

        static int height = m_anim->m_height;
        if (ImGui::DragInt("Height", &height))
        {
            if (height < 1)
                height = 1;

            m_anim->scaleToHeight(height);
        }

        if (ImGui::DragInt("Width", &m_anim->m_width))
        {
            if (m_anim->m_width < 1)
                m_anim->m_width = 1;
        }

        ImGui::SeparatorText("Origin");

        ImGui::DragFloat("X", &m_anim->m_origin.x);
        ImGui::SameLine();
        ImGui::DragFloat("Y", &m_anim->m_origin.y);

        if (ImGui::Button("7", {28, 28}))
            m_anim->m_origin = {0, 0};
        ImGui::SameLine(0.0f, 4.0f);
        if (ImGui::Button("8", {28, 28}))
            m_anim->m_origin = {m_anim->m_width / 2.0f, 0};
        ImGui::SameLine(0.0f, 4.0f);
        if (ImGui::Button("9", {28, 28}))
            m_anim->m_origin = {(float)m_anim->m_width, 0};

        if (ImGui::Button("4", {28, 28}))
            m_anim->m_origin = {0, m_anim->m_height / 2.0f};
        ImGui::SameLine(0.0f, 4.0f);
        if (ImGui::Button("5", {28, 28}))
            m_anim->m_origin = {m_anim->m_width / 2.0f, m_anim->m_height / 2.0f};
        ImGui::SameLine(0.0f, 4.0f);
        if (ImGui::Button("6", {28, 28}))
            m_anim->m_origin = {(float)m_anim->m_width, m_anim->m_height / 2.0f};

        if (ImGui::Button("1", {28, 28}))
            m_anim->m_origin = {0, (float)m_anim->m_height};
        ImGui::SameLine(0.0f, 4.0f);
        if (ImGui::Button("2", {28, 28}))
            m_anim->m_origin = {m_anim->m_width / 2.0f, (float)m_anim->m_height};
        ImGui::SameLine(0.0f, 4.0f);
        if (ImGui::Button("3", {28, 28}))
            m_anim->m_origin = {(float)m_anim->m_width, (float)m_anim->m_height};

        ImGui::PushItemWidth(300);
        ImGui::SeparatorText("Output");
        ImGui::InputText("File name", m_filename, 1024);
        if (ImGui::Button("Export"))
        {
            std::string flnm(m_filename);
            std::cout << flnm << std::endl;
            m_anim->saveAnimation(m_originalPath + "/" + m_filename, 5, 1.2);
            std::cout << "SAVED\n";
            m_db.pushFile(m_originalPath + "/" + m_filename, flnm);
        }

        ImGui::SeparatorText("Misc");
        if (ImGui::Button("Return"))
        {
            returnToStart();
        }

        ImGui::End();
    }
    else if (m_stage == SelectionStage::REORDER_SPRITES)
    {
        ImGui::Begin("Sprite order", &m_winOpen);

        if (ImGui::BeginTable("Sprite order tbl", 4,  ImGuiTableFlags_SizingStretchSame))
        {
            ImGui::TableSetupColumn("Filename", ImGuiTableColumnFlags_WidthFixed);
            ImGui::TableSetupColumn("MoveUp");
            ImGui::TableSetupColumn("MoveDown");
            ImGui::TableSetupColumn("filler", 0);
            for (int i = 0; i < m_sprites.size(); ++i)
            {
                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex(0);
                ImGui::Text(m_sprites[i].filename().string().c_str());

                ImGui::TableSetColumnIndex(1);
                if (ImGui::Button((std::string("Move up##") + std::to_string(i)).c_str()))
                {
                    if (i != 0)
                        std::swap(m_sprites[i], m_sprites[i - 1]);
                }

                ImGui::TableSetColumnIndex(2);
                if (ImGui::Button((std::string("Move down##") + std::to_string(i)).c_str()))
                {
                    if (i != m_sprites.size() - 1)
                        std::swap(m_sprites[i], m_sprites[i + 1]);
                }
            }
            ImGui::EndTable();
        }

        if (ImGui::Button("Create animation"))
        {
            Renderer *renderer = m_application->getRenderer();

            for (auto &el : m_sprites)
                m_anim->addFrame(el.string(), *renderer);

            m_stage = SelectionStage::EDIT_ANIM;

            m_animFrames.setProps(&m_anim->m_framesData);
            m_animFrames.setLimits(0, m_anim->m_surfaces.size() - 1);
        }

        ImGui::End();
    }
    
}

void AnimViewLevel::draw()
{
    ImGui::Render();

    auto &renderer = *m_application->getRenderer();
    renderer.fillRenderer(SDL_Color{ 25, 25, 25, 255 });

    renderer.drawRectangle({0, gamedata::stages::levelOfGround}, {m_size.x, 2}, {0, 0, 200, 255}, m_camera);
    renderer.drawRectangle({m_size.x / 2.0f, 0.0f}, {2.0f, m_size.y}, {0, 0, 200, 255}, m_camera);
    renderer.drawRectangle({0, gamedata::stages::levelOfGround - 350}, {m_size.x, 1}, {0, 0, 200, 255}, m_camera);
    renderer.drawRectangle({0, gamedata::stages::levelOfGround - 175}, {m_size.x, 1}, {0, 0, 200, 255}, m_camera);

    renderer.drawRectangle({m_size.x / 2.0f - gamedata::global::cameraWidth * gamedata::stages::minCameraScale / 2.0f, gamedata::stages::stageHeight - gamedata::global::cameraHeight * gamedata::stages::minCameraScale - 1},
    {gamedata::global::cameraWidth * gamedata::stages::minCameraScale, gamedata::global::cameraHeight * gamedata::stages::minCameraScale}, {255, 255, 0, 255}, m_camera);
    
    renderer.drawRectangle({m_size.x / 2.0f - gamedata::global::cameraWidth * gamedata::stages::maxCameraScale / 2.0f, gamedata::stages::stageHeight - gamedata::global::cameraHeight * gamedata::stages::maxCameraScale - 1},
    {gamedata::global::cameraWidth * gamedata::stages::maxCameraScale, gamedata::global::cameraHeight * gamedata::stages::maxCameraScale}, {255, 255, 0, 255}, m_camera);

    if (m_anim->m_frameCount > 0)
        renderer.renderTexture((*m_anim)[currentFrame], m_size.x / 2.0f - m_anim->m_origin.x, gamedata::stages::levelOfGround - m_anim->m_origin.y, m_anim->m_width, m_anim->m_height, m_camera, 0, SDL_FLIP_NONE);

    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData());

    renderer.updateScreen();
}
