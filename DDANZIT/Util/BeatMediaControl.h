#pragma once

#include "INC_Windows.h"
#include "DefineOption.h"

#include <mfapi.h>
#include <mfplay.h>
#include <mfreadwrite.h>
#include <xaudio2.h>

#include <string>
#include <vector>
#include <memory>

#pragma comment(lib, "mf.lib")
#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "mfplay.lib")
#pragma comment(lib, "mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")
#pragma comment(lib, "xaudio2.lib")







//    4. MP3 BGM 재생
//    gMedia.PlayMp3(BeatMp3Id::Title, true);
//    
//    두 번째 인자 true는 처음부터 다시 재생한다는 뜻입니다.
//    
//    gMedia.PlayMp3(BeatMp3Id::Stage01, true);
//    
//    일시정지:
//    
//    gMedia.PauseMp3(BeatMp3Id::Stage01);
//    
//    정지:
//    
//    gMedia.StopMp3(BeatMp3Id::Stage01);
//    
//    볼륨 조절 :
//    
//    gMedia.SetMp3Volume(BeatMp3Id::Stage01, 0.5f);
//    
//    재생 위치 확인 :
//    
//    double current = gMedia.GetMp3PositionSeconds(BeatMp3Id::Stage01);
//    double length = gMedia.GetMp3LengthSeconds(BeatMp3Id::Stage01);
//    
//    리듬게임 판정 타이밍이면 보통 이렇게 씁니다.
//    
//    double musicTime = gMedia.GetMp3PositionSeconds(BeatMp3Id::Stage01);
//    
//    // musicTime 기준으로 노트 판정
//    5. MP4 재생
//    gMedia.PlayMp4(BeatMp4Id::Intro, true);
//    
//    일시정지:
//    
//    gMedia.PauseMp4(BeatMp4Id::Intro);
//    
//    정지:
//    
//    gMedia.StopMp4(BeatMp4Id::Intro);
//    
//    볼륨 조절 :
//    
//    gMedia.SetMp4Volume(BeatMp4Id::Intro, 0.8f);
//    
//    재생 위치 확인 :
//    
//    double videoTime = gMedia.GetMp4PositionSeconds(BeatMp4Id::Intro);
//    
//    윈도우 크기 변경이나 화면 갱신이 필요할 때 :
//    
//    gMedia.UpdateMp4Video(BeatMp4Id::Intro);
//    
//    예를 들어 WM_SIZE에서 호출할 수 있습니다.
//    
//    case WM_SIZE:
//    {
//        gMedia.UpdateMp4Video(BeatMp4Id::Intro);
//        break;
//    }
//    6. 효과음 재생
//    
//    효과음은 겹쳐 재생 가능하게 되어 있습니다.
//    
//    gMedia.PlaySfx(BeatSfxId::ButtonClick);
//    
//    볼륨 지정 :
//    
//    gMedia.PlaySfx(BeatSfxId::HitPerfect, 1.0f);
//    
//    같은 효과음을 중첩 재생 허용 :
//    
//    gMedia.PlaySfx(BeatSfxId::HitPerfect, 1.0f, true);
//    
//    기존 재생 중인 같은 효과음을 끊고 새로 재생 :
//    
//    gMedia.PlaySfx(BeatSfxId::HitPerfect, 1.0f, false);
//    
//    특정 효과음 정지 :
//    
//    gMedia.StopSfx(BeatSfxId::HitPerfect);
//    
//    전체 효과음 정지 :
//    
//    gMedia.StopAllSfx();











int BeatToIndex(MusicIndex id);
int BeatToIndex(VideoIndex id);
int BeatToIndex(SFXIndex id);

std::wstring BeatResolveResourcePath(const std::wstring& relativePath);

class BeatMfPlayer;

class BeatMfPlayerEventSink
{
public:
    virtual ~BeatMfPlayerEventSink() {}
    virtual void OnMfPlayerPlaybackEnded(BeatMfPlayer* player) = 0;
};

class BeatMediaFoundationContext
{
public:
    BeatMediaFoundationContext();
    ~BeatMediaFoundationContext();

    BeatMediaFoundationContext(const BeatMediaFoundationContext&) = delete;
    BeatMediaFoundationContext& operator=(const BeatMediaFoundationContext&) = delete;

    bool Initialize();
    void Shutdown();

    bool IsInitialized() const;
    HRESULT LastError() const;

private:
    bool m_comInitialized;
    bool m_mfStarted;
    bool m_initialized;

    HRESULT m_lastError;
};

class BeatXAudioContext
{
public:
    BeatXAudioContext();
    ~BeatXAudioContext();

    BeatXAudioContext(const BeatXAudioContext&) = delete;
    BeatXAudioContext& operator=(const BeatXAudioContext&) = delete;

    bool Initialize();
    void Shutdown();

    IXAudio2* Engine() const;
    IXAudio2MasteringVoice* MasterVoice() const;

    bool IsInitialized() const;
    HRESULT LastError() const;

private:
    IXAudio2* m_engine;
    IXAudio2MasteringVoice* m_masterVoice;

    bool m_initialized;
    HRESULT m_lastError;
};

class BeatMfPlayer
{
public:
    BeatMfPlayer();
    ~BeatMfPlayer();

    BeatMfPlayer(const BeatMfPlayer&) = delete;
    BeatMfPlayer& operator=(const BeatMfPlayer&) = delete;

    bool Open(
        const std::wstring& relativePath,
        HWND videoWindow = nullptr,
        BeatMfPlayerEventSink* eventSink = nullptr
    );

    void Close();

    bool Play(bool restart = false);
    bool Pause();
    void Stop();

    bool SetPositionSeconds(double seconds);

