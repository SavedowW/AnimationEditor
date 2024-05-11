#include "GuidelineViewer.h"

GuidelineViewer::GuidelineViewer(const Vector2<float> lvlSize_, GuidelinesManager *guidelineManager_) :
    m_lvlSize(lvlSize_),
    m_guidelineManager(guidelineManager_)
{
    m_guidelines = m_guidelineManager->getGuidelines();
}

void GuidelineViewer::proceed()
{
    ImGui::Checkbox("Visual editing", &m_editing);

    if (ImGui::BeginTable("Frame data", 4,  ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchSame))
    {
        ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 16);
        ImGui::TableSetupColumn("", 0, 100);
        ImGui::TableSetupColumn("", 0, 50);
        ImGui::TableSetupColumn("", 0, 50);

        for (int i = 0; i < m_guidelines.size(); ++i)
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            std::string s = (m_guidelines[i].m_axis == Guideline::LineAxis::HORIZONTAL ? "H" : "V");
            if (m_guidelines[i].m_dirtyflag)
                s += '*';
            ImGui::Text(s.c_str());

            ImGui::TableSetColumnIndex(1);
            if (ImGui::DragFloat((std::string("Coord##") + std::to_string(i)).c_str(), &m_guidelines[i].m_coord))
            {
                std::cout << i << std::endl;
                m_guidelines[i].m_dirtyflag = true;
            }

            ImGui::TableSetColumnIndex(2);
            if (ImGui::Button((std::string("Rotate##") + std::to_string(i)).c_str()))
            {
                m_guidelines[i].m_axis = Guideline::LineAxis(1 - (int)m_guidelines[i].m_axis);
                m_guidelines[i].m_dirtyflag = true;
            }

            ImGui::TableSetColumnIndex(3);
            if (ImGui::Button((std::string("Delete##") + std::to_string(i)).c_str()))
            {
                m_guidelineManager->deleteGuideline(m_guidelines[i].m_id);
                m_guidelines.erase(m_guidelines.begin() + i);
            }
        }
        ImGui::EndTable();
    }

    if (ImGui::Button("+"))
    {
        auto newid = m_guidelineManager->createGuideline();
        Guideline gl;
        gl.m_id = newid;
        gl.m_dirtyflag = true;
        gl.m_axis = Guideline::LineAxis::VERTICAL;
        gl.m_coord = m_lvlSize.x / 2 + 10.0f;
        m_guidelines.push_back(gl);
    }
    ImGui::SameLine();
    if (ImGui::Button("Update all"))
    {
        for (auto &el : m_guidelines)
        {
            if (el.m_dirtyflag)
            {
                m_guidelineManager->update(el);
                el.m_dirtyflag = false;
            }
        }
    }
}

void GuidelineViewer::addGuideline(const Guideline &guideline_)
{
    m_guidelines.push_back(guideline_);
}

void GuidelineViewer::updateMousePos(const Vector2<float> mouseWorldPos_)
{
    if (m_editing && !m_attached)
    {
        float minrange = -1;
        int id = 0;
        for (int i = 0; i < m_guidelines.size(); ++i)
        {
            float range = 0;
            if (m_guidelines[i].m_axis == Guideline::LineAxis::HORIZONTAL)
                range = abs(m_guidelines[i].m_coord - mouseWorldPos_.y);
            else
                range = abs(m_guidelines[i].m_coord - mouseWorldPos_.x);

            if (minrange == -1 || range < minrange)
            {
                minrange = range;
                id = i;
            }
        }

        if (minrange != -1 && minrange <= m_maxRange)
            m_closestLineIndex = id;
        else
            m_closestLineIndex = -1;
    }
    else if (m_attached && m_closestLineIndex != -1)
    {
        if (m_guidelines[m_closestLineIndex].m_axis == Guideline::LineAxis::HORIZONTAL)
            m_guidelines[m_closestLineIndex].m_coord = mouseWorldPos_.y;
        else
            m_guidelines[m_closestLineIndex].m_coord = mouseWorldPos_.x;
    }
}

void GuidelineViewer::attachLine()
{
    if (m_editing && m_closestLineIndex != -1)
    {
        m_attached = true;
        m_guidelines[m_closestLineIndex].m_dirtyflag = true;
    }
}

void GuidelineViewer::detachLine()
{
    m_attached = false;
}

void GuidelineViewer::draw(Renderer &renderer_, Camera &cam_)
{
    for (int i = 0; i < m_guidelines.size(); ++i)
    {
        auto &line = m_guidelines[i];
        SDL_Color col = (i == m_closestLineIndex ? SDL_Color{100, 100, 255, 255} : SDL_Color{0, 0, 200, 255});
        if (line.m_axis == Guideline::LineAxis::HORIZONTAL)
        {
            renderer_.drawLine({0, line.m_coord}, {m_lvlSize.x, line.m_coord}, col, cam_);
        }
        else
        {
            renderer_.drawLine({line.m_coord, 0.0f}, {line.m_coord, m_lvlSize.y}, col, cam_);
        }
    }

}
