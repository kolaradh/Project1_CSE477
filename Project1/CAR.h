#pragma once
#include "CAudioNode.h"

class CAR : public CAudioNode
{
public:
    CAR();
    virtual ~CAR() {}

    void SetSource(CAudioNode* source) { m_source = source; }
    void Start();
    bool Generate();

    void SetAttack(double attack) { m_attack = attack; }
    void SetRelease(double release) { m_release = release; }
    void SetDuration(double duration) { m_duration = duration; }

private:
    CAudioNode* m_source = nullptr;
    double m_time = 0.0;
    double m_attack = 0.05;
    double m_release = 0.05;
    double m_duration = 0.25;
    bool m_active = false;
};
