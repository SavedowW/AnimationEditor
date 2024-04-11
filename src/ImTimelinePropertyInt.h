#ifndef IM_TIMELINE_PROPERTY_INT_H_
#define IM_TIMELINE_PROPERTY_INT_H_

#include "TimelineProperty.h"
#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_sdlrenderer2.h"
#include "Vector2.h"

class ImTimelinePropertyInt
{
public:
    ImTimelinePropertyInt();

    void setProps(TimelineProperty<int> *prop_);
    void setLimits(const int &min_, const int &max_);
    void clampAll();
    void clampID(int id_);

    void proceed(int currentFrame_);

    void rebuildList();

protected:
    TimelineProperty<int> *m_prop;
    std::vector<std::pair<uint32_t, int>> m_pairs;

    bool m_limited = false;
    int m_min;
    int m_max;

};

#endif