    double PositionSeconds() const;
    double LengthSeconds() const;

    bool UpdateVideo();
    bool SetVolume(float volume);

    bool IsOpened() const;
    bool HasAudio() const;
    bool HasVideo() const;
    bool IsPlaybackEnded() const;

    HRESULT LastError() const;
    const std::wstring& SourcePath() const;

private:
    friend class BeatMfPlayerCallback;

    void ReleasePlayer();
    void NotifyPlaybackEnded();

private:
    IMFPMediaPlayer* m_player;
    IMFPMediaPlayerCallback* m_callback;
    BeatMfPlayerEventSink* m_eventSink;

    HWND m_videoWindow;
    std::wstring m_sourcePath;

    bool m_opened;
    bool m_hasAudio;
    bool m_hasVideo;
    bool m_playbackEnded;

    HRESULT m_lastError;
};

class BeatSfxClip
{
private:
    struct PlayingVoice;

public:
    BeatSfxClip();
    ~BeatSfxClip();

    BeatSfxClip(const BeatSfxClip&) = delete;
    BeatSfxClip& operator=(const BeatSfxClip&) = delete;

    bool Load(IXAudio2* xaudio, const std::wstring& relativePath);
    void Unload();

    bool Play(float volume = 1.0f, bool allowOverlap = true);
    void StopAll();

    bool IsLoaded() const;
    HRESULT LastError() const;
    const std::wstring& SourcePath() const;

private:
    void CleanupFinishedVoices();

private:
    IXAudio2* m_xaudio;

    std::wstring m_sourcePath;

    WAVEFORMATEX m_waveFormat;
    std::vector<BYTE> m_pcmBuffer;

    std::vector<PlayingVoice*> m_playingVoices;

    bool m_loaded;
    HRESULT m_lastError;
};

class BeatMediaSystem : private BeatMfPlayerEventSink
{
public:
    static BeatMediaSystem& Instance();
    ~BeatMediaSystem();

    BeatMediaSystem(const BeatMediaSystem&) = delete;
    BeatMediaSystem& operator=(const BeatMediaSystem&) = delete;

    // parentWindow is the main game window.
    // The system creates an internal child window for MP4 playback.
    bool Initialize(
        HWND parentWindow,
        const wchar_t* mp3FilePath[],
        int mp3Count,
        const wchar_t* mp4FilePath[],
        int mp4Count,
        const wchar_t* sfxFilePath[],
        int sfxCount
    );

    void Shutdown();

    HWND GetVideoWindow() const;
    bool ResizeVideoWindow();
    bool SetVideoWindowRect(int x, int y, int width, int height);
    void ShowVideoWindow(bool show);

    BeatMfPlayer* GetMp3(MusicIndex id);
    BeatMfPlayer* GetMp4(VideoIndex id);
    BeatSfxClip* GetSfx(SFXIndex id);

    const BeatMfPlayer* GetMp3(MusicIndex id) const;
    const BeatMfPlayer* GetMp4(VideoIndex id) const;
    const BeatSfxClip* GetSfx(SFXIndex id) const;

    bool PlayMp3(MusicIndex id, bool restart = true);
    bool PauseMp3(MusicIndex id);
    void StopMp3(MusicIndex id);

    bool SetMp3Volume(MusicIndex id, float volume);
    bool SetMp3PositionSeconds(MusicIndex id, double seconds);

    double GetMp3PositionSeconds(MusicIndex id) const;
    double GetMp3LengthSeconds(MusicIndex id) const;

    bool PlayMp4(VideoIndex id, bool restart = true);
    bool PauseMp4(VideoIndex id);
    void StopMp4(VideoIndex id);

    bool SetMp4Volume(VideoIndex id, float volume);
    bool SetMp4PositionSeconds(VideoIndex id, double seconds);
    bool UpdateMp4Video(VideoIndex id);

    double GetMp4PositionSeconds(VideoIndex id) const;
    double GetMp4LengthSeconds(VideoIndex id) const;

    bool PlaySfx(SFXIndex id, float volume = 1.0f, bool allowOverlap = true);
    void StopSfx(SFXIndex id);

    void StopAllMp3();
    void StopAllMp4();
    void StopAllSfx();

    bool IsInitialized() const;
    HRESULT LastError() const;

private:
    BeatMediaSystem();

    bool CreateVideoChildWindow(HWND parentWindow);
    void DestroyVideoChildWindow();

    bool LoadMp3Table(const wchar_t* filePath[], int count);
    bool LoadMp4Table(const wchar_t* filePath[], int count, HWND videoWindow);
    bool LoadSfxTable(const wchar_t* filePath[], int count);

    bool IsValidMp3Id(MusicIndex id) const;
    bool IsValidMp4Id(VideoIndex id) const;
    bool IsValidSfxId(SFXIndex id) const;

    void OnMfPlayerPlaybackEnded(BeatMfPlayer* player) override;
    void HideVideoAndRefreshParent();

private:
    BeatMediaFoundationContext m_mfContext;
    BeatXAudioContext m_xaudioContext;

    std::vector<std::unique_ptr<BeatMfPlayer>> m_mp3Players;

    // MP4 uses one actual MFPlay player only.
    // Paths are kept in a vector and the selected video is opened at PlayMp4 time.
    std::vector<std::wstring> m_mp4FilePaths;
    std::unique_ptr<BeatMfPlayer> m_mp4Player;
    VideoIndex m_currentVideoIndex;

    std::vector<std::unique_ptr<BeatSfxClip>> m_sfxClips;

    HWND m_parentWindow;
    HWND m_videoWindow;

    bool m_videoWindowVisible;
    bool m_initialized;
    HRESULT m_lastError;
};
