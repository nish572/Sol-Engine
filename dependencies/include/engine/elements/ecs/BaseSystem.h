#pragma once

class BaseSystem
{
public:
    //Make destructor virtual to ensure that derived classes' destructors are called
    //This is necessary because the ECS framework uses polymorphism
    //default keyword specifies that the default destructor should be used
    virtual ~BaseSystem() = default;
    //update is called every frame
    virtual void update(double deltaTime) = 0;
    //fixedUpdate is called every fixed timestep
    virtual void fixedUpdate(double fixedTimestep) = 0;
};
