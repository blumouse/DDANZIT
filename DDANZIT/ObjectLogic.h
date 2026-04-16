#pragma once
#include "Pipeline.h"
class ObjectLogic : Pipeline
{
public:
	void Init();
	void Update();
	void LateUpdate();
	void Close();
};

