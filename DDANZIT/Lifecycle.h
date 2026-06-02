#pragma once

class DDANZIT_Core;
class GameObject;


// 컴포넌트에서 사용하는 라이프사이클
class Lifecycle
{
public:
	friend class GameObject;
	friend class DDANZIT_Core;

	friend bool DDANZIT_Initialize(const wchar_t* windowName, unsigned int width, unsigned int height);
	friend void DDANZIT_Run();
	friend void DDANZIT_Finalize();

#pragma region Construntor

protected:
	Lifecycle() = default;
	Lifecycle(const Lifecycle&) = delete;

public:
	virtual ~Lifecycle() = default;

#pragma endregion


	// 원대한 계획
#pragma region LifeCycles

private:
	bool activeAwake = false;
	bool activeOnEnable = false;
	bool activeStart = false;

	bool activeUpdate = false;
	bool activeFixedUpdate = false;
	bool activeLateUpdate = false;

	bool activeOnDisable = false;
	bool activeOnDestroy = false;


	// 생성자에 넣어야한다
protected:
	void EnableAwake() { activeAwake = true; }
	void EnableOnEnable() { activeOnEnable = true; }
	void EnableStart() { activeStart = true; }

	void EnableFixedUpdate() { activeFixedUpdate = true; }
	void EnableUpdate() { activeUpdate = true; }
	void EnableLateUpdate() { activeLateUpdate = true; }

	void EnableOnDisable() { activeOnDisable = true; }
	void EnableOnDestroy() { activeOnDestroy = true; }


public:
	virtual void Awake() {}
	virtual void OnEnable() {}
	virtual void Start() {}

	virtual void FixedUpdate() {}
	// virtual void OnTrigger...();
	// virtual void OnCollision...();
	// virtual void WaitForFixedUpdate();
	// virtual void OnMouse...();
	virtual void Update() {}
	virtual void LateUpdate() {}

	virtual void OnDisable() {}
	virtual void OnDestroy() {}

#pragma endregion

};