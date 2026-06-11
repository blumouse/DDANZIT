#pragma once

class Collider2D;


// 컴포넌트에서 사용하는 라이프사이클
// 그냥 모노비헤이비어로 상속시켜서 쓰는게 맞겠다 접근을 모두 거기서
class Lifecycle
{
public:
	friend class DDANZIT_Core;
	friend class Scene;
	friend class GameObject;
	friend class MonoBehavior;

#pragma region Constructor

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

	bool activeFixedUpdate = false;

	bool activeOnTriggerEnter2D = false;
	bool activeOnTriggerStay2D = false; 
	bool activeOnTriggerExit2D = false;

	bool activeUpdate = false;
	bool activeLateUpdate = false;

	bool activeOnDisable = false;
	bool activeOnDestroy = false;


	// 생성자에 넣어야한다
protected:
	void EnableAwake() { activeAwake = true; }
	void EnableOnEnable() { activeOnEnable = true; }
	void EnableStart() { activeStart = true; }

	void EnableFixedUpdate() { activeFixedUpdate = true; }

	void EnableOnTriggerEnter2D() { activeOnTriggerEnter2D = true; }
	void EnableOnTriggerStay2D() { activeOnTriggerStay2D = true; }
	void EnableOnTriggerExit2D() { activeOnTriggerExit2D = true; }

	void EnableUpdate() { activeUpdate = true; }
	void EnableLateUpdate() { activeLateUpdate = true; }

	void EnableOnDisable() { activeOnDisable = true; }
	void EnableOnDestroy() { activeOnDestroy = true; }


public:
	virtual void Awake() {}
	virtual void OnEnable() {}
	virtual void Start() {}

	virtual void FixedUpdate() {}

	virtual void OnTriggerEnter2D(Collider2D* collision) {}
	virtual void OnTriggerStay2D(Collider2D* collision) {}
	virtual void OnTriggerExit2D(Collider2D* collision) {}

	// virtual void OnCollision...();
	// virtual void WaitForFixedUpdate();
	// virtual void OnMouse...();

	virtual void Update() {}
	virtual void LateUpdate() {}

	virtual void OnDisable() {}
	virtual void OnDestroy() {}

#pragma endregion

};