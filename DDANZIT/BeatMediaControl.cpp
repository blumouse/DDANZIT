//수정할것
#include "BeatMediaControl.h"

#include <propvarutil.h>
#include <cwctype>
#include <cstring>

int BeatToIndex(MusicIndex id)
{
    return static_cast<int>(id);
}

int BeatToIndex(VideoIndex id)
{
    return static_cast<int>(id);
}

int BeatToIndex(SFXIndex id)
{
    return static_cast<int>(id);
}

namespace
{
    constexpr double kMediaFoundationTimeScale = 10000000.0;

    bool FileExists(const std::wstring& path)
    {
        DWORD attr = GetFileAttributesW(path.c_str());
        return attr != INVALID_FILE_ATTRIBUTES && (attr & FILE_ATTRIBUTE_DIRECTORY) == 0;
    }

    std::wstring JoinPath(const std::wstring& left, const std::wstring& right)
    {
        if (left.empty())
            return right;

        wchar_t last = left[left.size() - 1];
        if (last == L'\\' || last == L'/')
            return left + right;

        return left + L"\\" + right;
    }

    std::wstring CurrentDirectory()
    {
        wchar_t buffer[MAX_PATH] = {};
        DWORD length = GetCurrentDirectoryW(MAX_PATH, buffer);

        if (length == 0 || length >= MAX_PATH)
            return L"";

        return buffer;
    }

    std::wstring ModuleDirectory()
    {
        wchar_t buffer[MAX_PATH] = {};
        DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH);

        if (length == 0 || length >= MAX_PATH)
            return L"";

        std::wstring path(buffer);
        size_t slash = path.find_last_of(L"\\/");

        if (slash == std::wstring::npos)
            return L"";

        return path.substr(0, slash);
    }

    bool IsAbsolutePath(const std::wstring& path)
    {
        bool drivePath =
            path.size() >= 3 &&
            path[1] == L':' &&
            (path[2] == L'\\' || path[2] == L'/');

        bool uncPath =
            path.size() >= 2 &&
            path[0] == L'\\' &&
            path[1] == L'\\';

        return drivePath || uncPath;
    }

    std::wstring ToFileUrl(const std::wstring& path)
    {
        std::wstring url = L"file:///";

        for (wchar_t ch : path)
        {
            if (ch == L'\\')
                url += L'/';
            else if (ch == L' ')
                url += L"%20";
            else
                url += ch;
        }

        return url;
    }

    double PropVariantToSeconds(PROPVARIANT& value)
    {
        if (value.vt == VT_I8)
            return static_cast<double>(value.hVal.QuadPart) / kMediaFoundationTimeScale;

        if (value.vt == VT_UI8)
            return static_cast<double>(value.uhVal.QuadPart) / kMediaFoundationTimeScale;

        return 0.0;
    }

    bool IsExtension(const std::wstring& path, const wchar_t* ext)
    {
        size_t dot = path.find_last_of(L'.');
        if (dot == std::wstring::npos)
            return false;

        std::wstring fileExt = path.substr(dot);
        std::wstring target(ext);

        for (wchar_t& ch : fileExt)
            ch = static_cast<wchar_t>(towlower(ch));

        for (wchar_t& ch : target)
            ch = static_cast<wchar_t>(towlower(ch));

        return fileExt == target;
    }

    class BeatSfxVoiceCallback final : public IXAudio2VoiceCallback
    {
    public:
        explicit BeatSfxVoiceCallback(bool* active)
            : m_active(active)
        {
        }

        void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32) override {}
        void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override {}
        void STDMETHODCALLTYPE OnStreamEnd() override {}
        void STDMETHODCALLTYPE OnBufferStart(void*) override {}
        void STDMETHODCALLTYPE OnLoopEnd(void*) override {}
        void STDMETHODCALLTYPE OnVoiceError(void*, HRESULT) override {}

        void STDMETHODCALLTYPE OnBufferEnd(void*) override
        {
            if (m_active != nullptr)
                *m_active = false;
        }

    private:
        bool* m_active;
    };
}

class BeatMfPlayerCallback final : public IMFPMediaPlayerCallback
{
public:
    explicit BeatMfPlayerCallback(BeatMfPlayer* owner)
        : m_refCount(1)
        , m_owner(owner)
    {
    }

    STDMETHODIMP QueryInterface(REFIID riid, void** ppvObject) override
    {
        if (ppvObject == nullptr)
            return E_POINTER;

        *ppvObject = nullptr;

        if (riid == __uuidof(IUnknown) || riid == __uuidof(IMFPMediaPlayerCallback))
        {
            *ppvObject = static_cast<IMFPMediaPlayerCallback*>(this);
            AddRef();
            return S_OK;
        }

        return E_NOINTERFACE;
    }

    STDMETHODIMP_(ULONG) AddRef() override
    {
        return InterlockedIncrement(&m_refCount);
    }

    STDMETHODIMP_(ULONG) Release() override
    {
        ULONG count = InterlockedDecrement(&m_refCount);
        if (count == 0)
            delete this;
        return count;
    }

