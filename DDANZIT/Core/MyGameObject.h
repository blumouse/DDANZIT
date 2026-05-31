#pragma once

class Component;

class GameObjectBase
{
protected:
    const int MAX_COMPONENT_SIZE = 20;

    Component** ppComponents = nullptr;
    int componentsSize = 0;


    GameObjectBase();
    GameObjectBase(const GameObjectBase&) = delete;

public:
    virtual ~GameObjectBase();

    Component** GetComponents() const { return ppComponents; }
    int GetComponentsSize() const { return componentsSize; }


    virtual void Awake();
    virtual void Start();
    virtual void Update(float deltaTime);
    virtual void FixedUpdate();

    virtual void OnDestroy();




    // 다 필요없지 않나..?

    //virtual void Render(HDC hdc) = 0;



//    void SetPosition(float x, float y) { m_pos = { x, y }; }
//    void SetDirection(Vector2f dir) { m_dir = dir; }
//    void SetSpeed(float speed) { m_speed = speed; }
//    void SetName(const char* name);
//
//    ObjectType Type() const { return m_type; }
//
//    const char* GetName() const { return m_name; }
//
//    Vector2f GetPosition() const { return m_pos; }
//    Vector2f GetDirection() const { return m_dir; }
//
//    float GetSpeed() const { return m_speed; }
//
//    void SetWidth(int width) { m_width = width; }
//    void SetHeight(int height) { m_height = height; }
//
//protected:
//
//    void Move(float deltaTime)
//    {
//        m_pos.x += m_dir.x * m_speed * deltaTime;
//        m_pos.y += m_dir.y * m_speed * deltaTime;
//    }
//
//    ObjectType m_type;
//
//    Vector2f m_pos = { 0.0f, 0.0f };
//    Vector2f m_dir = { 0.0f, 0.0f }; // 방향 (단위 벡터)
//
//    int m_width;
//    int m_height;
//
//    float m_speed = 0.0f; // 속력
//
//    char m_name[OBJECT_NAME_LEN_MAX] = "";
};

//class GameObject : public GameObjectBase
//{
//    using ColliderCircle = learning::ColliderCircle;
//    using ColliderBox = learning::ColliderBox;
//    using BitmapInfo = renderHelp::BitmapInfo;
//
//public:
//    GameObject(const GameObject&) = delete;
//    GameObject(ObjectType type) : GameObjectBase(type) {}
//    ~GameObject() override;
//
//    void Update(float deltaTime) override;
//    void Render(HDC hdc) override;
//
//    void SetColliderCircle(float radius);
//    void SetColliderBox(float halfWidth, float halfHeight);
//
//    void SetColor(int r = 255, int g = 0, int b = 0);
//
//    void SetBitmapInfo(BitmapInfo* bitmapInfo);
//
//    // Collider
//    ColliderCircle* m_pColliderCircle = nullptr;
//    ColliderBox* m_pColliderBox = nullptr;
//
//    bool isIntersect = false;
//
//    GameObject** intersectColliders = nullptr;
//    int IntersectColliders_index = 0;
//
//protected:
//
//    void DrawBitmap(HDC hdc);
//    void UpdateFrame(float deltaTime);
//
//    // Bitmap 정보
//    BitmapInfo* m_pBitmapInfo = nullptr;
//
//    // 점진적으로 예쁘게 고쳐 보아요.
//    struct FrameFPos
//    {
//        int x;
//        int y;
//    };
//    // 프레임 정보: 왜 14개냐고 물으시면 셌다고 밖에...:)
//    FrameFPos m_frameXY[14] = { { 0, 0 }, };
//    int m_frameWidth = 0;
//    int m_frameHeight = 0;
//    int m_frameIndex = 0;
//    int m_frameCount = 14; // 프레임 수
//
//    float m_frameTime = 0.0f;
//    float m_frameDuration = 100.0f; // 임의 설정
//
//    void DrawCollider(HDC hdc);
//
//    void Move(float deltaTime);
//
//    int m_r;
//    int m_g;
//    int m_b;
//};