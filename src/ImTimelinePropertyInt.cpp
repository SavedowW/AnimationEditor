#include "ImTimelinePropertyInt.h"

ImTimelinePropertyInt::ImTimelinePropertyInt()
{
}

void ImTimelinePropertyInt::setProps(TimelineProperty<int> *prop_)
{
    m_prop = prop_;

    rebuildList();
}

void ImTimelinePropertyInt::setLimits(const int &min_, const int &max_)
{
    m_limited = true;
    m_min = min_;
    m_max = max_;
}

void ImTimelinePropertyInt::clampAll()
{
    if (!m_limited || !m_prop)
        return;

    for (auto &el : m_pairs)
    {
        el.second = utils::clamp(el.second, m_min, m_max);
    }
}

void ImTimelinePropertyInt::clampID(int id_)
{
    if (!m_limited || !m_prop)
        return;

    m_pairs[id_].second = utils::clamp(m_pairs[id_].second, m_min, m_max);
}

void ImTimelinePropertyInt::proceed(int currentFrame_)
{
    if (ImGui::BeginTable("Frame data", 3,  ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchSame))
    {
        ImGui::TableSetupColumn("Frame", ImGuiTableColumnFlags_WidthFixed, 100);
        ImGui::TableSetupColumn("Sprite", 0, 100);
        ImGui::TableSetupColumn("Delete", 0, 100);
        ImGui::TableHeadersRow();
        for (int i = 0; i < m_pairs.size(); ++i)
        {
            ImGui::TableNextRow();

            ImGui::TableSetColumnIndex(0);
            ImGui::Text(std::to_string(m_pairs[i].first).c_str());

            ImGui::TableSetColumnIndex(1);
            if (ImGui::InputInt((std::string("Frame##") + std::to_string(i)).c_str(), &(m_pairs[i].second)))
            {
                clampID(i);
                m_prop->setPairValue(i, std::move(m_pairs[i].second));
            }

            ImGui::TableSetColumnIndex(2);
            if (ImGui::Button((std::string("Delete##") + std::to_string(i)).c_str()))
            {
                auto res = m_prop->deletePair(i);
                std::cout << "Delete: " << res << std::endl;
                if (res)
                {
                    m_pairs.erase(m_pairs.begin() + i);
                }
            }
        }
        ImGui::EndTable();
    }

    static int nval = 0;
    if (ImGui::Button("Set sprite at current frame"))
    {
        m_prop->addPropertyValue(currentFrame_, std::move(nval));
        rebuildList();
    }
    ImGui::SameLine();
    ImGui::PushItemWidth(100);
    if (ImGui::InputInt("Sprite ID", &nval))
    {
        nval = utils::clamp(nval, m_min, m_max);
    }
}

void ImTimelinePropertyInt::rebuildList()
{
    m_pairs.clear();

    for (int i = 0; i < m_prop->getValuesCount(); ++i)
    {
        auto pair = m_prop->getValuePair(i);
        m_pairs.push_back(pair);

    }
}