    void STDMETHODCALLTYPE OnMediaPlayerEvent(MFP_EVENT_HEADER* pEventHeader) override
    {
        if (pEventHeader == nullptr)
            return;

        if (FAILED(pEventHeader->hrEvent))
            return;

        if (pEventHeader->eEventType == MFP_EVENT_TYPE_PLAYBACK_ENDED)
        {
            if (m_owner != nullptr)
                m_owner->NotifyPlaybackEnded();
        }
    }

private:
    volatile LONG m_refCount;
    BeatMfPlayer* m_owner;
};

struct BeatSfxClip::PlayingVoice
{
    IXAudio2SourceVoice* voice;
    IXAudio2VoiceCallback* callback;
    bool active;

    PlayingVoice()
        : voice(nullptr)
        , callback(nullptr)
        , active(false)
    {
    }
};

std::wstring BeatResolveResourcePath(const std::wstring& relativePath)
{
    if (IsAbsolutePath(relativePath) && FileExists(relativePath))
        return relativePath;

    std::wstring cwd = CurrentDirectory();
    std::wstring module = ModuleDirectory();

    std::vector<std::wstring> candidates;
    candidates.push_back(JoinPath(cwd, relativePath));
    candidates.push_back(JoinPath(JoinPath(cwd, L"DDANZIT"), relativePath));
    candidates.push_back(JoinPath(module, relativePath));
    candidates.push_back(JoinPath(JoinPath(module, L"..\\..\\DDANZIT"), relativePath));
    candidates.push_back(JoinPath(JoinPath(module, L"..\\..\\Resource"), relativePath));

    for (const std::wstring& candidate : candidates)
    {
        if (FileExists(candidate))
            return candidate;
    }

    return relativePath;
}

BeatMediaFoundationContext::BeatMediaFoundationContext()
    : m_comInitialized(false)
    , m_mfStarted(false)
    , m_initialized(false)
    , m_lastError(S_OK)
{
}

BeatMediaFoundationContext::~BeatMediaFoundationContext()
{
    Shutdown();
}

bool BeatMediaFoundationContext::Initialize()
{
    if (m_initialized)
        return true;

    HRESULT result = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

    if (SUCCEEDED(result))
    {
        m_comInitialized = true;
    }
    else if (result != RPC_E_CHANGED_MODE)
    {
        m_lastError = result;
        return false;
    }

    result = MFStartup(MF_VERSION);

    if (FAILED(result))
    {
        m_lastError = result;

        if (m_comInitialized)
        {
            CoUninitialize();
            m_comInitialized = false;
        }

        return false;
    }

    m_mfStarted = true;
    m_initialized = true;
    m_lastError = S_OK;

    return true;
}

void BeatMediaFoundationContext::Shutdown()
{
    if (m_mfStarted)
    {
        MFShutdown();
        m_mfStarted = false;
    }

    if (m_comInitialized)
    {
        CoUninitialize();
        m_comInitialized = false;
    }

    m_initialized = false;
}

bool BeatMediaFoundationContext::IsInitialized() const
{
    return m_initialized;
}

HRESULT BeatMediaFoundationContext::LastError() const
{
    return m_lastError;
}

BeatXAudioContext::BeatXAudioContext()
    : m_engine(nullptr)
    , m_masterVoice(nullptr)
    , m_initialized(false)
    , m_lastError(S_OK)
{
}

BeatXAudioContext::~BeatXAudioContext()
{
    Shutdown();
}

bool BeatXAudioContext::Initialize()
{
    if (m_initialized)
        return true;

    HRESULT result = XAudio2Create(&m_engine, 0, XAUDIO2_DEFAULT_PROCESSOR);

    if (FAILED(result))
    {
        m_lastError = result;
        return false;
    }

    result = m_engine->CreateMasteringVoice(&m_masterVoice);

    if (FAILED(result))
    {
        m_lastError = result;
        m_engine->Release();
        m_engine = nullptr;
        return false;
    }

    m_initialized = true;
    m_lastError = S_OK;

    return true;
}

void BeatXAudioContext::Shutdown()
{
    if (m_masterVoice != nullptr)
    {
        m_masterVoice->DestroyVoice();
        m_masterVoice = nullptr;
    }

    if (m_engine != nullptr)
    {
        m_engine->Release();
        m_engine = nullptr;
    }

    m_initialized = false;
}

IXAudio2* BeatXAudioContext::Engine() const
{
    return m_engine;
}

IXAudio2MasteringVoice* BeatXAudioContext::MasterVoice() const
{
    return m_masterVoice;
}

bool BeatXAudioContext::IsInitialized() const
{
    return m_initialized;
}

HRESULT BeatXAudioContext::LastError() const
{
    return m_lastError;
}

BeatMfPlayer::BeatMfPlayer()
    : m_player(nullptr)
    , m_callback(nullptr)
    , m_eventSink(nullptr)
    , m_videoWindow(nullptr)
    , m_opened(false)
    , m_hasAudio(false)
    , m_hasVideo(false)
    , m_playbackEnded(false)
    , m_lastError(S_OK)
{
}

