#pragma once
#include "Pipeline.h"
#include "Component.h"

class ObjectLogic : public Component, public Pipeline
{
public:
	void Init() override;
	void Update() override;
	void LateUpdate() override;
	void Close() override;

protected:
};

