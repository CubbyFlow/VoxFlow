// Author : snowapril

#include <cuda_runtime.h>
#include <VoxFlow/Editor/VoxFlow.hpp>

namespace VoxFlow
{
VoxFlow::VoxFlow()
{
}
VoxFlow::~VoxFlow()
{
}

bool VoxFlow::initialize()
{
    auto solver = CUDAPCISPHSolver3::Builder().MakeShared();
    solver->SetDragCoefficient(0.0f);
    solver->SetRestitutionCoefficient(0.0f);
    solver->SetViscosityCoefficient(0.1f);
    solver->SetPseudoViscosityCoefficient(10.0f);
    solver->SetIsUsingFixedSubTimeSteps(true);
    solver->SetNumberOfFixedSubTimeSteps(1);

    auto particles = solver->SPHSystemData();
    particles->SetTargetSpacing(targetSpacing);
    particles->SetRelativeKernelRadius(1.8f);

    // Seed particles
    BoundingBox3D vol(Vector3D(), Vector3D(0.5, 0.5, 0.5));
    vol.Expand(-targetSpacing);
    Array1<Vector3D> rawPoints;
    GridPointGenerator3 generator;
    generator.Generate(vol, targetSpacing, &rawPoints);
    Array1<float4> hostData(rawPoints.Length());
    for (std::size_t i = 0; i < rawPoints.Length(); ++i)
    {
        Vector3F rp = rawPoints[i].CastTo<float>();
        hostData[i] = make_float4(rp[0], rp[1], rp[2], 0.0f);
    }
    CUDAArray1<float4> deviceData(hostData);
    particles->AddParticles(deviceData);
    printf("Number of particles: %zu\n", particles->NumberOfParticles());
}

void VoxFlow::updateSolver()
{
    for (Frame frame(0, 1.0 / 60.0); frame.index < 10; ++frame)
    {
        solver->Update(frame);
    }
}
}  // namespace VoxFlow

#endif