BeatMfPlayer::~BeatMfPlayer()
{
    Close();
}

bool BeatMfPlayer::Open(const std::wstring& relativePath, HWND videoWindow, BeatMfPlayerEventSink* eventSink)
{
    Close();

    m_videoWindow = videoWindow;
    m_eventSink = eventSink;
    m_sourcePath = BeatResolveResourcePath(relativePath);

    m_callback = new BeatMfPlayerCallback(this);

    HRESULT result = MFPCreateMediaPlayer(
        nullptr,
        FALSE,
        MFP_OPTION_NONE,
        m_callback,
        videoWindow,
        &m_player
    );

    if (FAILED(result))
    {
        m_lastError = result;
        Close();
        return false;
    }

    IMFPMediaItem* item = nullptr;
    std::wstring url = ToFileUrl(m_sourcePath);

    result = m_player->CreateMediaItemFromURL(
        url.c_str(),
        TRUE,
        0,
        &item
    );

    if (FAILED(result))
    {
        m_lastError = result;
        Close();
        return false;
    }

    result = m_player->SetMediaItem(item);
    item->Release();
    item = nullptr;

    if (FAILED(result))
    {
        m_lastError = result;
        Close();
        return false;
    }

    m_opened = true;
    m_hasAudio = IsExtension(m_sourcePath, L".mp3") || IsExtension(m_sourcePath, L".mp4");
    m_hasVideo = IsExtension(m_sourcePath, L".mp4");
    m_playbackEnded = false;
    m_lastError = S_OK;

    return true;
}

void BeatMfPlayer::Close()
{
    Stop();
    ReleasePlayer();

    m_sourcePath.clear();
    m_videoWindow = nullptr;
    m_eventSink = nullptr;

    m_opened = false;
    m_hasAudio = false;
    m_hasVideo = false;
    m_playbackEnded = false;
}

bool BeatMfPlayer::Play(bool restart)
{
    if (!m_opened || m_player == nullptr)
        return false;

    m_playbackEnded = false;

    if (restart)
        SetPositionSeconds(0.0);

    HRESULT result = m_player->Play();

    if (FAILED(result))
    {
        m_lastError = result;
        return false;
    }

    m_lastError = S_OK;
    return true;
}

bool BeatMfPlayer::Pause()
{
    if (!m_opened || m_player == nullptr)
        return false;

    HRESULT result = m_player->Pause();

    if (FAILED(result))
    {
        m_lastError = result;
        return false;
    }

    m_lastError = S_OK;
    return true;
}

void BeatMfPlayer::Stop()
{
    if (m_player != nullptr)
        m_player->Stop();
}

bool BeatMfPlayer::SetPositionSeconds(double seconds)
{
    if (!m_opened || m_player == nullptr)
        return false;

    PROPVARIANT position;
    PropVariantInit(&position);

    position.vt = VT_I8;
    position.hVal.QuadPart = static_cast<LONGLONG>(seconds * kMediaFoundationTimeScale);

    HRESULT result = m_player->SetPosition(MFP_POSITIONTYPE_100NS, &position);

    PropVariantClear(&position);

    if (FAILED(result))
    {
        m_lastError = result;
        return false;
    }

    m_lastError = S_OK;
    return true;
}

double BeatMfPlayer::PositionSeconds() const
{
    if (!m_opened || m_player == nullptr)
        return 0.0;

    PROPVARIANT position;
    PropVariantInit(&position);

    HRESULT result = m_player->GetPosition(MFP_POSITIONTYPE_100NS, &position);

    if (FAILED(result))
    {
        PropVariantClear(&position);
        return 0.0;
    }

    double seconds = PropVariantToSeconds(position);

    PropVariantClear(&position);
    return seconds;
}

double BeatMfPlayer::LengthSeconds() const
{
    if (!m_opened || m_player == nullptr)
        return 0.0;

    PROPVARIANT duration;
    PropVariantInit(&duration);

    HRESULT result = m_player->GetDuration(MFP_POSITIONTYPE_100NS, &duration);

    if (FAILED(result))
    {
        PropVariantClear(&duration);
        return 0.0;
    }

    double seconds = PropVariantToSeconds(duration);

    PropVariantClear(&duration);
    return seconds;
}

bool BeatMfPlayer::UpdateVideo()
{
    if (!m_opened || m_player == nullptr)
        return false;

    HRESULT result = m_player->UpdateVideo();

    if (FAILED(result))
    {
        m_lastError = result;
        return false;
    }

    m_lastError = S_OK;
    return true;
}

bool BeatMfPlayer::SetVolume(float volume)
{
    if (!m_opened || m_player == nullptr)
        return false;

    if (volume < 0.0f)
        volume = 0.0f;

    if (volume > 1.0f)
        volume = 1.0f;

    HRESULT result = m_player->SetVolume(volume);

    if (FAILED(result))
    {
        m_lastError = result;
        return false;
    }

    m_lastError = S_OK;
    return true;
}

