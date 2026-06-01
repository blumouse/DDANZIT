#pragma once
#include "GameObject.h"
#include "Component.h"

#pragma region Types

typedef struct Pos2D {
	float xPos;
	float yPos;
};

typedef struct Scale2D {
	float xScale;
	float yScale;
};

// 등등.. 근데 enum 있다니까 걍 그게 나을듯
// 근데! 결국에는 클래스정의가 필요할텐데! 음.. 아닌가

enum Shape
{
	None,
	Point,
	Square,
	Ellipse,
};


#pragma endregion


// 기본제공 컴포넌트 역할을 한다
class ObjectData : public Component
{
public:
	Pos2D pos;
	Shape shape;
	float rotate;
	Scale2D scale;
	short depth;

	ObjectData(GameObject* gameObject);
	~ObjectData();
};

// 오브젝트가 공통으로 가질법한 데이터? 속성?
// 모양을.. enum처럼 갈아끼우고싶은데 사전세트로 <- 확장도 돼야하지 않나?
// 아 이것도 상속시키면되자나 ?되나
// 기본 (탬플릿이나 업캐스팅)큐같은걸 두고 거기다가 넣게할까
