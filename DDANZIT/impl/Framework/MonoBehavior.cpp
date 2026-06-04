#include "MonoBehavior.h"

#include "DDANZIT_Core.h"
#include "GameObject.h"
#include "Scene.h"


#pragma region Constructor

MonoBehavior::MonoBehavior(GameObject* pGameObject) : Component(pGameObject), isInUpdateList(false)
{

}

MonoBehavior::MonoBehavior(const MonoBehavior& other) : Component(other), isInUpdateList(false)
{

}

#pragma endregion



#pragma region Clone

Component* MonoBehavior::Clone() const
{
	return new MonoBehavior(*this);
}

#pragma endregion



#pragma region Methods

void MonoBehavior::SetActive(bool newActive)
{
	// 이미 그상태, 상관없이 토글은 해주고 호출은 안함
	if (parentActive == newActive)
	{
		_active = newActive;
		return;
	}

	if (_active == newActive)
		return;

	_active = newActive;

	// ㅇㄴ 생성/파괴시에만 순서를 따르고, 평소 온오프에서는 즉시 실행 및 상태변경

	// TODO: 이거 런타임 아니면 실행 안할듯?
	// 런타임검사... 이런것들이 제법 지금 있을법한데
	if (newActive /*== true*/)
	{
		if (activeOnEnable)
		{
			OnEnable();

			// 업데이트는 다음 프레임부터 ..뭐 시점이 Start 이전이면 아닐수도 있고
			if (!isInUpdateList)
				DDANZIT_Core::RegisterUpdateExecLists(this);
		}
	}
	else
	{
		if (activeOnDisable)
		{
			OnDisable();

			// 이 경우는 또 다름! 즉시 중지 (삭제말고 플래그 체크로, 여기선 안함)
		}
	}
}


void MonoBehavior::SetParentActive(bool newActive)
{
	if (_active == newActive)
	{
		parentActive = newActive;
		return;
	}

	if (parentActive == newActive)
		return;

	parentActive = newActive;


	if (newActive /*== true*/)
	{
		if (activeOnEnable)
		{
			OnEnable();

			if (!isInUpdateList)
				DDANZIT_Core::RegisterUpdateExecLists(this);
		}
	}
	else
	{
		if (activeOnDisable)
		{
			OnDisable();
		}
	}
}

#pragma endregion