bool BeatMfPlayer::IsOpened() const
{
    return m_opened;
}

bool BeatMfPlayer::HasAudio() const
{
    return m_hasAudio;
}

bool BeatMfPlayer::HasVideo() const
{
    return m_hasVideo;
}

bool BeatMfPlayer::IsPlaybackEnded() const
{
    return m_playbackEnded;
}

HRESULT BeatMfPlayer::LastError() const
{
    return m_lastError;
}

const std::wstring& BeatMfPlayer::SourcePath() const
{
    return m_sourcePath;
}

void BeatMfPlayer::ReleasePlayer()
{
    if (m_player != nullptr)
    {
        m_player->Shutdown();
        m_player->Release();
        m_player = nullptr;
    }

    if (m_callback != nullptr)
    {
        m_callback->Release();
        m_callback = nullptr;
    }
}

void BeatMfPlayer::NotifyPlaybackEnded()
{
    m_playbackEnded = true;

    if (m_eventSink != nullptr)
        m_eventSink->OnMfPlayerPlaybackEnded(this);
}

BeatSfxClip::BeatSfxClip()
    : m_xaudio(nullptr)
    , m_loaded(false)
    , m_lastError(S_OK)
{
    ZeroMemory(&m_waveFormat, sizeof(m_waveFormat));
}

BeatSfxClip::~BeatSfxClip()
{
    Unload();
}

bool BeatSfxClip::Load(IXAudio2* xaudio, const std::wstring& relativePath)
{
    Unload();

    if (xaudio == nullptr)
    {
        m_lastError = E_POINTER;
        return false;
    }

    m_xaudio = xaudio;
    m_sourcePath = BeatResolveResourcePath(relativePath);

    IMFSourceReader* reader = nullptr;
    IMFMediaType* outputType = nullptr;
    IMFMediaType* currentType = nullptr;

    std::wstring url = ToFileUrl(m_sourcePath);

    HRESULT result = MFCreateSourceReaderFromURL(
        url.c_str(),
        nullptr,
        &reader
    );

    if (FAILED(result))
    {
        m_lastError = result;
        return false;
    }

    result = MFCreateMediaType(&outputType);

    if (SUCCEEDED(result))
        result = outputType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);

    if (SUCCEEDED(result))
        result = outputType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);

    if (SUCCEEDED(result))
    {
        result = reader->SetCurrentMediaType(
            MF_SOURCE_READER_FIRST_AUDIO_STREAM,
            nullptr,
            outputType
        );
    }

    if (SUCCEEDED(result))
    {
        result = reader->GetCurrentMediaType(
            MF_SOURCE_READER_FIRST_AUDIO_STREAM,
            &currentType
        );
    }

    if (FAILED(result))
    {
        if (currentType != nullptr) currentType->Release();
        if (outputType != nullptr) outputType->Release();
        if (reader != nullptr) reader->Release();

        m_lastError = result;
        return false;
    }

    UINT32 channels = MFGetAttributeUINT32(currentType, MF_MT_AUDIO_NUM_CHANNELS, 0);
    UINT32 samplesPerSecond = MFGetAttributeUINT32(currentType, MF_MT_AUDIO_SAMPLES_PER_SECOND, 0);
    UINT32 bitsPerSample = MFGetAttributeUINT32(currentType, MF_MT_AUDIO_BITS_PER_SAMPLE, 0);

    if (channels == 0 || samplesPerSecond == 0 || bitsPerSample == 0)
    {
        currentType->Release();
        outputType->Release();
        reader->Release();

        m_lastError = E_FAIL;
        return false;
    }

    m_waveFormat.wFormatTag = WAVE_FORMAT_PCM;
    m_waveFormat.nChannels = static_cast<WORD>(channels);
    m_waveFormat.nSamplesPerSec = samplesPerSecond;
    m_waveFormat.wBitsPerSample = static_cast<WORD>(bitsPerSample);
    m_waveFormat.nBlockAlign = static_cast<WORD>((m_waveFormat.nChannels * m_waveFormat.wBitsPerSample) / 8);
    m_waveFormat.nAvgBytesPerSec = m_waveFormat.nSamplesPerSec * m_waveFormat.nBlockAlign;
    m_waveFormat.cbSize = 0;

    while (true)
    {
        DWORD streamIndex = 0;
        DWORD flags = 0;
        LONGLONG timestamp = 0;
        IMFSample* sample = nullptr;

        result = reader->ReadSample(
            MF_SOURCE_READER_FIRST_AUDIO_STREAM,
            0,
            &streamIndex,
            &flags,
            &timestamp,
            &sample
        );

        if (FAILED(result))
            break;

        if (flags & MF_SOURCE_READERF_ENDOFSTREAM)
            break;

        if (sample != nullptr)
        {
            IMFMediaBuffer* buffer = nullptr;
            result = sample->ConvertToContiguousBuffer(&buffer);

            if (SUCCEEDED(result))
            {
                BYTE* data = nullptr;
                DWORD maxLength = 0;
                DWORD currentLength = 0;

                result = buffer->Lock(&data, &maxLength, &currentLength);

                if (SUCCEEDED(result))
                {
                    size_t oldSize = m_pcmBuffer.size();
                    m_pcmBuffer.resize(oldSize + currentLength);
                    memcpy(m_pcmBuffer.data() + oldSize, data, currentLength);
                    buffer->Unlock();
                }

                buffer->Release();
            }

            sample->Release();

            if (FAILED(result))
                break;
        }
    }

    currentType->Release();
    outputType->Release();
    reader->Release();

    if (FAILED(result) || m_pcmBuffer.empty())
    {
        m_pcmBuffer.clear();
        m_lastError = FAILED(result) ? result : E_FAIL;
        return false;
    }

    m_loaded = true;
    m_lastError = S_OK;

    return true;
}

