#include "Script.h"

#include "DDANZITEngine.h"

Script::Script(GameObject* gameObject) : MonoBehavior(gameObject)
{
	EnableAwake();
}

// 인스턴트 하고싶으면 복사생성자 / 클론 함수 재정의도 하셔야 해요

void Script::Awake()
{
	auto comp = gameObject()->GetComponent<Script>();
}