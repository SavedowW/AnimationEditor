#include "LayerViewer.h"

LayerViewer::LayerViewer(const Vector2<float> lvlSize_, Renderer *ren_, EngineAnimation *anim_) :
    m_lvlSize(lvlSize_),
    m_anim(nullptr),
    m_ren(ren_)
{
    setAnim(anim_);
}

void LayerViewer::setAnim(EngineAnimation *anim_)
{
    m_anim = anim_;
    m_layers.clear();

    if (!m_anim)
        return;

    for (size_t i = 0; i < m_anim->m_layers.size(); ++i)
        m_layers.push_back({i, 150.0f});

    m_layers[0].m_alpha = 255.0f;
}

void LayerViewer::proceed()
{
    static int blurRange_ = 2;
    ImGui::InputInt("Pure white blur range", &blurRange_);

    // Name*, [Generate], [Up], [Down], [Alpha]
    if (ImGui::BeginTable("Layer data", 5,  ImGuiTableFlags_Borders | ImGuiTableFlags_SizingStretchSame))
    {
        //ImGui::TableSetupColumn("", ImGuiTableColumnFlags_WidthFixed, 16);
        ImGui::TableSetupColumn("", 0, 100);
        ImGui::TableSetupColumn("", 0, 50);
        ImGui::TableSetupColumn("", 0, 50);
        ImGui::TableSetupColumn("", 0, 50);
        ImGui::TableSetupColumn("", 0, 100);

        for (int i = 0; i < m_layers.size(); ++i)
        {
            auto layerId = m_layers[i].m_layerId;
            auto &layer = m_anim->m_layers[layerId];

            ImGui::TableNextRow();
            
            ImGui::TableSetColumnIndex(0);
            ImGui::Text((layer.m_layerName + (layer.m_isGenerated ? "" : "*")).c_str());

            ImGui::TableSetColumnIndex(1);
            if (ImGui::Button((std::string("Generate##") + std::to_string(i)).c_str()))
            {
                m_anim->generateLayer(m_layers[i].m_layerId, blurRange_, *m_ren);
            }

            ImGui::TableSetColumnIndex(2);
            if (ImGui::Button((std::string("Up##") + std::to_string(i)).c_str()))
            {
                if (i != 0)
                {
                    std::swap(m_layers[i], m_layers[i - 1]);
                }
            }

            ImGui::TableSetColumnIndex(3);
            if (ImGui::Button((std::string("Down##") + std::to_string(i)).c_str()))
            {
                if (i != m_layers.size() - 1)
                {
                    std::swap(m_layers[i], m_layers[i + 1]);
                }
            }

            ImGui::TableSetColumnIndex(4);
            if (ImGui::DragFloat((std::string("Alpha##") + std::to_string(i)).c_str(), &m_layers[i].m_alpha, 1.0f, 0.0f, 255.0f))
            {
            }
        }

        ImGui::EndTable();
    }
}

void LayerViewer::draw(Renderer &renderer_, Camera &cam_, int currentFrame_, float levelOfGround_)
{
    if (m_anim->m_frameCount <= 0)
        return;

    for (auto &layerData : m_layers)
    {
        if (layerData.m_alpha && m_anim->m_layers[layerData.m_layerId].m_isGenerated)
        {
            auto *tex = (*m_anim)(layerData.m_layerId, currentFrame_);
            SDL_SetTextureAlphaMod(tex, layerData.m_alpha);
            renderer_.renderTexture(tex, m_lvlSize.x / 2.0f - m_anim->m_origin.x, levelOfGround_ - m_anim->m_origin.y, m_anim->m_width, m_anim->m_height, cam_, 0, SDL_FLIP_NONE);
        }
    }
}
