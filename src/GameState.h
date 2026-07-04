#pragma once

class Renderer;

class GameState {
public:
    virtual ~GameState() = default;

    // Called when the state is pushed onto the engine's stack or set as active
    virtual void OnEnter() {}
    
    // Called when the state is removed or replaced
    virtual void OnExit() {}

    virtual void ProcessInput() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void Render(Renderer* renderer) = 0;
};
