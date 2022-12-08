#include "system.h"

System::System(uint numParticles, const QVector3D& origin, float size)
    : numParticles(numParticles), origin(origin), size(size)
{
    initParams();
    initialize();
}

System::~System()
{
    finalize();
}

void System::initParams()
{
    
}

void System::initialize()
{
    hPos = new float[numParticles * 3]();
    hVel = new float[numParticles * 3]();
    hAccel = new float[numParticles * 3]();


}

void System::finalize()
{
}
