#pragma once

typedef struct Pos2D {
	float XPos;
	float YPos;
};

typedef struct Scale2D {
	float XScale;
	float YScale;
};

class Shape {

};

class None : Shape {

};

class Sqaure : Shape {

};

// 등등..

class ObjectData
{
public:
	Pos2D pos;
	Shape shape;
	float rotate;
	Scale2D scale;
};

// 오브젝트가 공통으로 가질법한 데이터? 속성?
// 모양을.. enum처럼 갈아끼우고싶은데 사전세트로 <- 확장도 돼야하지 않나?
// 아 이것도 상속시키면되자나 ?되나
// 기본 (탬플릿이나 업캐스팅)큐같은걸 두고 거기다가 넣게할까