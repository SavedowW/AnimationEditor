#include "ColliderViewer.h"
#include <array>

void ColliderViewer::setFile(const std::string &filepath_)
{
    m_colliderGroups = m_db->getColliders(filepath_);
    m_shown = true;
    m_filepath = filepath_;
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

void ColliderViewer::setCurrentFrame(int currentFrame_)
{
    m_currentFrame = currentFrame_;
}

void ColliderViewer::setDuration(int duration_)
{
    m_duration = duration_;
}

void ColliderViewer::proceed()
{
    if (!m_shown)
        return;

    ImGui::SetNextWindowPos(ImVec2(1250, 520));
    ImGui::SetNextWindowSize(ImVec2(340, 360));

    ImGui::Begin("Collider editing", &m_shown);

    static char groupname[1024];
    ImGui::InputText("Group Name", groupname, 1024);
    ImGui::SameLine();
    if (ImGui::Button("+##group", {18, 18}))
    {
        collidergroupdata cgd;
        cgd.m_id = m_db->createColliderGroup(groupname, m_filepath);
        cgd.m_name = groupname;
        m_colliderGroups.push_back(cgd);
        cgd.m_dirtyflag = false;
    }


    for (int gpi = 0; gpi < m_colliderGroups.size(); ++gpi)
    {
        auto &gp = m_colliderGroups[gpi];
        ImGui::SeparatorText(gp.m_name.c_str());

        if (ImGui::ColorEdit3(("Color##cld_" + std::to_string(gpi)).c_str(), gp.m_color))
        {
            gp.m_dirtyflag = true;
        }

        if (ImGui::Button(("+##cld_" + std::to_string(gpi)).c_str(), {18, 18}))
        {
            colliderdata cld;
            cld.m_id = m_db->getColliderManager().createCollider(m_colliderGroups[gpi].m_id, cld);
            cld.m_dirtyflag = false;
            m_colliderGroups[gpi].m_colliders.push_back(cld);
        }
        ImGui::SameLine();
        if (ImGui::Button(("Delete##cg_" + std::to_string(gpi)).c_str()))
        {
            m_db->getColliderManager().deleteColliderGroup(gp.m_id);
            m_colliderGroups.erase(m_colliderGroups.begin() + gpi);
        }

        for (int cldi = 0; cldi < gp.m_colliders.size(); ++cldi)
        {
            auto &cld = gp.m_colliders[cldi];

            if (ImGui::Button(("Delete##_" + std::to_string(gpi) + "_" + std::to_string(cldi)).c_str()))
            {
                m_db->getColliderManager().deleteCollider(cld.m_id);
                gp.m_colliders.erase(gp.m_colliders.begin() + cldi);
            }

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

            ImGui::PushItemWidth(200);
            if (ImGui::SliderInt((std::string("Begin##_") + std::to_string(gpi) + "_" + std::to_string(cldi)).c_str(), &cld.m_firstFrame, 0, cld.m_lastFrame))
                cld.m_dirtyflag = true;
            ImGui::PushItemWidth(200);
            if (ImGui::SliderInt((std::string("End##_") + std::to_string(gpi) + "_" + std::to_string(cldi)).c_str(), &cld.m_lastFrame, cld.m_firstFrame, m_duration - 1))
                cld.m_dirtyflag = true;
        }
    }

    if (ImGui::Button("Update all##colliders"))
    {
        for (auto &grp : m_colliderGroups)
        {
            for (auto &cld : grp.m_colliders)
            {
                if (cld.m_dirtyflag)
                {
                    m_db->getColliderManager().updateCollider(cld);
                    cld.m_dirtyflag = false;
                }
            }

            if (grp.m_dirtyflag)
            {
                m_db->getColliderManager().updateColliderGroup(grp);
                grp.m_dirtyflag = false;
            }
        }
    }

    ImGui::End();
}

void ColliderViewer::updateMousePos(const Vector2<float> mouseWorldPos_)
{
    if (!m_shown)
        return;

    if (!m_attached)
    {
        float closestDir = -1;
        m_closestCollider = nullptr;
        std::array<Vector2<int>, 4> ptids{Vector2{0, 0}, Vector2{1, 0}, Vector2{0, 1}, Vector2{1, 1}};
        for (auto &cg : m_colliderGroups)
        {
            for (auto &cld : cg.m_colliders)
            {
                if (cld.m_firstFrame > m_currentFrame || cld.m_lastFrame < m_currentFrame)
                    continue;

                for (const auto &ptid : ptids)
                {
                    auto point = m_animPos + cld.m_pos + cld.m_size.mulComponents(ptid);
                    auto dst = (point - mouseWorldPos_).getSqLen();
                    if ((closestDir == -1 || dst < closestDir) && dst < m_maxRange)
                    {
                        closestDir = dst;
                        m_closestCollider = &cld;
                        m_attachedPoint = ptid;
                    }
                }
            }
        }
    }

    else if (m_attached && m_closestCollider)
    {
        if (m_attachedPoint.x == 0)
        {
            auto delta = mouseWorldPos_.x - m_animPos.x - m_closestCollider->m_pos.x;
            m_closestCollider->m_pos.x = mouseWorldPos_.x - m_animPos.x;
            m_closestCollider->m_size.x -= delta;
        }
        else
            m_closestCollider->m_size.x = mouseWorldPos_.x - m_animPos.x - m_closestCollider->m_pos.x;

        if (m_attachedPoint.y == 0)
        {
            auto delta = mouseWorldPos_.y - m_animPos.y - m_closestCollider->m_pos.y;
            m_closestCollider->m_pos.y = mouseWorldPos_.y - m_animPos.y;
            m_closestCollider->m_size.y -= delta;
        }
        else
            m_closestCollider->m_size.y = mouseWorldPos_.y - m_animPos.y - m_closestCollider->m_pos.y;
    }
}

bool ColliderViewer::attachPoint()
{
    if (!m_shown)
        return false;

    if (m_closestCollider && (m_closestCollider->m_firstFrame > m_currentFrame || m_closestCollider->m_lastFrame < m_currentFrame))
        return false;

    m_attached = true;

    return m_closestCollider;
}

void ColliderViewer::detachPoint()
{
    if (!m_shown)
        return;

    m_attached = false;

    if (m_closestCollider)
    {
        if (m_closestCollider->m_size.x < 0)
        {
            m_closestCollider->m_pos.x += m_closestCollider->m_size.x;
            m_closestCollider->m_size.x = abs(m_closestCollider->m_size.x);
        }

        if (m_closestCollider->m_size.y < 0)
        {
            m_closestCollider->m_pos.y += m_closestCollider->m_size.y;
            m_closestCollider->m_size.y = abs(m_closestCollider->m_size.y);
        }

        m_closestCollider->m_dirtyflag = true;
    }

    m_closestCollider = nullptr;
}

void ColliderViewer::draw(Renderer &renderer_, Camera &cam_)
{
    if (!m_shown)
        return;

    for (auto &gp : m_colliderGroups)
    {
        for (auto &cld : gp.m_colliders)
        {
            if (cld.m_firstFrame > m_currentFrame || cld.m_lastFrame < m_currentFrame)
                    continue;

            renderer_.fillRectangle({cld.m_pos.x + m_animPos.x, cld.m_pos.y + m_animPos.y}, {cld.m_size.x, cld.m_size.y}, {Uint8(255 * gp.m_color[0]), Uint8(255 * gp.m_color[1]), Uint8(255 * gp.m_color[2]), 100}, cam_);
            renderer_.drawRectangle({cld.m_pos.x + m_animPos.x, cld.m_pos.y + m_animPos.y}, {cld.m_size.x, cld.m_size.y}, {Uint8(255 * gp.m_color[0]), Uint8(255 * gp.m_color[1]), Uint8(255 * gp.m_color[2]), 100}, cam_);
            if (&cld == m_closestCollider)
            {
                auto attachedPointWorld = m_animPos + m_closestCollider->m_pos + m_closestCollider->m_size.mulComponents(m_attachedPoint);
                renderer_.fillRectangle(attachedPointWorld - Vector2{5.0f, 5.0f}, {10.0f, 10.0f}, {Uint8(255 * gp.m_color[0]), Uint8(255 * gp.m_color[1]), Uint8(255 * gp.m_color[2]), 100}, cam_);
            }
        }
    }
}
