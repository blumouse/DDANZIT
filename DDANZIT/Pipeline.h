#pragma once
class Pipeline
{
public:
	virtual void Init();
	virtual void Update();
	virtual void LateUpdate();
	virtual void Close();
	// 를 상위에서 갈무리해서 실행하는거지

	virtual ~Pipeline();	// 이게 있어야한다고..
};

