#include "ColliderViewer.h"

void ColliderViewer::setFile(const std::string &filepath_)
{
    m_colliderGroups = m_db->getColliders(filepath_);
    m_shown = true;
}

void ColliderViewer::resetFile()
{
    m_shown = false;
}

ColliderViewer::ColliderViewer(Vector2<float> animPos_, DBManager *dbManager_) :
    m_db(dbManager_),
    m_animPos(animPos_)
{
}

void ColliderViewer::proceed()
{
    if (!m_shown)
        return;

    ImGui::SetNextWindowPos(ImVec2(1250, 520));
    ImGui::SetNextWindowSize(ImVec2(340, 360));

    ImGui::Begin("Collider editing", &m_shown);

    for (int gpi = 0; gpi < m_colliderGroups.size(); ++gpi)
    {
        auto &gp = m_colliderGroups[gpi];
        ImGui::SeparatorText(gp.m_name.c_str());

        for (int cldi = 0; cldi < gp.m_colliders.size(); ++cldi)
        {
            auto &cld = gp.m_colliders[cldi];
            ImGui::PushItemWidth(64);
            if (ImGui::DragFloat((std::string("x##_") + std::to_string(gpi) + "_" + std::to_string(cldi)).c_str(), &cld.m_pos.x))
                cld.m_dirtyflag = true;
            ImGui::SameLine();
            if (ImGui::DragFloat((std::string("y##_") + std::to_string(gpi) + "_" + std::to_string(cldi)).c_str(), &cld.m_pos.y))
                cld.m_dirtyflag = true;
            ImGui::SameLine();
            if (ImGui::DragFloat((std::string("w##_") + std::to_string(gpi) + "_" + std::to_string(cldi)).c_str(), &cld.m_size.x))
                cld.m_dirtyflag = true;
            ImGui::SameLine();
            if (ImGui::DragFloat((std::string("h##_") + std::to_string(gpi) + "_" + std::to_string(cldi)).c_str(), &cld.m_size.y))
                cld.m_dirtyflag = true;
        }
    }

    ImGui::End();
}

void ColliderViewer::updateMousePos(const Vector2<float> mouseWorldPos_)
{
}

void ColliderViewer::attachPoint()
{
}

void ColliderViewer::detachPoint()
{
}

void ColliderViewer::draw(Renderer &renderer_, Camera &cam_)
{
    if (!m_shown)
        return;

    for (auto &gp : m_colliderGroups)
    {
        for (auto &cld : gp.m_colliders)
        {
            renderer_.fillRectangle({cld.m_pos.x + m_animPos.x, cld.m_pos.y + m_animPos.y}, {cld.m_size.x, cld.m_size.y}, {255, 100, 100, 100}, cam_);
            renderer_.drawRectangle({cld.m_pos.x + m_animPos.x, cld.m_pos.y + m_animPos.y}, {cld.m_size.x, cld.m_size.y}, {255, 100, 100, 100}, cam_);
        }
    }
}