void BeatSfxClip::Unload()
{
    StopAll();

    m_xaudio = nullptr;
    m_sourcePath.clear();
    m_pcmBuffer.clear();

    ZeroMemory(&m_waveFormat, sizeof(m_waveFormat));

    m_loaded = false;
}

bool BeatSfxClip::Play(float volume, bool allowOverlap)
{
    if (!m_loaded || m_xaudio == nullptr || m_pcmBuffer.empty())
        return false;

    CleanupFinishedVoices();

    if (!allowOverlap)
        StopAll();

    if (volume < 0.0f)
        volume = 0.0f;

    IXAudio2SourceVoice* sourceVoice = nullptr;

    PlayingVoice* playing = new PlayingVoice();
    playing->active = true;
    playing->callback = new BeatSfxVoiceCallback(&playing->active);

    HRESULT result = m_xaudio->CreateSourceVoice(
        &sourceVoice,
        &m_waveFormat,
        0,
        XAUDIO2_DEFAULT_FREQ_RATIO,
        playing->callback
    );

    if (FAILED(result))
    {
        delete playing->callback;
        delete playing;

        m_lastError = result;
        return false;
    }

    XAUDIO2_BUFFER buffer = {};
    buffer.AudioBytes = static_cast<UINT32>(m_pcmBuffer.size());
    buffer.pAudioData = m_pcmBuffer.data();
    buffer.Flags = XAUDIO2_END_OF_STREAM;

    result = sourceVoice->SubmitSourceBuffer(&buffer);

    if (FAILED(result))
    {
        sourceVoice->DestroyVoice();
        delete playing->callback;
        delete playing;

        m_lastError = result;
        return false;
    }

    sourceVoice->SetVolume(volume);

    result = sourceVoice->Start(0);

    if (FAILED(result))
    {
        sourceVoice->DestroyVoice();
        delete playing->callback;
        delete playing;

        m_lastError = result;
        return false;
    }

    playing->voice = sourceVoice;
    m_playingVoices.push_back(playing);

    m_lastError = S_OK;
    return true;
}

void BeatSfxClip::StopAll()
{
    for (PlayingVoice* playing : m_playingVoices)
    {
        if (playing == nullptr)
            continue;

        if (playing->voice != nullptr)
        {
            playing->voice->Stop(0);
            playing->voice->FlushSourceBuffers();
            playing->voice->DestroyVoice();
            playing->voice = nullptr;
        }

        delete playing->callback;
        playing->callback = nullptr;

        delete playing;
    }

    m_playingVoices.clear();
}

bool BeatSfxClip::IsLoaded() const
{
    return m_loaded;
}

HRESULT BeatSfxClip::LastError() const
{
    return m_lastError;
}

const std::wstring& BeatSfxClip::SourcePath() const
{
    return m_sourcePath;
}

void BeatSfxClip::CleanupFinishedVoices()
{
    for (auto it = m_playingVoices.begin(); it != m_playingVoices.end(); )
    {
        PlayingVoice* playing = *it;

        if (playing == nullptr || !playing->active)
        {
            if (playing != nullptr)
            {
                if (playing->voice != nullptr)
                {
                    playing->voice->DestroyVoice();
                    playing->voice = nullptr;
                }

                delete playing->callback;
                playing->callback = nullptr;

                delete playing;
            }

            it = m_playingVoices.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

BeatMediaSystem::BeatMediaSystem()
    : m_mp4Player(new BeatMfPlayer())
    , m_currentVideoIndex(static_cast<VideoIndex>(-1))
    , m_parentWindow(nullptr)
    , m_videoWindow(nullptr)
    , m_videoWindowVisible(false)
    , m_initialized(false)
    , m_lastError(S_OK)
{
}

BeatMediaSystem& BeatMediaSystem::Instance()
{
    static BeatMediaSystem instance;
    return instance;
}

BeatMediaSystem::~BeatMediaSystem()
{
    Shutdown();
}

bool BeatMediaSystem::Initialize(
    HWND parentWindow,
    const wchar_t* mp3FilePath[],
    int mp3Count,
    const wchar_t* mp4FilePath[],
    int mp4Count,
    const wchar_t* sfxFilePath[],
    int sfxCount
)
{
    Shutdown();

    m_parentWindow = parentWindow;

    if (!m_mfContext.Initialize())
    {
        m_lastError = m_mfContext.LastError();
        return false;
    }

    if (!m_xaudioContext.Initialize())
    {
        m_lastError = m_xaudioContext.LastError();
        Shutdown();
        return false;
    }

    if (!CreateVideoChildWindow(parentWindow))
    {
        Shutdown();
        return false;
    }

    if (!LoadMp3Table(mp3FilePath, mp3Count))
    {
        Shutdown();
        return false;
    }

    if (!LoadMp4Table(mp4FilePath, mp4Count, m_videoWindow))
    {
        Shutdown();
        return false;
    }

    if (!LoadSfxTable(sfxFilePath, sfxCount))
    {
        Shutdown();
        return false;
    }

    m_initialized = true;
    m_lastError = S_OK;

    return true;
}

void BeatMediaSystem::Shutdown()
{
    StopAllSfx();
    StopAllMp4();
    StopAllMp3();

    m_sfxClips.clear();
    m_mp4FilePaths.clear();

    if (m_mp4Player != nullptr)
        m_mp4Player->Close();

    m_mp3Players.clear();

    DestroyVideoChildWindow();

    m_xaudioContext.Shutdown();
    m_mfContext.Shutdown();

    m_parentWindow = nullptr;
    m_videoWindow = nullptr;
    m_videoWindowVisible = false;
    m_initialized = false;
    m_lastError = S_OK;
}

HWND BeatMediaSystem::GetVideoWindow() const
{
    return m_videoWindow;
}

bool BeatMediaSystem::CreateVideoChildWindow(HWND parentWindow)
{
    if (parentWindow == nullptr)
    {
        m_lastError = E_INVALIDARG;
        return false;
    }

    if (m_videoWindow != nullptr)
        return true;

    m_parentWindow = parentWindow;

    LONG_PTR parentStyle = GetWindowLongPtrW(parentWindow, GWL_STYLE);
    parentStyle |= WS_CLIPCHILDREN;
    SetWindowLongPtrW(parentWindow, GWL_STYLE, parentStyle);

    RECT rc = {};
    GetClientRect(parentWindow, &rc);

    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    m_videoWindow = CreateWindowExW(
        0,
        L"STATIC",
        L"",
        WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
        0,
        0,
        width,
        height,
        parentWindow,
        nullptr,
        GetModuleHandleW(nullptr),
        nullptr
    );

    if (m_videoWindow == nullptr)
    {
        m_lastError = HRESULT_FROM_WIN32(GetLastError());
        return false;
    }

    ShowWindow(m_videoWindow, SW_HIDE);
    m_videoWindowVisible = false;

    return true;
}

void BeatMediaSystem::DestroyVideoChildWindow()
{
    if (m_videoWindow != nullptr)
    {
        DestroyWindow(m_videoWindow);
        m_videoWindow = nullptr;
    }
}

bool BeatMediaSystem::ResizeVideoWindow()
{
    if (m_parentWindow == nullptr || m_videoWindow == nullptr)
        return false;

    RECT rc = {};
    GetClientRect(m_parentWindow, &rc);

    return SetVideoWindowRect(
        0,
        0,
        rc.right - rc.left,
        rc.bottom - rc.top
    );
}

bool BeatMediaSystem::SetVideoWindowRect(int x, int y, int width, int height)
{
    if (m_videoWindow == nullptr)
        return false;

    BOOL result = MoveWindow(
        m_videoWindow,
        x,
        y,
        width,
        height,
        TRUE
    );

    if (!result)
    {
        m_lastError = HRESULT_FROM_WIN32(GetLastError());
        return false;
    }

    if (m_mp4Player != nullptr && m_mp4Player->IsOpened())
        m_mp4Player->UpdateVideo();

    return true;
}

void BeatMediaSystem::ShowVideoWindow(bool show)
{
    if (m_videoWindow == nullptr)
        return;

    if (show)
    {
        ResizeVideoWindow();
        ShowWindow(m_videoWindow, SW_SHOW);
        SetWindowPos(
            m_videoWindow,
            HWND_TOP,
            0,
            0,
            0,
            0,
            SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW
        );
        UpdateWindow(m_videoWindow);
        m_videoWindowVisible = true;
    }
    else
    {
        ShowWindow(m_videoWindow, SW_HIDE);
        m_videoWindowVisible = false;
        HideVideoAndRefreshParent();
    }
}

void BeatMediaSystem::HideVideoAndRefreshParent()
{
    if (m_videoWindow != nullptr)
        ShowWindow(m_videoWindow, SW_HIDE);

    m_videoWindowVisible = false;

    if (m_parentWindow != nullptr)
    {
        InvalidateRect(m_parentWindow, nullptr, TRUE);
        UpdateWindow(m_parentWindow);
    }
}

BeatMfPlayer* BeatMediaSystem::GetMp3(MusicIndex id)
{
    if (!IsValidMp3Id(id))
        return nullptr;

    return m_mp3Players[BeatToIndex(id)].get();
}

BeatMfPlayer* BeatMediaSystem::GetMp4(VideoIndex id)
{
    if (!IsValidMp4Id(id))
        return nullptr;

    return m_mp4Player.get();
}

BeatSfxClip* BeatMediaSystem::GetSfx(SFXIndex id)
{
    if (!IsValidSfxId(id))
        return nullptr;

    return m_sfxClips[BeatToIndex(id)].get();
}

const BeatMfPlayer* BeatMediaSystem::GetMp3(MusicIndex id) const
{
    if (!IsValidMp3Id(id))
        return nullptr;

    return m_mp3Players[BeatToIndex(id)].get();
}

const BeatMfPlayer* BeatMediaSystem::GetMp4(VideoIndex id) const
{
    if (!IsValidMp4Id(id))
        return nullptr;

    return m_mp4Player.get();
}

const BeatSfxClip* BeatMediaSystem::GetSfx(SFXIndex id) const
{
    if (!IsValidSfxId(id))
        return nullptr;

    return m_sfxClips[BeatToIndex(id)].get();
}

bool BeatMediaSystem::PlayMp3(MusicIndex id, bool restart)
{
    BeatMfPlayer* player = GetMp3(id);
    return player != nullptr && player->Play(restart);
}

bool BeatMediaSystem::PauseMp3(MusicIndex id)
{
    BeatMfPlayer* player = GetMp3(id);
    return player != nullptr && player->Pause();
}

void BeatMediaSystem::StopMp3(MusicIndex id)
{
    BeatMfPlayer* player = GetMp3(id);

    if (player != nullptr)
        player->Stop();
}

bool BeatMediaSystem::SetMp3Volume(MusicIndex id, float volume)
{
    BeatMfPlayer* player = GetMp3(id);
    return player != nullptr && player->SetVolume(volume);
}

bool BeatMediaSystem::SetMp3PositionSeconds(MusicIndex id, double seconds)
{
    BeatMfPlayer* player = GetMp3(id);
    return player != nullptr && player->SetPositionSeconds(seconds);
}

double BeatMediaSystem::GetMp3PositionSeconds(MusicIndex id) const
{
    const BeatMfPlayer* player = GetMp3(id);
    return player != nullptr ? player->PositionSeconds() : 0.0;
}

double BeatMediaSystem::GetMp3LengthSeconds(MusicIndex id) const
{
    const BeatMfPlayer* player = GetMp3(id);
    return player != nullptr ? player->LengthSeconds() : 0.0;
}

bool BeatMediaSystem::PlayMp4(VideoIndex id, bool restart)
{
    if (!IsValidMp4Id(id))
    {
        m_lastError = E_INVALIDARG;
        return false;
    }

    int index = BeatToIndex(id);

    if (m_mp4FilePaths[index].empty())
    {
        m_lastError = E_INVALIDARG;
        return false;
    }

    if (m_videoWindow == nullptr)
    {
        m_lastError = E_HANDLE;
        return false;
    }

    if (m_mp4Player == nullptr)
        m_mp4Player.reset(new BeatMfPlayer());

    m_mp4Player->Stop();
    m_mp4Player->Close();

    ResizeVideoWindow();
    ShowVideoWindow(true);

    if (!m_mp4Player->Open(m_mp4FilePaths[index], m_videoWindow, this))
    {
        m_lastError = m_mp4Player->LastError();
        ShowVideoWindow(false);
        return false;
    }

    m_currentVideoIndex = id;

    if (!m_mp4Player->Play(restart))
    {
        m_lastError = m_mp4Player->LastError();
        m_mp4Player->Close();
        ShowVideoWindow(false);
        return false;
    }

    m_mp4Player->UpdateVideo();
    m_lastError = S_OK;

    return true;
}

bool BeatMediaSystem::PauseMp4(VideoIndex id)
{
    if (!IsValidMp4Id(id) || m_mp4Player == nullptr)
        return false;

    return m_mp4Player->Pause();
}

void BeatMediaSystem::StopMp4(VideoIndex id)
{
    if (!IsValidMp4Id(id))
        return;

    if (m_mp4Player != nullptr)
    {
        m_mp4Player->Stop();
        m_mp4Player->Close();
    }

    m_currentVideoIndex = static_cast<VideoIndex>(-1);
    ShowVideoWindow(false);
}

bool BeatMediaSystem::SetMp4Volume(VideoIndex id, float volume)
{
    if (!IsValidMp4Id(id) || m_mp4Player == nullptr)
        return false;

    return m_mp4Player->SetVolume(volume);
}

bool BeatMediaSystem::SetMp4PositionSeconds(VideoIndex id, double seconds)
{
    if (!IsValidMp4Id(id) || m_mp4Player == nullptr)
        return false;

    return m_mp4Player->SetPositionSeconds(seconds);
}

bool BeatMediaSystem::UpdateMp4Video(VideoIndex id)
{
    if (!IsValidMp4Id(id) || m_mp4Player == nullptr)
        return false;

    return m_mp4Player->UpdateVideo();
}

double BeatMediaSystem::GetMp4PositionSeconds(VideoIndex id) const
{
    if (!IsValidMp4Id(id) || m_mp4Player == nullptr)
        return 0.0;

    return m_mp4Player->PositionSeconds();
}

double BeatMediaSystem::GetMp4LengthSeconds(VideoIndex id) const
{
    if (!IsValidMp4Id(id) || m_mp4Player == nullptr)
        return 0.0;

    return m_mp4Player->LengthSeconds();
}

bool BeatMediaSystem::PlaySfx(SFXIndex id, float volume, bool allowOverlap)
{
    BeatSfxClip* clip = GetSfx(id);
    return clip != nullptr && clip->Play(volume, allowOverlap);
}

void BeatMediaSystem::StopSfx(SFXIndex id)
{
    BeatSfxClip* clip = GetSfx(id);

    if (clip != nullptr)
        clip->StopAll();
}

void BeatMediaSystem::StopAllMp3()
{
    for (auto& player : m_mp3Players)
    {
        if (player)
            player->Stop();
    }
}

void BeatMediaSystem::StopAllMp4()
{
    if (m_mp4Player != nullptr)
    {
        m_mp4Player->Stop();
        m_mp4Player->Close();
    }

    m_currentVideoIndex = static_cast<VideoIndex>(-1);
    ShowVideoWindow(false);
}

void BeatMediaSystem::StopAllSfx()
{
    for (auto& clip : m_sfxClips)
    {
        if (clip)
            clip->StopAll();
    }
}

bool BeatMediaSystem::IsInitialized() const
{
    return m_initialized;
}

HRESULT BeatMediaSystem::LastError() const
{
    return m_lastError;
}

bool BeatMediaSystem::LoadMp3Table(const wchar_t* filePath[], int count)
{
    if (filePath == nullptr || count <= 0)
        return true;

    m_mp3Players.reserve(count);

    for (int i = 0; i < count; ++i)
    {
        std::unique_ptr<BeatMfPlayer> player(new BeatMfPlayer());

        if (filePath[i] != nullptr)
        {
            if (!player->Open(filePath[i], nullptr, nullptr))
            {
                m_lastError = player->LastError();
                return false;
            }
        }

        m_mp3Players.push_back(std::move(player));
    }

    return true;
}

bool BeatMediaSystem::LoadMp4Table(const wchar_t* filePath[], int count, HWND videoWindow)
{
    (void)videoWindow;

    m_mp4FilePaths.clear();

    if (filePath == nullptr || count <= 0)
        return true;

    m_mp4FilePaths.reserve(count);

    for (int i = 0; i < count; ++i)
    {
        if (filePath[i] != nullptr)
            m_mp4FilePaths.push_back(filePath[i]);
        else
            m_mp4FilePaths.push_back(L"");
    }

    if (m_mp4Player == nullptr)
        m_mp4Player.reset(new BeatMfPlayer());

    return true;
}

bool BeatMediaSystem::LoadSfxTable(const wchar_t* filePath[], int count)
{
    if (filePath == nullptr || count <= 0)
        return true;

    IXAudio2* xaudio = m_xaudioContext.Engine();

    if (xaudio == nullptr)
    {
        m_lastError = E_POINTER;
        return false;
    }

    m_sfxClips.reserve(count);

    for (int i = 0; i < count; ++i)
    {
        std::unique_ptr<BeatSfxClip> clip(new BeatSfxClip());

        if (filePath[i] != nullptr)
        {
            if (!clip->Load(xaudio, filePath[i]))
            {
                m_lastError = clip->LastError();
                return false;
            }
        }

        m_sfxClips.push_back(std::move(clip));
    }

    return true;
}

bool BeatMediaSystem::IsValidMp3Id(MusicIndex id) const
{
    int index = BeatToIndex(id);
    return index >= 0 && index < static_cast<int>(m_mp3Players.size());
}

bool BeatMediaSystem::IsValidMp4Id(VideoIndex id) const
{
    int index = BeatToIndex(id);
    return index >= 0 && index < static_cast<int>(m_mp4FilePaths.size());
}

bool BeatMediaSystem::IsValidSfxId(SFXIndex id) const
{
    int index = BeatToIndex(id);
    return index >= 0 && index < static_cast<int>(m_sfxClips.size());
}

void BeatMediaSystem::OnMfPlayerPlaybackEnded(BeatMfPlayer* player)
{
    if (m_mp4Player != nullptr && player == m_mp4Player.get())
    {
        if (m_mp4Player != nullptr)
            m_mp4Player->Close();

        m_currentVideoIndex = static_cast<VideoIndex>(-1);
        HideVideoAndRefreshParent();
    }
}
